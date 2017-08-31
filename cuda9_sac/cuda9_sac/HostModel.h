#pragma once
#include <map>
#include <functional>
#include <vector>
#include <tuple>
#include "XModel.h"
namespace cudacp {

enum DomType {
	//从零开始连续整数
	DT_STD = 1,
	//离散值
	DT_DP = 0
};

//HostModel的一些信息
struct HostProp {
	int vs_size;
	int ts_size;
};

class HostVar {
public:
	int id;
	int size;
	int *vals;

	HostVar(const int id, const int size) :id(id), size(size) {
		vals = new int[size];
	};

	~HostVar() {
		delete[] vals;
		vals = NULL;
	};
};

class HostTab {
public:
	int id;
	int arity;
	int size;
	int** tuples;
	int* scope;

	HostTab(const int id, const int arity, const int size, int** tps, int* scp)
		:id(id), arity(arity), size(size) {
		scope = new int[arity];
		tuples = new int*[size];

		for (size_t i = 0; i < arity; i++) {
			scope[i] = scp[i];
		}

		for (size_t i = 0; i < size; i++) {
			tuples[i] = new int[arity];
			for (size_t j = 0; j < arity; j++) {
				tuples[i][j] = tps[i][j];
			}
		}
	};

	~HostTab() {
		delete[] scope;
		scope = NULL;

		for (int i = 0; i < size; ++i) {
			delete[] tuples[i];
			tuples[i] = NULL;
		}

		delete[] tuples;
		tuples = NULL;
	};
};

class HostModel {
public:
	HostProp property;
	HostVar** vars;
	HostTab** tabs;
};

class HostDomMap {
public:
	int id;
	std::map<int, int>m;
	DomType type = DT_STD;
	HostDomMap() {};
	void MakeMap(XDom* d) {
		id = d->id;
		for (int i = 0; i < d->size; ++i) {
			m[d->values[i]] = i;
			if (d->values[i] != i)
				type = DT_DP;
		}
	};
	~HostDomMap() {};
};


class HostModelBuilder {
public:
	std::vector<HostDomMap> domms;
	XModel* xm;
	HostModel* hm;
	std::vector<std::vector<int>> tmp_tuples;
	HostModelBuilder(XModel* xmodel, HostModel* hmodel) :xm(xmodel), hm(hmodel) {
		domms.resize(xm->feature.ds_size);

		hm->property.vs_size = xm->feature.vs_size;
		hm->property.ts_size = xm->feature.cs_size;
		hm->vars = new HostVar*[hm->property.vs_size];
		hm->tabs = new HostTab*[hm->property.ts_size];

		generateHostDomMap();
		generateHostVars();

	};

	void GetIntTuple(int idx, std::vector<int> t, XDom** ds) {
		for (int i = (t.size() - 1); i >= 0; --i) {
			t[i] = ds[i]->values[(idx % (ds[i]->size))];
			idx /= (ds[i]->size);
		}
	}

	~HostModelBuilder() {};

private:
	void Equal(std::vector<int>&lh, std::vector<int>& rh) {
		//for (int i = 0; i < lh.size(); ++i)
		//	lh[i] = domms[ds[i]->id].m[rh[i]];
		lh.assign(rh.begin(), rh.end());
	}

	//************************************
	// Method:    IsEqual
	// FullName:  IsEqual
	// Access:    public 
	// Returns:   bool
	// Qualifier: 判断元组是否相等
	// Parameter: const int * lh
	// Parameter: const int * rh
	// Parameter: const int & len	元组长度
	//************************************
	bool IsEqual(const int* lh, std::vector<int> &rh) {
		for (int i = 0; i < rh.size(); ++i)
			if (lh[i] != rh[i])
				return false;

		return true;
	}

	void makeHostTab(HostTab* t, const  XCon *c, const XRel* r, const int sup_size) {
		t->id = c->id;
		t->arity = c->arity;
		t->size = sup_size;
		t->scope = new int[t->arity];
		for (size_t i = 0; i < t->arity; i++)
			t->scope[i] = c->scope[i];

		t->tuples = new int*[t->size];

		for (size_t i = 0; i < t->size; i++) {
			t->tuples[i] = new int[t->arity];
			for (size_t j = 0; j < t->arity; j++) {
				t->tuples[i][j] = tmp_tuples[i][j];
			}
		}

	}

	void generateHostDomMap() {
		for (size_t i = 0; i < xm->feature.ds_size; ++i) {
			domms[i].MakeMap(xm->doms[i]);
		}
	};

	void generateHostVars() {
		for (size_t i = 0; i < xm->feature.vs_size; ++i) {
			hm->vars[i] = new HostVar(i, xm->doms[xm->vars[i]->dom_id]->size);
		}
	};

	void generateHostTab() {
		//for (size_t i = 0; i < xm->feature.cs_size; ++i) {
		//	const XCon* c = xm->cons[i];
		//	if (c->type == EXT) {
		//		for (size_t j = 0; j < c->arity; j++) {
		//			const int var_id = c->scope[j];
		//			const int dom_id = xm->vars[var_id]->dom_id;
		//			if
		//		}
		//	}
		//}
		//若为约束EXT
		for (int i = 0; i < xm->feature.cs_size; ++i) {
			const XCon* c = xm->cons[i];


			if (c->type == EXT) {
				EXTConToTab(c);
			}

			//转标准论域
		//for (int j = 0; j < c->arity; ++j) {
		//	XVar* v = xm->vars[c->scope[j]];
		//	const int dom_id = v->dom_id;
		//	if (domms[dom_id].type == DT_DP) {
		//		for (int k = 0; k < r->size; ++k) {
		//			const int s = r->tuples[k][j];
		//			const int res = domms[v->dom_id].m[s];
		//			r->tuples[k][j] = res;
		//		}
		//	}
		//}
		}
	};

	void EXTConToTab(const XCon *c) {
		const XRel* r = xm->rels[c->rel_id];
		const int i = c->id;
		std::vector<std::tuple<int, DomType>> id_ty(c->arity);
		bool IsAllStd = true;

		//中间变量元组
		std::vector<int> tpl(c->arity);
		for (size_t j = 0; j < c->arity; ++j) {
			const XVar* v = xm->vars[c->scope[j]];
			id_ty[j] = std::make_tuple(v->dom_id, domms[v->dom_id].type);
			IsAllStd &= domms[v->dom_id].type;
		}
		//正表
		if (r->sem == SEM_SUPPORT) {
			hm->tabs[i] = new HostTab(i, c->arity, r->size, r->tuples, c->scope);
		}
		//负表
		else {
			//获取总个数
			int num_all_tpls = 1;
			for (size_t j = 0; j < c->arity; j++) {
				const XDom *d = xm->doms[xm->vars[c->scope[j]]->dom_id];
				num_all_tpls *= d->size;
			}
			//正表元组个数 = 总元组-负元组
			int num_sup_tpls = num_all_tpls - r->size;

			tmp_tuples.resize(num_sup_tpls);

			for (size_t j = 0; j < r->size; j++) {
				tmp_tuples[j].resize(r->arity);
			}

			//通过index获取tuple，一维索引转多维元组
			int k = 0, m = 0;
			for (int j = 0; j < num_all_tpls; ++j) {
				GetIntTuple(j, tpl, xm->doms);
				//支持
				if (m < r->size) {
					if (!IsEqual(r->tuples[m], tpl)) {
						Equal(tmp_tuples[k], tpl);
						++k;
					}
					else
						++m;
				}
				else {
					Equal(tmp_tuples[k], tpl);
					++k;
				}
			}

			//hm->tabs[i] = new HostTab(i, c->arity, r->size, r->tuples, c->scope);
			//生成HostTab
			makeHostTab(hm->tabs[i], c, r, num_sup_tpls);
		}

		//若论域为非标准
		if (IsAllStd == false) {

		}
	};

};

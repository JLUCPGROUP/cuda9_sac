#include "HostModel.h"

namespace cudacp {

HostVar::HostVar(const int id, const int size)
	:id(id), size(size) {
	vals = new int[size];
	for (size_t i = 0; i < size; i++) {
		vals[i] = i;
	}
}

HostVar::~HostVar() {
	delete[] vals;
	vals = NULL;
}

//HostTab::HostTab(const int id, const int arity, const int size)
//	:id(id), arity(arity), size(size) {
//	scope = new int[arity];
//	tuples = new int*[size];
//	for (size_t i = 0; i < size; i++) {
//		tuples[i] = new int[arity];
//	}
//}


HostTab::HostTab(const int id_, const int arity_, const int size_, int** tps_, int* scp_)
	: id(id_), arity(arity_), size(size_) {
	scope = new int[arity_];
	for (size_t i = 0; i < arity; i++)
		scope[i] = scp_[i];

	//int **p = new int*[100];
	//for (int i = 0; i <= 10; ++i) {
	//	p[i] = new int[10];
	//}

	//for (size_t i = 0; i < 10; i++) {
	//	delete[] p[i];
	//	p[i] = NULL;
	//}
	//delete[] p;
	//p = NULL;
	makeTuples(tps_);
}

HostTab::~HostTab() {
	delete[] scope;
	scope = NULL;

	for (int i = 0; i < size; ++i) {
		delete[] tuples[i];
		tuples[i] = NULL;
	}

	delete[] tuples;
	tuples = NULL;
}

void HostDomMap::MakeMap(XDom * d) {
	id = d->id;
	for (int i = 0; i < d->size; ++i) {
		m[d->values[i]] = i;
		if (d->values[i] != i)
			type = DT_DP;
	}
}



HostModelBuilder::HostModelBuilder(XModel * xmodel, HostModel * hmodel)
	:xm(xmodel), hm(hmodel) {
	domms.resize(xm->feature.ds_size);

	hm->property.vs_size = xm->feature.vs_size;
	hm->property.ts_size = xm->feature.cs_size;
	hm->vars = new HostVar*[hm->property.vs_size];
	hm->tabs = new HostTab*[hm->property.ts_size];

	generateHostDomMap();
	generateHostVars();
	generateHostTab();
}

void HostModelBuilder::getIntTuple(int idx, int *t, const XCon* c) {
	for (int i = (c->arity - 1); i >= 0; --i) {
		XDom* d = xm->doms[xm->vars[c->scope[i]]->dom_id];
		t[i] = d->values[(idx % (d->size))];
		idx /= (d->size);
	}
}

void HostModelBuilder::ShowHostModel() {
	//显示变量
	printf("Show variables:\n");
	for (size_t i = 0; i < hm->property.vs_size; i++) {
		const HostVar* v = hm->vars[i];
		printf("variable id = %d: ", i);
		for (size_t j = 0; j < v->size; j++) {
			printf("%d ", v->vals[j]);
		}
		printf("\n");
	}

	printf("Show constraints:\n");
	for (size_t i = 0; i < hm->property.ts_size; i++) {
		const HostTab* t = hm->tabs[i];
		printf("table_id = %d, arity = %d, size = %d, arity = %d, scope = { ", t->id, t->arity, t->size);
		for (size_t j = 0; j < t->arity; j++) {
			printf("%d ", t->scope[j]);
		}
		printf("}: ");

		for (size_t i = 0; i < t->size; i++) {
			for (size_t j = 0; j < t->arity; j++) {
				printf("%d ", t->tuples[i][j]);
			}
			printf("|");
		}
		printf("\n");
	}

}

void HostModelBuilder::assign(int* lh, const int* rh, const int len) {
	for (size_t i = 0; i < len; i++) {
		lh[i] = rh[i];
	}
}

bool HostModelBuilder::isEqual(const int* lh, const int* rh, const int len) {
	for (int i = 0; i < len; ++i)
		if (lh[i] != rh[i])
			return false;

	return true;
}

//void HostModelBuilder::makeHostTab(HostTab * t, const XCon * c, const XRel * r, const int sup_size) {
//
//
//	for (size_t i = 0; i < t->arity; i++)
//		t->scope[i] = c->scope[i];
//
//	for (size_t i = 0; i < t->size; i++)
//		for (size_t j = 0; j < t->arity; j++)
//			t->tuples[i][j] = tmp_tuples[i][j];
//}

void HostModelBuilder::generateHostDomMap() {
	for (size_t i = 0; i < xm->feature.ds_size; ++i)
		domms[i].MakeMap(xm->doms[i]);
}

void HostModelBuilder::generateHostVars() {
	for (size_t i = 0; i < xm->feature.vs_size; ++i)
		hm->vars[i] = new HostVar(i, xm->doms[xm->vars[i]->dom_id]->size);
}

void HostModelBuilder::generateHostTab() {
	//若为约束EXT
	for (int i = 0; i < xm->feature.cs_size; ++i) {
		const XCon* c = xm->cons[i];

		if (c->type == EXT)
			EXTConToTab(c);
	}
}

void HostModelBuilder::EXTConToTab(const XCon * c) {
	const XRel* r = xm->rels[c->rel_id];
	const int i = c->id;
	//std::map<int, DomType> mtype;
	std::vector<DomType> type(c->arity);
	bool IsAllStd = true;

	//中间变量元组
	for (size_t j = 0; j < c->arity; ++j) {
		const XVar* v = xm->vars[c->scope[j]];
		//mtype[j] = domms[v->dom_id].type;
		type[j] = domms[v->dom_id].type;
		IsAllStd &= domms[v->dom_id].type;
	}
	//正表
	if (r->sem == SEM_SUPPORT) {
		hm->tabs[i] = new HostTab(i, c->arity, r->size, r->tuples, c->scope);
	}
	//负表
	else {
		int** ts;
		int* tpl = new int[c->arity];
		//获取总个数
		int num_all_tpls = 1;
		for (size_t j = 0; j < c->arity; j++) {
			const XDom *d = xm->doms[xm->vars[c->scope[j]]->dom_id];
			num_all_tpls *= d->size;
		}
		//正表元组个数 = 总元组-负元组
		int num_sup_tpls = num_all_tpls - r->size;

		//生成临时元组集合
		ts = new int*[num_sup_tpls];
		for (int j = 0; j < num_all_tpls; ++j)
			ts[j] = new int[c->arity];

		//通过index获取tuple，一维索引转多维元组
		int k = 0, m = 0;
		for (int j = 0; j < num_all_tpls; ++j) {
			getIntTuple(j, tpl, c);
			//支持
			if (m < r->size) {
				if (!isEqual(r->tuples[m], tpl, c->arity)) {
					assign(ts[k], tpl, c->arity);
					++k;
				}
				else
					++m;
			}
			else {
				assign(ts[k], tpl, c->arity);
				++k;
			}
		}

		hm->tabs[i] = new HostTab(i, c->arity, num_sup_tpls, ts, c->scope);

		for (int j = 0; j < num_sup_tpls; ++j) {
			delete[] ts[j];
			ts[j] = NULL;
		}

		delete[] ts;
		ts = NULL;

		delete[] tpl;
		tpl = NULL;
		//hm->tabs[i] = new HostTab(c->id, c->arity, num_sup_tpls);
		////生成HostTab
		//makeHostTab(hm->tabs[i], c, r, num_sup_tpls);
		//std::for_each(tmp_tuples.begin(), tmp_tuples.end(), [](auto tp) {tp.clear(); });
		//tmp_tuples.clear();
	}

	//若论域为非标准
	if (IsAllStd == false) {
		modifyTuple(hm->tabs[i], type);
	}


}

void HostModelBuilder::modifyTuple(HostTab * t, std::vector<DomType>& type) {
	for (size_t i = 0; i < t->size; ++i) {
		for (size_t j = 0; j < t->arity; ++j) {
			const DomType tp = type[j];
			//若为非标准论域则标准化
			if (tp == DT_DP) {
				const XVar* v = xm->vars[t->scope[j]];
				const int s = t->tuples[i][j];
				const int res = domms[v->dom_id].m[s];
				t->tuples[j][i] = res;
			}
		}
	}
}

};
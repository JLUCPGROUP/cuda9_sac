#pragma once
#include <map>
#include <functional>
#include <vector>
#include <tuple>
#include <algorithm>
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

	HostVar(const int id, const int size);

	~HostVar();
};

class HostTab {
public:
	int id;
	int arity;
	int size;
	int** tuples;
	int* scope;
	//HostTab(const int id, const int arity, const int size);
	HostTab(const int id, const int arity, const int size, int** tps, int* scp);
	~HostTab();
private:
	void makeTuples(int** tps){
		tuples = (int **)malloc(sizeof(int*)*10);
		//tuples = new int*[size];
		for (size_t i = 0; i < size; i++) {
			tuples[i] = (int*)malloc(sizeof(int)*arity);
			//tuples[i] = new int[arity];
			for (size_t j = 0; j < arity; j++)
				tuples[i][j] = tps[i][j];
		}
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
	void MakeMap(XDom* d);
	~HostDomMap() {};
};


class HostModelBuilder {
public:
	std::vector<HostDomMap> domms;
	XModel* xm;
	HostModel* hm;
	//std::vector<std::vector<int>> tmp_tuples;

	HostModelBuilder(XModel* xmodel, HostModel* hmodel);
	~HostModelBuilder() {};

	/**
	*\brief 显示HostModel
	*/
	void ShowHostModel();
private:
	void assign(int* lh, const int* rh, const int len);

	void getIntTuple(int idx, int *t, const XCon* c);

	/**
	*\brief 判断元组是否相等
	*/
	bool isEqual(const int* lh, const int* rh, const int len);

	/**
	*\brief 生成HostTab
	*/
	//void makeHostTab(HostTab* t, const XCon *c, const XRel* r, const int sup_size);

	/**
	*\brief 生成HostDomMap
	*/
	void generateHostDomMap();

	/**
	*\brief 生成HostVars
	*/
	void generateHostVars();

	/**
	*\brief 生成HostTab
	*/
	void generateHostTab();

	/**
	*\brief 表约束转HostTab
	*/
	void EXTConToTab(const XCon *c);

	/**
	*\brief 转标准表约束
	*/
	void modifyTuple(HostTab* t, std::vector<DomType>& type);

};
};

#pragma once
#include <string>
namespace cudacp {

typedef unsigned int u32;
/**
* \brief SCP问题取值范围
* INT_MIN < 表达式 < INT_MIN + 4096 < 合法值 < INT_MAX - 4096 < 变量id取值范围 < INT_MAX
*/
const int MAX_VALUE = INT_MAX - 4096;	///<最大值
const int MAX_OPT = INT_MIN + 4096;		///<最小值

/**
* \brief operator for predicate
*/
enum PredicateOperator {
	PO_NON = INT_MIN, 		///<空		-2147483648
	PO_L_PAR = INT_MIN + 1,	///<左括号	-2147483647
	PO_R_PAR = INT_MIN + 2,	///<右括号	-2147483646
	PO_COMMA = INT_MIN + 3,	///<逗号		-2147483645
	PO_ADD = INT_MIN + 10,	///<加法		-2147483638
	PO_SUB = INT_MIN + 11,	///<减法		-2147483637
	PO_MUL = INT_MIN + 12,	///<乘法		-2147483636
	PO_DIV = INT_MIN + 13,	///<除法		-2147483635
	PO_MOD = INT_MIN + 14,	///<取余		-2147483634
	PO_EQ = INT_MIN + 30,	///<"="		-2147483618
	PO_NE = INT_MIN + 31,	///<"!="	-2147483617
	PO_LT = INT_MIN + 32,	///<"<"		-2147483616
	PO_LE = INT_MIN + 33,	///<"<="	-2147483615
	PO_GT = INT_MIN + 34,	///<">"		-2147483614
	PO_GE = INT_MIN + 35,	///<">="	-2147483613
	PO_ABS = INT_MIN + 60,	///<取模		-2147483588
	PO_AND = INT_MIN + 101,	///<逻辑与	-2147483547
	PO_OR = INT_MIN + 102	///<逻辑或	-2147483546
};
/**
* \brief Max number
*/
#define MAX(x,y) (x)>(y)?(x):(y)

enum RelType {
	EXT, INT
};

enum Semantices {
	//冲突
	SEM_CONFLICT = 0,
	//支持
	SEM_SUPPORT = 1
};

struct PStack {
	int ps[200];
	int num_prs = 0;
	int top = -1;
	int bottom = 0;
	void push(const int e);
	int pop();
};

class XDom {
public:
	int id;
	int size;
	int* values;
	XDom() {};
	XDom(const int id, const int size, char* values_str);
	virtual ~XDom();
private:
	void GenerateValues(char* values_str);
};

class XVar {
public:
	int id;
	int dom_id;
	XVar() {};
	XVar(const int id, const int dom_id) :id(id), dom_id(dom_id) {}
	virtual ~XVar() {}
};

class XRel {
public:
	int id;
	int arity;
	int size;
	int **tuples;
	Semantices sem;
	XRel() {};
	XRel(const int id, const int arity, const int size, const Semantices sem, char* tuples_str);
	virtual ~XRel();
private:
	void GenerateTuples(char *tuples_str);
};

class XPre {
public:
	int id;
	int *paras;
	int num_paras;
	PStack prop_stack;
	XPre() {};
	XPre(const int id, char* pas_chr, char* prs_chr);
	void GeneratePStack(char* prs_chr);
	int GetOperator(std::string s);
	void GeneratePostfixExpression();
	~XPre();

};

class XCon {
public:
	int id;
	int rel_id;
	int arity;
	int* scope;
	RelType type;
	XCon() {};
	XCon(const int id, const int rel_id, const int arity, const RelType type, char* scope_str);
	~XCon();
};

class XINTCon :public XCon {
public:
	int num_paras;
	int* paras;
	XINTCon() {};
	XINTCon(const int id, const int rel_id, const int arity, const RelType type, char* scope_str, char* pars);
	~XINTCon();
	int GetParameters(const char* par_chr);
	//void GenerateParameters(char* pars_chr, const int pars_len);
	//void GenerateParameters(char* pars_chr);
};

struct XFeature {
	int ds_size;
	int vs_size;
	int rs_size;
	int ps_size;
	int cs_size;
	int max_arity;
	int max_dom_size;

	XFeature& operator=(const XFeature& xf) {
		this->ds_size = xf.ds_size;
		this->vs_size = xf.vs_size;
		this->rs_size = xf.rs_size;
		this->ps_size = xf.ps_size;
		this->cs_size = xf.cs_size;
		this->max_arity = xf.max_arity;
		this->max_dom_size = xf.max_dom_size;
		return *this;
	}
};

class XModel {
public:
	XFeature feature;
	XDom** doms;
	XVar** vars;
	XRel** rels;
	XPre** pres;
	XCon** cons;
	XModel() {};
	virtual ~XModel();
};
}



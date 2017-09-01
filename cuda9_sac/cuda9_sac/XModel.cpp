#include "XModel.h"
#include <string>

namespace cudacp {

XDom::XDom(int id_, int size_, char* val_str_) :
	id(id_), size(size_) {
	values = new int[size];
	GenerateValues(val_str_);
}

XDom::~XDom() {
	for (int i = 0; i < size; ++i) {
		delete[] values;
		values = NULL;
	}
}

void XDom::GenerateValues(char* values_str) {
	std::string s = values_str;
	s += " ";
	std::string tmp = values_str;
	int start = 0;
	int end = 0;
	int pre_blankpos = 0;
	std::string::size_type blankpos = s.find(" ", 0);
	std::string::size_type pointpos = 0;
	int g_count = 0;
	int value_int;

	if (blankpos == std::string::npos) {
		sscanf_s(values_str, "%d..%d", &start, &end);
		for (int i = start; i <= end; ++i) {
			values[g_count] = i;
			++g_count;
		}
	}
	else {
		while (blankpos != std::string::npos) {
			tmp = s.substr(0, blankpos - pre_blankpos);
			s = s.substr(blankpos + 1);
			pointpos = tmp.find(".", 0);
			blankpos = s.find(" ", 0);

			if (pointpos != std::string::npos) {
				sscanf_s(tmp.c_str(), "%d..%d", &start, &end);

				for (int i = start; i <= end; ++i) {
					values[g_count] = i;
					++g_count;
				}
			}
			else {
				sscanf_s(tmp.c_str(), "%d", &value_int);
				values[g_count] = value_int;
				++g_count;
			}
		}
	}
}

XRel::XRel(const int id_, const int arity_, const int size_, const Semantices sem_, char* tuples_str_)
	:id(id_), arity(arity_), size(size_), sem(sem_) {
	tuples = new int *[size];

	for (int i = 0; i < size; ++i)
		tuples[i] = new int[arity];

	GenerateTuples(tuples_str_);
}

XRel::~XRel() {
	for (int i = 0; i < size; ++i) {
		delete[] tuples[i];
		tuples[i] = NULL;
	}

	delete[] tuples;
	tuples = NULL;
}

void XRel::GenerateTuples(char *tuples_str_) {
	for (int i = 0; i < size; ++i)
		for (int j = 0; j < arity; ++j)
			tuples[i][j] = atoi(strtok_s(tuples_str_, " |", &tuples_str_));
}

XCon::XCon(const int id_, const int rel_id_, const int arity_, const RelType type_, char * scope_str_)
	:id(id_), rel_id(rel_id_), type(type_), arity(arity_) {
	scope = new int[arity];

	for (int i = 0; i < arity; ++i)
		scope[i] = atoi(strtok_s(scope_str_, " V", &scope_str_));
}

XCon::~XCon() {
	delete[] scope;
	scope = NULL;
}

XINTCon::XINTCon(const int id, const int rel_id, const int arity, const RelType type, char* scope_str, char* pars)
	:XCon(id, rel_id, arity, type, scope_str) {

	int pas[50];
	std::string s = pars;
	s += " ";
	int startpos = 0;
	int j = -1;
	std::string tmp;
	int op;
	int par;

	for (int i = 0; i < s.length(); ++i) {
		switch (s[i]) {
		case ' ':
			tmp = s.substr(startpos, i - startpos);
			par = GetParameters(tmp.c_str());
			pas[++j] = par;
			startpos = i + 1;
			break;
		default:
			break;
		}
	}
	num_paras = j + 1;
	paras = new int[num_paras];

	for (size_t i = 0; i <= num_paras; i++)
		paras[i] = pas[i];

}
int XINTCon::GetParameters(const char* par_chr) {
	int value;
	std::string s = par_chr;

	if (s[0] == 'V') {
		sscanf(s.c_str(), "V%d", &value);
		value += MAX_VALUE;
	}
	else {
		value = atoi(par_chr);
	}
	return value;
}
XINTCon::~XINTCon() {
	delete[] paras;
	paras = NULL;
}

XModel::~XModel() {
	for (int i = 0; i < feature.ds_size; ++i) {
		delete  doms[i];
		doms[i] = NULL;
	}
	delete[]  doms;
	doms = NULL;


	for (int i = 0; i < feature.vs_size; ++i) {
		delete  vars[i];
		vars[i] = NULL;
	}
	delete[]  vars;
	vars = NULL;

	if (feature.rs_size > 0) {
		for (int i = 0; i < feature.rs_size; ++i) {
			delete  rels[i];
			rels[i] = NULL;
		}
		delete[]  rels;
		rels = NULL;
	}

	if (feature.ps_size > 0) {
		for (size_t i = 0; i < feature.ps_size; i++) {
			delete pres[i];
			pres[i] = NULL;
		}
		delete[] pres;
		pres = NULL;
	}

	for (int i = 0; i < feature.cs_size; ++i) {
		delete  cons[i];
		cons[i] = NULL;
	}
	delete[]  cons;
	cons = NULL;
}

void PStack::push(const int e) {
	if (top <= 200) {
		++num_prs;
		++top;
		ps[top] = e;
	}
}

int PStack::pop() {
	int res = 0;
	if (num_prs >= 0) {
		--num_prs;
		res = ps[top];
		--top;
	}
	return res;
}

XPre::XPre(const int id, char * pas_chr, char * prs_chr) :id(id) {
	int max_par_index;
	std::string par_str;
	par_str = pas_chr;
	par_str = par_str.substr(par_str.rfind("X") + 1);
	max_par_index = atoi(par_str.c_str());
	++max_par_index;
	num_paras = max_par_index;
	paras = new int[max_par_index];
	for (int i = 0; i < max_par_index; ++i) {
		paras[i] = i;
	}
	GeneratePStack(prs_chr);
	GeneratePostfixExpression();
}
void XPre::GeneratePStack(char * prs_chr) {
	int startpos = 0;
	std::string s = prs_chr;
	std::string tmp;
	int op;
	int i = 0;
	int j = -1;
	for (i = 0; i < s.length(); ++i) {
		switch (s[i]) {
		case '(':
			tmp = s.substr(startpos, i - startpos);
			op = GetOperator(tmp);
			if (op != PO_NON) {
				prop_stack.ps[++j] = op;
			}
			prop_stack.ps[++j] = PO_L_PAR;
			startpos = i + 1;
			break;
		case ')':
			tmp = s.substr(startpos, i - startpos);
			op = GetOperator(tmp);
			if (op != PO_NON) {
				prop_stack.ps[++j] = op;
			}
			prop_stack.ps[++j] = PO_R_PAR;
			startpos = i + 1;
			break;
		case ',':
			tmp = s.substr(startpos, i - startpos);
			op = GetOperator(tmp);
			if (op != PO_NON) {
				prop_stack.ps[++j] = op;
			}
			startpos = i + 1;
			break;
		case ' ':
			startpos = i + 1;
			break;
		default:
			break;
		}
	}
	prop_stack.num_prs = j + 1;
	prop_stack.top = j;
	prop_stack.bottom = 0;
}

int XPre::GetOperator(std::string s) {
	int value;

	if (s == "(") {
		return PO_L_PAR;
	}
	else if (s == ")") {
		return PO_R_PAR;
	}
	else if (s == "div") {
		return PO_DIV;
	}
	else if (s == "mod") {
		return PO_MOD;
	}
	else if (s == "sub") {
		return PO_SUB;
	}
	else if (s == "mul") {
		return PO_MUL;
	}
	else if (s == "abs") {
		return PO_ABS;
	}
	else if (s == "eq") {
		return PO_EQ;
	}
	else if (s == "ne") {
		return PO_NE;
	}
	else if (s == "lt") {
		return PO_LT;
	}
	else if (s == "le") {
		return PO_LE;
	}
	else if (s == "gt") {
		return PO_GT;
	}
	else if (s == "ge") {
		return PO_GE;
	}
	else if (s == "and") {
		return PO_AND;
	}
	else if (s == "or") {
		return PO_OR;
	}
	else if (s[0] == 'X') {
		sscanf(s.c_str(), "X%d", &value);
		return value;
	}

	return PO_NON;
}
void XPre::GeneratePostfixExpression() {
	PStack res_stk;
	u32 ps_size = prop_stack.num_prs;
	int op;
	u32 i = 0, j = 0;
	int last_lpar_idx = 0;
	u32 idx = 0;

	while (i < prop_stack.num_prs) {
		op = prop_stack.ps[i];

		if (op == PO_L_PAR) {
			last_lpar_idx = i;
		}
		else if (op == PO_R_PAR) {
			for (j = last_lpar_idx; j < i; ++j) {
				if (prop_stack.ps[j] > MAX_OPT) {
					res_stk.push(prop_stack.ps[j]);
					prop_stack.ps[j] = PO_NON;
				}
			}

			prop_stack.ps[last_lpar_idx] = PO_NON;
			idx = last_lpar_idx - 1;
			op = prop_stack.ps[idx];
			res_stk.push(op);

			while (op != PO_L_PAR && last_lpar_idx > 0) {
				--last_lpar_idx;
				op = prop_stack.ps[last_lpar_idx];
			}
		}
		++i;
	}

	prop_stack.num_prs = res_stk.num_prs;
	prop_stack.top = res_stk.top;
	prop_stack.bottom = res_stk.bottom;

	for (i = 0; i < prop_stack.num_prs; ++i)
		prop_stack.ps[i] = res_stk.ps[i];
}

cudacp::XPre::~XPre() {
	delete[] paras;
	paras = NULL;
}
}
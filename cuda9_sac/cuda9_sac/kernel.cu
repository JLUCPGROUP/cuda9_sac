#pragma once

#include <iostream>
#include <string>
#include "cudasac.cuh"
#undef DOMDocument

#include "XBuilder.h"

using namespace std;
using namespace cudacp;
const string X_PATH = "BMPath.xml";

int main() {
	XBuilder path_builder(X_PATH, XRT_BM_PATH);
	string bm_path = path_builder.GetBMFile();
	cout << bm_path << endl;
	XBuilder builder(bm_path, XRT_BM);
	XModel* xmodel = new XModel();
	builder.GenerateModelFromXml(xmodel);
	//float build_time = BuidBitModel32bit(xmodel);
	//float exe_time = SACGPU();
	//DelGPUModel();

	////for (size_t i = 0; i < xmodel->feature.cs_size; i++) {
	////	XCon *c = xmodel->cons[i];
	////	XINTCon* ic = static_cast<XINTCon *>(c);
	////	printf("%d:", ic->id);
	////	for (size_t j = 0; j < ic->num_paras; j++) {
	////		printf("%d,", ic->paras[j]);
	////	}
	////	printf("\n");
	////}

	//for (size_t i = 0; i < xmodel->feature.ps_size; i++) {
	//	XPre* p = xmodel->pres[i];
	//	printf("%d, num_p:%d\n", p->id, p->num_paras);
	//	//for (size_t j = 0; j < p->num_paras; j++) {
	//	//	printf("%d,", p->paras[j]);
	//	//}
	//	for (size_t j = 0; j < p->prop_stack.num_prs; j++) {
	//		printf("%d,", p->prop_stack.ps[j]);
	//	}
	//	printf("\n");
	//}

	delete xmodel;
	xmodel = NULL;
	//printf("Build time = %f\n", build_time);
	//printf("Execution time = %f\n", exe_time);
	printf("---end---\n");
	return 0;
}


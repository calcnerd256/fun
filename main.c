#include "types.h"
#include "tc.h"
#include "iota.tc.h"
#include "io.h"
#include "eval.iota.tc.h"


int iotaTest(struct byteArray *arfs){
	//I really should be doing refcounting first...
	struct byteArray *expr = iotaGen(iotaGen(iotaGen(S())));
		//tcCons(tcCons(tcCons(S(), tcPtr((void*)3)), tcPtr((void*)1)), tcPtr((void*)2));
		//iotaGen(tcCons(tcPtr((void*)1), tcPtr((void*)2)));
		/*tcCons(
		tcCons(
			tcCons(S(), I()),
			I()
		),
		tcCons(
			tcCons(S(), I()),
			I()
		)
	);*/
	tcPrintDump(expr);
	printf("\n");
	struct byteArray *stepMem = tcIotaEvalStepLeak(expr);
	tcPrintDump(stepMem);
	printf("\n");
	tcPrintDump(tcCar(stepMem));
	printf("\n");
	freeLeakStack(tcCdr(stepMem));
	tcFreeTree(tcCdr(stepMem));
	freeTcCons(stepMem);
        tcFreeTree(expr);
	return 0;
}

int barrMain(struct byteArray *arfs){

	tcPrintDump(arfs);
	printf("\n");

	while(tcConsp(arfs)){
		printf("%s", (char*)tcValue(tcCar(arfs)));
		arfs = tcCdr(arfs);
		if(tcValue(arfs))
			printf(" ");
	}
	printf("\n");

	return iotaTest(arfs);
}

void pointAtSelf(void* *ptr){
	*ptr = ptr;
}
int init(){
	pointAtSelf(&leaf);
	pointAtSelf(&pair);
	pointAtSelf(&error);
	pointAtSelf(&iota);
	pointAtSelf(&cstr);
	return 0;
}
int main(int arfc, char* *arfv){
	int result;
	result = init();
	if(result) return result;
	struct byteArray *arfs = argsToTc(arfc, arfv);
	result = barrMain(arfs);
	tcFreeTree(arfs);
	return result;
}

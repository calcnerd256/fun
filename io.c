#include "types.h"
#include "tc.h"
#include "iota.tc.h"
#include "io.h"

struct byteArray *strToRbarr(char *str){
	return referenceBarr(strlen(str), str);
}
void printBarr(struct byteArray *barr){
	int i;
	for(i = 0; i < barr->size; i++)
		printf("%c", barr->arr[i]);
}
struct byteArray *argsToBarr(int arfc, char* *arfv){
	int i;
	struct byteArray* *rbarrs;
	struct byteArray *result = allocBarr(arfc * sizeof(struct byteArray*));
	rbarrs = (struct byteArray**)(result->arr);
	for(i = 0; i < arfc; i++)
		rbarrs[i] = strToRbarr(arfv[i]);
	return result;
}
void freeArgsBarr(struct byteArray *args){
	int i;
	for(i = 0; i < args->size / sizeof(struct byteArray*); i++)
		freeRbarr(((struct byteArray**)(args->arr))[i]);
	freeBarr(args);
}


void tcPrintAtom(struct byteArray* tc){
	char* ptr;
	if(!tc || !tcValue(tc)){
		printf("nil");
		return;
	}
	if(tcConsp(tc)){
		printf("somecons");
		return;
	}
	if(!tcAtomp(tc)){
		printf("<??\?>");
		return;
	}
	if(cstr == tcType(tc)){
		printf("\"");
		ptr = (char*)tcValue(tc);
		while(*ptr){
			if('\\' == *ptr || '\"' == *ptr)
				printf("\\");
			printf("%c", *ptr++);
		}
		printf("\"");
		return;
	}
	if(iota == tcValue(tc)){
		printf("iota");
		return;
	}
	printf("<%p>", tcValue(tc));
}
int tcPrintIotaSpecial(struct byteArray* tc){
	//assume tcIotaSpecialp
	if(iota == tcValue(tc))
		return printf("iota");
	tc = tcCdr(tc);
	if(iota == tcValue(tc))
		return printf("I");
	tc = tcCdr(tc);
	if(iota == tcValue(tc))
		return printf("0");
	tc = tcCdr(tc);
	if(iota == tcValue(tc))
		return printf("K");
	return printf("S");
}
void tcPrintDump(struct byteArray* tc){
	struct byteArray* ptr;
	struct byteArray* stack = 0;
	if(!tc){
		printf("nil");
		return;
	}
	if(!tcConsp(tc)){
		tcPrintAtom(tc);
		return;
	}
	stack = simpleCons(&ptr, stack);// print ")" later
	stack = simpleCons(tc, stack);
	printf("(");
	while(stack){
		//pop tc
		tc = bcar(stack);
		ptr = bcdr(stack);
		freeBarr(stack);
		stack = ptr;

		if((void*)&ptr == tc)
			printf(")");
		else if((void*)&stack == tc)
			printf(" . ");
		else if((void*)&tc == tc)
			printf(" ");
		else if(tcIotaSpecialp(tc))
			tcPrintIotaSpecial(tc);
		else if(tcConsp(tc)){
			//print cdr later
			if(tcValue(tcCdr(tc))){
				stack = simpleCons(tcCdr(tc), stack);
				stack = simpleCons(
					tcAtomp(tcCdr(tc)) ?
						&stack : // " . "
						tcIotaSpecialp(tcCdr(tc)) ?
							&stack : // " . "
							&tc, // " "
					stack
				);
			}

			if(!tcIotaSpecialp(tcCar(tc))){
				//print car later
				if(tcConsp(tcCar(tc))){
					stack = simpleCons(&ptr, stack);// print ")" later
					printf("(");
				}
				stack = simpleCons(tcCar(tc), stack);
			}
			else
				tcPrintIotaSpecial(tcCar(tc));
		}
		else
			tcPrintAtom(tc);
	}
}

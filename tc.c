#include "types.h"
#include "tc.h"


struct byteArray *tcCons(void *head, void *tail){
	return simpleCons(pair, simpleCons(head, tail));
}
struct byteArray *tcPtr(void *value){
	return simpleCons(leaf, value);
}

void* tcType(struct byteArray *tc){
	return (void*)bcar(tc);
}
int tcConsp(struct byteArray *tc){
	return pair == tcType(tc);
}
int tcAtomp(struct byteArray *tc){
	if(cstr == tcType(tc)) return 1;
	return leaf == tcType(tc);
}

void* tcValue(struct byteArray *tc){
	if(!tc) return 0;
	if(error == tc) return error;
	return bcdr(tc);
}

struct byteArray* tcCar(struct byteArray *tc){
	struct byteArray* tcVal = (struct byteArray*)tcValue(tc);
	if(!tcVal) return 0;
	if(error == tcVal) return error;
	if(!tcConsp(tc)) return error;
	return bcar(tcVal);
}
struct byteArray* tcCdr(struct byteArray *tc){
	struct byteArray* tcVal = (struct byteArray*)tcValue(tc);
	if(!tcVal) return 0;
	if(error == tcVal) return error;
	if(!tcConsp(tc)) return error;
	return bcdr(tcVal);
}

void tcFreeTree(struct byteArray *tc){
	struct byteArray* stack = 0;
	struct byteArray* ptr;
	stack = simpleCons(tc, stack);
	while(stack){
		tc = bcar(stack);
		ptr = bcdr(stack);
		freeBarr(stack);
		stack = ptr;
		if(tcConsp(tc)){
			stack = simpleCons(tcCdr(tc), stack);
			stack = simpleCons(tcCar(tc), stack);
		}
		freeBarr(tc);
	}
}

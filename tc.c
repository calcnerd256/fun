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

struct byteArray* tcSetCar(struct byteArray *tc, struct byteArray* value){
	struct byteArray* tcVal = (struct byteArray*)tcValue(tc);
	if(!tcVal) return error;
	if(error == tcVal) return error;
	if(!tcConsp(tc)) return error;
	return (struct byteArray*)barrSetCar(tcVal, value);
}
struct byteArray* tcSetCdr(struct byteArray *tc, struct byteArray* value){
	struct byteArray* tcVal = (struct byteArray*)tcValue(tc);
	if(!tcVal) return error;
	if(error == tcVal) return error;
	if(!tcConsp(tc)) return error;
	return (struct byteArray*)barrSetCdr(tcVal, value);
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

void freeTcCons(struct byteArray* tc){
	//only frees the cons cell, not its contents
	//a tcCons is a simple cons whose car is not to be freed and whose cdr is a simple cons
	freeBarr(bcdr(tc));
	freeBarr(tc);
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

struct byteArray *pcblToTc(struct byteArray *pcbl){
	struct byteArray *result;
	struct byteArray *ptr;
	struct byteArray *backwards = reversePcbl(pcbl);
	ptr = backwards;
	result = tcPtr(0);
	while(ptr){
		result = tcCons(tcPtr(bcar(ptr)), result);
		ptr = bcdr(ptr);
	}
	freePcbl(backwards);
	return result;
}

void tcStackPush(struct byteArray* *stack, void *val){
	*stack = tcCons(val, *stack);
}
struct byteArray *tcStackSlinky(struct byteArray *src, struct byteArray *dest){
	struct byteArray *ptr = src;
	while(src && tcConsp(src)){
		ptr = src;
		tcStackPush(&dest, tcCar(src));
		src = tcCdr(src);
		freeTcCons(ptr);
	}
	if(src)
		freeBarr(src);
	return dest;
}


int cdrwiseHeight(struct byteArray *tree){
	//return number of cdrs from root to cdrmost leaf
	int result = 0;
	while(tree && tcConsp(tree)){
		tree = tcCdr(tree);
		++result;
	}
	return result;
}

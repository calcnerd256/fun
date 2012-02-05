#include "tc.h"

struct byteArray *simpleCons(void *head, void *tail){
	struct byteArray *result = allocBarr(sizeof(struct consCell));
	struct consCell *cell = (struct consCell*)(result->arr);
	cell->head = head;
	cell->tail = tail;
	return result;
}
struct byteArray* bcar(struct byteArray *cell){
	return (struct byteArray*)car(cell->arr);
}
struct byteArray* bcdr(struct byteArray *cell){
	return (struct byteArray*)cdr(cell->arr);
}


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

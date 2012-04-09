#include "types.h"
#include "barr.cons.h"

struct byteArray *simpleCons(void *head, void *tail){
	struct byteArray *result = allocBarr(sizeof(struct consCell));
	barrSetCar(result, head);
	barrSetCdr(result, tail);
	return result;
}
void *barrSetCar(struct byteArray *cell, void* value){
	return setCar((struct consCell*)(cell->arr), value);
}
void *barrSetCdr(struct byteArray *cell, void* value){
	return setCdr((struct consCell*)(cell->arr), value);
}
struct byteArray* bcar(struct byteArray *cell){
	return (struct byteArray*)car(cell->arr);
}
struct byteArray* bcdr(struct byteArray *cell){
	return (struct byteArray*)cdr(cell->arr);
}

//pcbl stands for pointer cons barr list: a list made of cons-barrs containing pointers
struct byteArray *ptrArrToPcbl(int len, void* *xs){
	struct byteArray *result = 0;
	while(len)
		result = simpleCons(xs[--len], (void*)result); 
	return result;
}
struct byteArray *ptrBarrToPcbl(struct byteArray *arr){
	return ptrArrToPcbl(arr->size / sizeof(void*), (void**)(arr->arr));
}
void freePcbl(struct byteArray *arr){
	struct byteArray *tail;
	while(arr){
		tail = bcdr(arr);
		freeBarr(arr);
		arr = tail;
	}
}

struct byteArray *reversePcbl(struct byteArray *ps){
	struct byteArray *result = 0;
	while(ps){
		result = simpleCons(bcar(ps), result);
		ps = bcdr(ps);
	}
	return result;
}

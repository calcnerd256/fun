#include "types.h"
#include "barr.cons.h"

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

#include "barr.h"

//from stdlib.h
extern void *malloc(size_t);

void *poolAlloc(size_t size){
	//TODO: use a pool of previously-allocated chunks
	//	Why? Isn't that malloc's job?
	return malloc(size);//for now
}

struct byteArray *referenceBarr(size_t size, void* ptr){
	struct byteArray *result = (struct byteArray*) poolAlloc(sizeof(struct byteArray));
	result->size = size;
	result->arr = ptr;
	return result;
}


struct byteArray *allocBarr(size_t size){
	return referenceBarr(size, poolAlloc(size));
}

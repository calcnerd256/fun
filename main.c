#include <stdio.h>

//stdlib.h
extern void free(void*);
extern void* malloc(size_t);

//string.h
extern size_t strlen(char*);

struct byteArray{
	size_t size;
	char* arr;
};

void poolFree(void *ptr){
	free(ptr);
}
void *poolAlloc(size_t size){
	//TODO: use a pool of previously-allocated chunks
	//	Why? Isn't that malloc's job?
	return malloc(size);//for now
}

void freeRbarr(struct byteArray *arr){
	poolFree(arr);
}
struct byteArray *referenceBarr(size_t size, void* ptr){
	struct byteArray *result = (struct byteArray*) poolAlloc(sizeof(struct byteArray));
	result->size = size;
	result->arr = ptr;
	return result;
}
void freeBarr(struct byteArray *arr){
	poolFree(arr->arr);//TODO: check error value
	freeRbarr(arr);
}
struct byteArray *allocBarr(size_t size){
	return referenceBarr(size, poolAlloc(size));
}

struct byteArray *argsToBarr(int arfc, char* *arfv){
	int i, len;
	struct byteArray *result = allocBarr(arfc * sizeof(struct byteArray*));
	for(i = 0; i < arfc; i++){
		len = strlen(arfv[i]);
		((struct byteArray**)(result->arr))[i] = referenceBarr(len, arfv[i]);
	}
	return result;
}
void freeArgsBarr(struct byteArray *args){
	int i;
	for(i = 0; i < args->size / sizeof(struct byteArray*); i++)
		freeRbarr(((struct byteArray**)(args->arr))[i]);
	freeBarr(args);
}

int main(int arfc, char* *arfv){
	int i;
	struct byteArray *mem = argsToBarr(arfc, arfv);//referenceBarr(4, "ok\n");
	for(i = 0; i < mem->size / sizeof(struct byteArray*); i++)
		printf("%s\n", ((struct byteArray**)(mem->arr))[i]->arr);
	freeRbarr(mem);
	return 0;
}

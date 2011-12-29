#include <stdlib.h>
#include <stdio.h>
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

void clearBarr(struct byteArray *arr){
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
	clearBarr(arr);
}
struct byteArray *allocBarr(size_t size){
	return referenceBarr(size, poolAlloc(size));
}


int main(int arfc, char* *arfv){
	char *str;
	struct byteArray *mem = allocBarr(4);
	str = mem->arr;
	str[0] = 'o';
	str[1] = 'k';
	str[2] = '\n';
	str[3] = 0;
	printf("%s", str);
	freeBarr(mem);
	return 0;
}

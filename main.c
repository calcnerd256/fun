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

void freeBarr(struct byteArray *arr){
	poolFree(arr->arr);//TODO: check error value
	poolFree(arr);
}
struct byteArray *allocBarr(size_t size){
	struct byteArray *result = (struct byteArray*) poolAlloc(sizeof(struct byteArray));
	result->arr = poolAlloc(size);
	result->size = size;
	return result;
}

int main(int arfc, char* *arfv){
	char *str;
	str = (char*) poolAlloc(4);
	str[0] = 'o';
	str[1] = 'k';
	str[2] = '\n';
	str[3] = 0;
	printf("%s", str);
	return 0;
}

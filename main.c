#include <stdio.h>
struct byteArray{
	size_t size;
	char* arr;
}

int poolFree(void* ptr){
	return free(ptr);
}
void* poolAlloc(size_t size){
	//TODO: use a pool of previously-allocated chunks
	//	Why? Isn't that malloc's job?
	return malloc(size);//for now
}

int freeBarr(struct byteArray *arr){
	poolFree(arr->arr);//TODO: check return value
	return poolFree(arr);
}
struct byteArray *allocBarr(size_t size){
	struct byteArray *result = (struct byteArray*) poolAlloc(sizeof(struct byteArray));
	result->arr = poolAlloc(size);
	result->size = size;
	return result;
}

int main(int arfc, char* *arfv){
	printf("%s", "ok\n");
	return 0;
}

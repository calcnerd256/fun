#include <stdio.h>
int poolFree(void* ptr){
	return free(ptr);
}
void* poolAlloc(size_t size){
	//TODO: use a pool of previously-allocated chunks
	//	Why? Isn't that malloc's job?
	return malloc(size);//for now
}
int main(int arfc, char* *arfv){
	printf("%s", "ok\n");
	return 0;
}

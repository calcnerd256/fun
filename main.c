#include <stdio.h>

//stdlib.h
extern void free(void*);
extern void *malloc(size_t);

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

struct byteArray *strToRbarr(char *str){
	return referenceBarr(strlen(str), str);
}
void printBarr(struct byteArray *barr){
	int i;
	for(i = 0; i < barr->size; i++)
		printf("%c", barr->arr[i]);
}
struct byteArray *argsToBarr(int arfc, char* *arfv){
	int i;
	struct byteArray* *rbarrs;
	struct byteArray *result = allocBarr(arfc * sizeof(struct byteArray*));
	rbarrs = (struct byteArray**)(result->arr);
	for(i = 0; i < arfc; i++)
		rbarrs[i] = strToRbarr(arfv[i]);
	return result;
}
void freeArgsBarr(struct byteArray *args){
	int i;
	for(i = 0; i < args->size / sizeof(struct byteArray*); i++)
		freeRbarr(((struct byteArray**)(args->arr))[i]);
	freeBarr(args);
}

struct consCell{
	void *head;
	void *tail;
};
struct byteArray *simpleCons(void *head, void *tail){
	struct byteArray *result = allocBarr(sizeof(struct consCell));
	struct consCell *cell = (struct consCell*)(result->arr);
	cell->head = head;
	cell->tail = tail;
	return result;
}
struct consCell *car(char *cell){
	return (struct consCell*)(
		((struct consCell*)cell)->head
	);
}
struct consCell *cdr(char *cell){
	return (struct consCell*)(
		((struct consCell*)cell)->tail
	);
}
struct byteArray *ptrBarrToConsBarrList(struct byteArray *arr){
	void* *ptr = (void**)(arr->arr);
	int len = arr->size / sizeof(void*);
	struct byteArray *result = 0;
	while(len)
		result = simpleCons(ptr[--len], (void*)result); 
	return result;
}
void freePtrConsBarrList(struct byteArray *arr){
	struct byteArray *tail;
	while(arr){
		tail = (struct byteArray*)cdr(arr->arr);
		freeBarr(arr);
		arr = tail;
	}
}

void *iota;
int barrMain(struct byteArray *args){
	int i, len;
	struct byteArray* *rbarrs;

	len = args->size / sizeof(struct byteArray*);
	rbarrs = (struct byteArray**)(args->arr);

	struct byteArray *cell = simpleCons(0, 0);
	struct byteArray *other = simpleCons(0, cell);
	freeBarr((struct byteArray*)cdr(other->arr));
	freeBarr(other);

	for(i = 0; i < len; i++){
		printBarr(rbarrs[i]);
		if(i+1 < len)
			printf(" ");
	}
	printf("\n");

	return 0;
}

int main(int arfc, char* *arfv){
	int result;
	struct byteArray *args = argsToBarr(arfc, arfv);
	result = barrMain(args);
	freeArgsBarr(args);
	return result;
}

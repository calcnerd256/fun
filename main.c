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
		tail = (struct byteArray*)cdr(arr->arr);
		freeBarr(arr);
		arr = tail;
	}
}

void *iota;
int barrMain(struct byteArray *args, struct byteArray *arhs){
	int i, len;
	struct byteArray* *rbarrs;

	while(arhs){
		printBarr((struct byteArray*)car(arhs->arr));
		arhs = (struct byteArray*)cdr(arhs->arr);
		if(arhs)
			printf(" ");
	}
	printf("\n");

	len = args->size / sizeof(struct byteArray*);
	rbarrs = (struct byteArray**)(args->arr);

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
	struct byteArray *arhs = ptrBarrToPcbl(args);
	struct byteArray *arfs = ptrArrToPcbl(arfc, (void**)arfv);
	result = barrMain(args, arhs);
	freePcbl(arhs);
	freeArgsBarr(args);
	freePcbl(arfs);
	return result;
}

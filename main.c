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
struct byteArray* bcar(struct byteArray *cell){
	return (struct byteArray*)car(cell->arr);
}
struct byteArray* bcdr(struct byteArray *cell){
	return (struct byteArray*)cdr(cell->arr);
}

void *leaf;
void *pair;
void *error;
void* tcType(struct byteArray *tc){
	return (void*)bcar(tc);
}
int tcConsp(struct byteArray *tc){
	return pair == tcType(tc);
}
struct byteArray* tcCar(struct byteArray *tc){
	if(!tc) return 0;
	if(error == tc) return error;
	if(!tcConsp(tc)) return error;
	return bcar(bcdr(tc));
}
struct byteArray* tcCdr(struct byteArray *tc){
	if(!tc) return 0;
	if(error == tc) return error;
	if(!tcConsp(tc)) return error;
	return bcdr(bcdr(tc));
}

void *iota;
int barrMain(struct byteArray *arfs){

	struct byteArray *test = simpleCons(pair, simpleCons(simpleCons(leaf, leaf), simpleCons(leaf, (void*)0)));
	if(leaf == tcType(bcdr(bcdr(test))))
		if(0 == (void*)bcdr(bcdr(bcdr(test))))
			printf("cddr is (cons leaf null)\n");
	freeBarr(bcdr(bcdr(test)));
	if(leaf == tcType(bcar(bcdr(test))))
		if(leaf == (void*)bcdr(bcar(bcdr(test))))
			printf("cadr is (cons leaf leaf)\n");
	freeBarr(bcar(bcdr(test)));
	if(pair == tcType(test));
		printf("car is pair\n");
	freeBarr(test);

	while(arfs){
		printf("%s", (char*)car(arfs->arr));
		arfs = (struct byteArray*)cdr(arfs->arr);
		if(arfs)
			printf(" ");
	}
	printf("\n");


	return 0;
}

int main(int arfc, char* *arfv){
	int result;
	struct byteArray *arfs = ptrArrToPcbl(arfc, (void**)arfv);
	result = barrMain(arfs);
	freePcbl(arfs);
	return result;
}

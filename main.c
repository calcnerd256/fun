//stddef.h
typedef long unsigned int size_t;


//stdio.h
extern int printf(char*, ...);

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
void *cstr;//null-terminated string buffer
void* tcType(struct byteArray *tc){
	return (void*)bcar(tc);
}
int tcConsp(struct byteArray *tc){
	return pair == tcType(tc);
}
int tcAtomp(struct byteArray *tc){
	if(cstr == tcType(tc)) return 1;
	return leaf == tcType(tc);
}
void* tcValue(struct byteArray *tc){
	if(!tc) return 0;
	if(error == tc) return error;
	return bcdr(tc);
}
struct byteArray* tcCar(struct byteArray *tc){
	struct byteArray* tcVal = (struct byteArray*)tcValue(tc);
	if(!tcVal) return 0;
	if(error == tcVal) return error;
	if(!tcConsp(tc)) return error;
	return bcar(tcVal);
}
struct byteArray* tcCdr(struct byteArray *tc){
	struct byteArray* tcVal = (struct byteArray*)tcValue(tc);
	if(!tcVal) return 0;
	if(error == tcVal) return error;
	if(!tcConsp(tc)) return error;
	return bcdr(tcVal);
}
void tcFreeTree(struct byteArray *tc){
	struct byteArray* stack = 0;
	struct byteArray* ptr;
	stack = simpleCons(tc, stack);
	while(stack){
		tc = bcar(stack);
		ptr = bcdr(stack);
		freeBarr(stack);
		stack = ptr;
		if(tcConsp(tc)){
			stack = simpleCons(tcCdr(tc), stack);
			stack = simpleCons(tcCar(tc), stack);
		}
		freeBarr(tc);
	}
}
struct byteArray *tcCons(void *head, void *tail){
	return simpleCons(pair, simpleCons(head, tail));
}
struct byteArray *tcPtr(void *value){
	return simpleCons(leaf, value);
}

void *iota;
void tcPrintAtom(struct byteArray* tc){
	char* ptr;
	if(!tc || !tcValue(tc)){
		printf("nil");
		return;
	}
	if(tcConsp(tc)){
		printf("somecons");
		return;
	}
	if(!tcAtomp(tc)){
		printf("<??\?>");
		return;
	}
	if(cstr == tcType(tc)){
		printf("\"");
		ptr = (char*)tcValue(tc);
		while(*ptr){
			if('\\' == *ptr || '\"' == *ptr)
				printf("\\");
			printf("%c", *ptr++);
		}
		printf("\"");
		return;
	}
	if(iota == tcValue(tc)){
		printf("iota");
		return;
	}
	printf("<%p>", tcValue(tc));
}
void tcPrintDump(struct byteArray* tc){
	struct byteArray* ptr;
	struct byteArray* stack = 0;
	if(!tc){
		printf("nil");
		return;
	}
	if(!tcConsp(tc)){
		tcPrintAtom(tc);
		return;
	}
	stack = simpleCons(&ptr, stack);// print ")" later
	stack = simpleCons(tc, stack);
	printf("(");
	while(stack){
		//pop tc
		tc = bcar(stack);
		ptr = bcdr(stack);
		freeBarr(stack);
		stack = ptr;

		if((void*)&ptr == tc)
			printf(")");
		else if((void*)&stack == tc)
			printf(" . ");
		else if((void*)&tc == tc)
			printf(" ");
		else if(tcConsp(tc)){
			//print cdr later
			if(tcValue(tcCdr(tc))){
				stack = simpleCons(tcCdr(tc), stack);
				stack = simpleCons(
					tcAtomp(tcCdr(tc)) ?
						&stack : // " . "
						&tc, // " "
					stack
				);
			}

			//print car later
			if(tcConsp(tcCar(tc))){
				stack = simpleCons(&ptr, stack);// print ")" later
				printf("(");
			}
			stack = simpleCons(tcCar(tc), stack);
		}
		else
			tcPrintAtom(tc);
	}
}
struct byteArray *reversePcbl(struct byteArray *ps){
	struct byteArray *result = 0;
	while(ps){
		result = simpleCons(bcar(ps), result);
		ps = bcdr(ps);
	}
	return result;
}
struct byteArray *pcblToTc(struct byteArray *pcbl){
	struct byteArray *result;
	struct byteArray *ptr;
	struct byteArray *backwards = reversePcbl(pcbl);
	ptr = backwards;
	result = tcPtr(0);
	while(ptr){
		result = tcCons(tcPtr(bcar(ptr)), result);
		ptr = bcdr(ptr);
	}
	freePcbl(backwards);
	return result;
}
struct byteArray *argsToTc(int arfc, char* *arfv){
	struct byteArray *result = tcPtr(0);
	while(arfc)
		result = tcCons(simpleCons(cstr, arfv[--arfc]), result);
	return result;
}

struct byteArray *iotaGen(struct byteArray *operand){
	//(iota operand) = (operand S K)
	return tcCons(tcPtr(iota), operand);
}
struct byteArray *I(){
	//i i x = i S K x
	// = S S K K x
	// = S K (K K) x
	// = K x (K K x = K)
	// = x
	//i i x = x
	// && I x = x
	// => i i = I
	return iotaGen(tcPtr(iota));
}
struct byteArray *ChurchZero(){
	//i I x y = I S K x y
	// = S K x y
	// = K y (x y)
	// = y
	//0 x y = y
	//i I = 0
	//K I x y = I y = y
	return iotaGen(I());
}
struct byteArray *K(){
	//i 0 = 0 S K
	// = K I S K
	// = I K
	// = K
	return iotaGen(ChurchZero());
}
struct byteArray *S(){
	//i K = K S K
	// = S
	return iotaGen(K());
}
int iotaTest(struct byteArray *arfs){
	//I really should be doing refcounting first...
	struct byteArray *expr = tcCons(
		tcCons(
			tcCons(S(), I()),
			I()
		),
		tcCons(
			tcCons(S(), I()),
			I()
		)
	);
	printf("%p\n", expr);
	tcPrintDump(expr);
	printf("\n");
        tcFreeTree(expr);
	return 0;
}

int barrMain(struct byteArray *arfs){

	tcPrintDump(arfs);
	printf("\n");

	while(tcConsp(arfs)){
		printf("%s", (char*)tcValue(tcCar(arfs)));
		arfs = tcCdr(arfs);
		if(tcValue(arfs))
			printf(" ");
	}
	printf("\n");

	return iotaTest(arfs);
}

void pointAtSelf(void* *ptr){
	*ptr = ptr;
}
int main(int arfc, char* *arfv){
	int result;
	pointAtSelf(&leaf);
	pointAtSelf(&pair);
	pointAtSelf(&error);
	pointAtSelf(&iota);
	pointAtSelf(&cstr);

	struct byteArray *arfs = argsToTc(arfc, arfv);
	result = barrMain(arfs);
	tcFreeTree(arfs);
	return result;
}

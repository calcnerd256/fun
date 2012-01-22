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
struct byteArray* bcar(struct byteArray *cell){
	return (struct byteArray*)car(cell->arr);
}
struct byteArray* bcdr(struct byteArray *cell){
	return (struct byteArray*)cdr(cell->arr);
}
void freePcbl(struct byteArray *arr){
	struct byteArray *tail;
	while(arr){
		tail = bcdr(arr);
		freeBarr(arr);
		arr = tail;
	}
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
int tcCdrHeavyIotaTreep(struct byteArray* tc){// no cycle detection!
	while(tc){
		if(iota == tcValue(tc))
			return 1;
		if(!tcConsp(tc))
			return 0;
		if(iota != tcValue(tcCar(tc)))
			return 0;
		tc = tcCdr(tc);
	}
	return 0;
}
int tcIotaSpecialp(struct byteArray* tc){
	//only covers i, I, 0, K, and S
	if(!tcCdrHeavyIotaTreep(tc)) return 0;
	if(!tcConsp(tc))
		return 1;//iota
	tc = tcCdr(tc);
	if(!tcConsp(tc))
		return 1;//I
	tc = tcCdr(tc);
	if(!tcConsp(tc))
		return 1;//0
	tc = tcCdr(tc);
	if(!tcConsp(tc))
		return 1;//K
	return iota == tcValue(tcCdr(tc));//S
}
int tcPrintIotaSpecial(struct byteArray* tc){
	//assume tcIotaSpecialp
	if(iota == tcValue(tc))
		return printf("iota");
	tc = tcCdr(tc);
	if(iota == tcValue(tc))
		return printf("I");
	tc = tcCdr(tc);
	if(iota == tcValue(tc))
		return printf("0");
	tc = tcCdr(tc);
	if(iota == tcValue(tc))
		return printf("K");
	return printf("S");
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
		else if(tcIotaSpecialp(tc))
			tcPrintIotaSpecial(tc);
		else if(tcConsp(tc)){
			//print cdr later
			if(tcValue(tcCdr(tc))){
				stack = simpleCons(tcCdr(tc), stack);
				stack = simpleCons(
					tcAtomp(tcCdr(tc)) ?
						&stack : // " . "
						tcIotaSpecialp(tcCdr(tc)) ?
							&stack : // " . "
							&tc, // " "
					stack
				);
			}

			if(!tcIotaSpecialp(tcCar(tc))){
				//print car later
				if(tcConsp(tcCar(tc))){
					stack = simpleCons(&ptr, stack);// print ")" later
					printf("(");
				}
				stack = simpleCons(tcCar(tc), stack);
			}
			else
				tcPrintIotaSpecial(tcCar(tc));
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

void tcStackPush(struct byteArray* *stack, void *val){
	*stack = tcCons(val, *stack);
}
struct byteArray *tcEvalIotaDefinitionStepLeak(struct byteArray *expr){
	struct byteArray *leakStack = tcPtr(0);
	//assume all checking has been done
	expr = tcCons(tcCons(tcCdr(expr), S()), K());
	tcStackPush(&leakStack, tcPtr(expr));// (x . S) . K
	tcStackPush(&leakStack, tcCdr(expr));// K
	tcStackPush(&leakStack, tcPtr(tcCar(expr)));// x . S
	tcStackPush(&leakStack, tcCdr(tcCar(expr)));// S
	return tcCons(expr, leakStack);
}
void freeTcCons(struct byteArray* tc){
	//only frees the cons cell, not its contents
	//a tcCons is a simple cons whose car is not to be freed and whose cdr is a simple cons
	freeBarr(bcdr(tc));
	freeBarr(tc);
}
struct byteArray *tcStackSlinky(struct byteArray *src, struct byteArray *dest){
	struct byteArray *ptr = src;
	while(src && tcConsp(src)){
		ptr = src;
		tcStackPush(&dest, tcCar(src));
		src = tcCdr(src);
		freeTcCons(ptr);
	}
	if(src)
		freeBarr(src);
	return dest;
}
int cdrwiseHeight(struct byteArray *tree){
	//return number of cdrs from root to cdrmost leaf
	int result = 0;
	while(tree && tcConsp(tree)){
		tree = tcCdr(tree);
		++result;
	}
	return result;
}
struct byteArray *tcIotaEvalStepLeak(struct byteArray *expr){
	struct byteArray *leakStack = tcPtr(0);
	struct byteArray *result = 0;
	struct byteArray *ptr = 0;
	struct byteArray *expar = 0;
	struct byteArray *expdr = 0;
	struct byteArray *expaar = 0;
	struct byteArray *expdar = 0;
	int n = 0;
	if(!expr) return tcCons(expr, leakStack);
	if(iota == tcValue(expr)) return tcCons(expr, leakStack);
	if(!tcConsp(expr)) return tcCons(expr, leakStack);
	expar = tcCar(expr);
	expdr = tcCdr(expr);
	if(iota == tcValue(expar)){
		//check it's not deeper than iS (which becomes S S K = \ x y . S S K x = \ x y . S x (K x) y = \ x y . x y x)
		if(iota == tcValue(expdr))
			//it's I
			return tcCons(expr, leakStack);
		if(!tcConsp(expdr))
			return tcEvalIotaDefinitionStepLeak(expr);
		if(tcIotaSpecialp(expr))
			return tcCons(result, leakStack);
		//i (x y)
		if(tcCdrHeavyIotaTreep(expr)){
			//now, which one is it?
			//it must be too long already
			//so maybe we can cut to the chase and cut it short?
			//nope, because i (i S) = I and i i = I but we don't yet detect i S
			//ah, but I can check its length
			n = cdrwiseHeight(expr);
			if(n < 6) return tcCons(expr, leakStack);
			//okay, beyond that, it's 6=2, so it's ((n-2)%6)+2, or n - 4 * (n-2)/4
			//that is, take the (n-2)&(~3)th cdr
			n = (n-2)&~3;
			while(n--) expr = tcCdr(expr);
			return tcCons(expr, leakStack);
		}
		if(iota != tcValue(tcCar(expdr))){
			//i (x y) = x y S K as above
			result = tcEvalIotaDefinitionStepLeak(expr);
			ptr = tcIotaEvalStepLeak(tcCar(result));
			leakStack = tcStackSlinky(tcCdr(result), tcCdr(ptr));//frees tcCdr(result)
			freeTcCons(result);
			result = tcCons(tcCar(ptr), leakStack);
			freeTcCons(ptr);
			return result;
		}
		//i (i x)
		//i (i x) = i x S K
		// = x S K S K
		//if x is then i, I, 0, K, then we have a special form that's already been taken care of
		//if x is equivalent to one of those or to S or (i S), then we would like to simplify it
	}
	if(!tcConsp(expar)){
		//TODO: recurse cdrwise
	}
	expaar = tcCar(expar);
	expdar = tcCdr(expar);
	//check I is car
	//if I is car, return cdr
	//caar is iota
	//cdar is iota
	if(tcIotaSpecialp(expar))
		if(iota == tcValue(expaar)){
			if(iota == tcValue(expdar))
				return tcCons(expdr, leakStack);
			//check 0 is car
			//if 0 is car, return I
			//then cdar should be I
			if(tcConsp(expdar))
				if(iota == tcValue(tcCar(expdar)))
					if(iota == tcValue(tcCdr(expdar)))
						return tcCons(expdar, leakStack);//reuse the I in the 0
			//check K is car
			//check S is car
			//well, both of those cases act the same, so...
			//TODO: recurse cdrwise, delay execution of incomplete higher-order functions K and S
		}
	if(tcIotaSpecialp(expaar))
		//check K is caar
		//iota is caaar
		//0 is cdaar
		//iota is cadaar
		//I is cddaar
		if(tcConsp(expaar))
			if(iota == tcValue(tcCar(expaar)))
				if(tcConsp(tcCdr(expaar)))
					if(iota == tcValue(tcCar(tcCdr(expaar))))
						//check I is cddaar
						//iota is caddaar
						//iota is cdddaar
						if(tcConsp(tcCdr(tcCdr(expaar))))
							if(iota == tcValue(tcCar(tcCdr(tcCdr(expaar))))){
								if(iota == tcValue(tcCdr(tcCdr(tcCdr(expaar)))))
									//if K is caar, return cdar
									return tcCons(expdar, leakStack);
								//check S is caar
								//I is cdddaar
								//iota is cadddaar
								//iota is cddddaar
								if(tcConsp(tcCdr(tcCdr(tcCdr(expaar)))))
									if(iota = tcValue(tcCar(tcCdr(tcCdr(tcCdr(expaar))))))
										if(iota = tcValue(tcCdr(tcCdr(tcCdr(tcCdr(expaar)))))){
											//TODO: recurse cdrwise and recurse upon cdar
										}
							}
	//check S is caaar
	if(tcConsp(expaar))
		//iota is caaaar
		//K is cdaaar
		if(tcConsp(tcCar(expaar)))
			if(iota == tcValue(tcCar(tcCar(expaar))))
				//iota is cadaaar
				//0 is cddaaar
				if(tcConsp(tcCdr(tcCar(expaar))))
					if(iota == tcValue(tcCar(tcCdr(tcCar(expaar)))))
						//iota is caddaaar
						//I is cdddaaar
						if(tcConsp(tcCdr(tcCdr(tcCar(expaar)))))
							if(iota == tcValue(tcCar(tcCdr(tcCdr(tcCar(expaar))))))
								if(tcConsp(tcCdr(tcCdr(tcCdr(tcCar(expaar))))))
									//iota is cadddaaar
									//iota is cddddaaar
									if(iota == tcValue(tcCar(tcCdr(tcCdr(tcCdr(tcCar(expaar)))))))
										if(iota == tcValue(tcCdr(tcCdr(tcCdr(tcCdr(tcCar(expaar))))))){
											//if S is caaar, return cdaar cdr (cdar cdr)
											//TODO
										}
	//recurse carwise
	//recurse cdrwise?
	return tcCons(expr, leakStack);
}

//a leak stack is a true-list whose conses are tcCons
//an element of a leak stack is either a tcCons tree or a tcAtomp whose tcValue is to be freed as a barr
void freeLeakStack(struct byteArray *stack){
	struct byteArray *ptr = stack;
	while(stack && tcConsp(stack)){
		if(tcAtomp(tcCar(stack))){
			ptr = tcValue(tcCar(stack));
			freeTcCons(ptr);
		}
		stack = tcCdr(stack);
	}
}

int iotaTest(struct byteArray *arfs){
	//I really should be doing refcounting first...
	struct byteArray *expr = //iotaGen(iotaGen(iotaGen(S())));
		tcCons(tcCons(tcCons(S(), tcPtr((void*)3)), tcPtr((void*)1)), tcPtr((void*)2));
		//iotaGen(tcCons(tcPtr((void*)1), tcPtr((void*)2)));
		/*tcCons(
		tcCons(
			tcCons(S(), I()),
			I()
		),
		tcCons(
			tcCons(S(), I()),
			I()
		)
	);*/
	tcPrintDump(expr);
	printf("\n");
	struct byteArray *stepMem = tcIotaEvalStepLeak(expr);
	tcPrintDump(stepMem);
	printf("\n");
	tcPrintDump(tcCar(stepMem));
	printf("\n");
	freeLeakStack(tcCdr(stepMem));
	tcFreeTree(tcCdr(stepMem));
	freeTcCons(stepMem);
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

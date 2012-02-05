#include "types.h"
#include "tc.h"
#include "iota.tc.h"
#include "io.h"

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
struct byteArray *nopLeak(struct byteArray *expr){
	return tcCons(expr, tcPtr((void*)0));
}
struct byteArray *tcIotaSimplifyDeepSpecial(struct byteArray *expr){
	if(!expr) return nopLeak(expr);
	if(!tcConsp(expr)) return nopLeak(expr);
	if(!tcCdrHeavyIotaTreep(expr)) return nopLeak(expr);
	//now, which one is it?
	//it must be too long already
	//so maybe we can cut to the chase and cut it short?
	//nope, because i (i S) = I and i i = I but we don't yet detect i S
	//ah, but I can check its length
	//okay, beyond that, it's 6=1, so it's ((n-1)%6)+1, or n - (6 * (n-1)/6 - 1)
		//by 6=1, I mean that i (i S) = i (i (i (i (i (i i))))) = i i
	//that is, take the 6*((n-1)/6) th cdr	int n = cdrwiseHeight(expr);
	int n = cdrwiseHeight(expr);
	if(n < 6) return nopLeak(expr);
	n = 6 * ((n - 1) / 6) - 1;
	if(n >= 0)
		while(n--)
			expr = tcCdr(expr);
	return nopLeak(expr);
}
struct byteArray *tcEvalStepCombineLeaks(struct byteArray *result, struct byteArray *temp){
	//temp starts out with the answer, and result ends up with it
	struct byteArray *leakStack = tcStackSlinky(tcCdr(result), tcCdr(temp));
	freeTcCons(result);
	result = tcCons(tcCar(temp), leakStack);
	freeTcCons(temp);
	return result;
}
struct byteArray *tcIotaEvalStepLeak(struct byteArray *expr){
	//struct byteArray *leakStack = 0;
	struct byteArray *result = 0;
	struct byteArray *expar = 0;
	struct byteArray *expdr = 0;
	struct byteArray *expaar = 0;
	struct byteArray *expdar = 0;
	struct byteArray *expaaar = 0;
	struct byteArray *expdaar = 0;
	if(!expr) return nopLeak(expr);
	if(iota == tcValue(expr)) return nopLeak(expr);
	if(!tcConsp(expr)) return nopLeak(expr);
	expar = tcCar(expr);
	expdr = tcCdr(expr);
	if(iota == tcValue(expar)){
		//check it's not deeper than iS (which becomes S S K = \ x y . S S K x = \ x y . S x (K x) y = \ x y . x y x)
		if(iota == tcValue(expdr))
			//it's I
			return nopLeak(expr);
		if(!tcConsp(expdr))
			return tcEvalIotaDefinitionStepLeak(expr);
		if(tcIotaSpecialp(expr))
			return nopLeak(result);
		//i (x y)
		if(tcCdrHeavyIotaTreep(expr))
			return tcIotaSimplifyDeepSpecial(expr);
		if(iota != tcValue(tcCar(expdr))){
			//i (x y) = x y S K as above
			result = tcEvalIotaDefinitionStepLeak(expr);
			return tcEvalStepCombineLeaks(
				result,
				tcIotaEvalStepLeak(tcCar(result))
			);
		}
		//i (i x)
		//i (i x) = i x S K
		// = x S K S K
		//if x is then i, I, 0, K, then we have a special form that's already been taken care of
		//if x is equivalent to one of those or to S or (i S), then we would like to simplify it

		//TODO: recurse appropriately
		//TODO: get and eval x=cddr expr
		//TODO: collect S, K, S, K, x S, x S K, x S K S K
		//TODO: can I use tcEvalIotaDefinitionStepLeak twice?
		return nopLeak(expr);
	}
	if(!tcConsp(expar)){
		//TODO: recurse cdrwise
		return nopLeak(expr);
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
				return nopLeak(expdr);
			//check 0 is car
			//if 0 is car, return I
			//then cdar should be I
			if(tcConsp(expdar))
				if(iota == tcValue(tcCar(expdar)))
					if(iota == tcValue(tcCdr(expdar)))
						return nopLeak(expdar);//reuse the I in the 0
			//check K is car
			//check S is car
			//well, both of those cases act the same, so...
			//TODO: recurse cdrwise, delay execution of incomplete higher-order functions K and S
			return nopLeak(expr);
		}
	if(!tcConsp(expaar)){
		//TODO: recurse appropriately
		return nopLeak(expr);
	}
	expaaar = tcCar(expaar);
	expdaar = tcCar(expaar);
	if(tcIotaSpecialp(expaar))
		//check K is caar
		//iota is caaar
		//0 is cdaar
		//iota is cadaar
		//I is cddaar
		if(iota == tcValue(expaaar))
			if(tcConsp(tcCdr(expaar)))
				if(iota == tcValue(tcCar(expdaar)))
					//check I is cddaar
					//iota is caddaar
					//iota is cdddaar
					if(tcConsp(tcCdr(expdaar)))
						if(iota == tcValue(tcCar(tcCdr(expdaar)))){
							if(iota == tcValue(tcCdr(tcCdr(expdaar))))
								//if K is caar, return cdar
								return nopLeak(expdar);
							//check S is caar
							//I is cdddaar
							//iota is cadddaar
							//iota is cddddaar
							if(tcConsp(tcCdr(tcCdr(expdaar))))
								if(iota == tcValue(tcCar(tcCdr(tcCdr(expdaar)))))
									if(iota == tcValue(tcCdr(tcCdr(tcCdr(expdaar))))){
										//TODO: recurse cdrwise and recurse upon cdar
										return nopLeak(expr);
									}
						}
	//check S is caaar
	//iota is caaaar
	//K is cdaaar
	if(tcConsp(expaaar))
		if(iota == tcValue(tcCar(expaaar)))
			//iota is cadaaar
			//0 is cddaaar
			if(tcConsp(tcCdr(expaaar)))
				if(iota == tcValue(tcCar(tcCdr(expaaar))))
					//iota is caddaaar
					//I is cdddaaar
					if(tcConsp(tcCdr(tcCdr(expaaar))))
						if(iota == tcValue(tcCar(tcCdr(tcCdr(expaaar)))))
							if(tcConsp(tcCdr(tcCdr(tcCdr(expaaar)))))
								//iota is cadddaaar
								//iota is cddddaaar
								if(iota == tcValue(tcCar(tcCdr(tcCdr(tcCdr(expaaar))))))
									if(iota == tcValue(tcCdr(tcCdr(tcCdr(tcCdr(expaaar)))))){
										//if S is caaar, return cdaar cdr (cdar cdr)
										//TODO
										return nopLeak(expr);
									}
	//recurse carwise
	//recurse cdrwise?
	return nopLeak(expr);
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
	struct byteArray *expr = iotaGen(iotaGen(iotaGen(S())));
		//tcCons(tcCons(tcCons(S(), tcPtr((void*)3)), tcPtr((void*)1)), tcPtr((void*)2));
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
int init(){
	pointAtSelf(&leaf);
	pointAtSelf(&pair);
	pointAtSelf(&error);
	pointAtSelf(&iota);
	pointAtSelf(&cstr);
	return 0;
}
int main(int arfc, char* *arfv){
	int result;
	result = init();
	if(result) return result;
	struct byteArray *arfs = argsToTc(arfc, arfv);
	result = barrMain(arfs);
	tcFreeTree(arfs);
	return result;
}

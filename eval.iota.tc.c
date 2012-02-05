#include "types.h"
#include "tc.h"
#include "iota.tc.h"
#include "eval.iota.tc.h"


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
struct byteArray *nopLeak(struct byteArray *expr){
	return tcCons(expr, tcPtr((void*)0));
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

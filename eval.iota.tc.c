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

struct byteArray *tcEvalIotaDefLeak(struct byteArray *expr, struct byteArray *expdr, struct byteArray *result){
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

//suddenly, a brilliant or terrible idea: write the evaluation step in a simpler language which itself we evaluate from conses

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
	if(iota == tcValue(expar))
		return tcEvalIotaDefLeak(expr, expdr, result);

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

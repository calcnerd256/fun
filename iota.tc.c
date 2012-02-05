#include "iota.tc.h"

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

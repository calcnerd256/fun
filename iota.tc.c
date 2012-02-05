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

#include "barr.h"
#include "cons.h"

struct byteArray *simpleCons(void*, void*);
struct byteArray *bcar(struct byteArray*);
struct byteArray *bcdr(struct byteArray*);

struct byteArray *ptrArrToPcbl(int, void**);
struct byteArray *ptrBarrToPcbl(struct byteArray*);
void freePcbl(struct byteArray*);

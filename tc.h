#include "barr.cons.h"

void *leaf;
void *pair;
void *error;
void *cstr;//null-terminated string buffer

struct byteArray *tcCons(void*, void*);
struct byteArray *tcPtr(void*);
void* tcType(struct byteArray*);
int tcConsp(struct byteArray*);
int tcAtomp(struct byteArray*);
void* tcValue(struct byteArray*);
struct byteArray *tcCar(struct byteArray*);
struct byteArray *tcCdr(struct byteArray*);
void freeTcCons(struct byteArray*);
void tcFreeTree(struct byteArray*);

struct byteArray *pcblToTc(struct byteArray*);
void tcStackPush(struct byteArray**, void*);
struct byteArray *tcStackSlinky(struct byteArray*, struct byteArray*);

int cdrwiseHeight(struct byteArray*);

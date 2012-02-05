#include "barr.h"
#include "cons.h"

void *leaf;
void *pair;

struct byteArray *simpleCons(void*, void*);
struct byteArray *bcar(struct byteArray*);
struct byteArray *bcdr(struct byteArray*);

struct byteArray *tcCons(void*, void*);
struct byteArray *tcPtr(void*);

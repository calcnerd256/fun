#include "barr.h"
#include "cons.h"

void *leaf;
void *pair;

struct byteArray *simpleCons(void*, void*);

struct byteArray *tcCons(void*, void*);
struct byteArray *tcPtr(void*);

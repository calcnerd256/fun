#include "tc.h"

struct byteArray *simpleCons(void *head, void *tail){
	struct byteArray *result = allocBarr(sizeof(struct consCell));
	struct consCell *cell = (struct consCell*)(result->arr);
	cell->head = head;
	cell->tail = tail;
	return result;
}


struct byteArray *tcCons(void *head, void *tail){
	return simpleCons(pair, simpleCons(head, tail));
}
struct byteArray *tcPtr(void *value){
	return simpleCons(leaf, value);
}

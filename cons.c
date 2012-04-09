#include "cons.h"

void *setCar(struct consCell *cell, void* value){
	void* result = cell->head;
	cell->head = value;
	return result;
}
void *setCdr(struct consCell *cell, void* value){
	void* result = cell->tail;
	cell->tail = value;
	return result;
}
struct consCell *car(char *cell){
	return (struct consCell*)(
		((struct consCell*)cell)->head
	);
}
struct consCell *cdr(char *cell){
	return (struct consCell*)(
		((struct consCell*)cell)->tail
	);
}

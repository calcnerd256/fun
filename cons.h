struct consCell{
	void *head;
	void *tail;
};

void *setCar(struct consCell*, void*);
void *setCdr(struct consCell*, void*);
struct consCell *car(char*);
struct consCell *cdr(char*);

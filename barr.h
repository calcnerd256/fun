struct byteArray{
	size_t size;
	char* arr;
};

struct byteArray *referenceBarr(size_t, void*);
struct byteArray *allocBarr(size_t);

void freeRbarr(struct byteArray*);
void freeBarr(struct byteArray*);

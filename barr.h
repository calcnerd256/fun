//stddef.h
typedef long unsigned int size_t;

struct byteArray{
	size_t size;
	char* arr;
};

struct byteArray *referenceBarr(size_t, void*);
struct byteArray *allocBarr(size_t);

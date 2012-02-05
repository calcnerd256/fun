//stdio.h
extern int printf(char*, ...);

//string.h
extern size_t strlen(char*);

struct byteArray *strToRbarr(char*);
void printBarr(struct byteArray*);
struct byteArray *argsToBarr(int, char**);
void freeArgsBarr(struct byteArray*);

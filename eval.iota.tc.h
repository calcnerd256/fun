struct byteArray *tcEvalIotaDefinitionStepLeak(struct byteArray*);
struct byteArray *nopLeak(struct byteArray*);
void freeLeakStack(struct byteArray*);
struct byteArray *tcIotaSimplifyDeepSpecial(struct byteArray*);
struct byteArray *tcEvalStepCombineLeaks(struct byteArray*, struct byteArray*);

#ifndef mymalloc_h_  
#define mymalloc_h_ 

void addNodeUsed(int address,int size);

void addNodeFree(int address,int size);

void MyFree(void *pointer);

void *MyMalloc(size_t n);

#endif 



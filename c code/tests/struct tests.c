#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct t{
	char name[20];
	char value[30];
}Token;
typedef struct t_array{
    size_t used;
    size_t size;
    Token *array;
}TokenArray;
void initToken(Token* t , char name[20],char value[30]){
    strcpy(t->name,name);
    strcpy(t->value,value);
}
void initArray(TokenArray *a, size_t initialSize) {
    a->array = malloc(initialSize * sizeof(Token));
    if (initialSize!=0 && a->array==NULL){
        printf("no memory");
    }
    else{
        a->used = 0;
        a->size = initialSize;
    }
}

void insertArray(TokenArray *a, Token element) {
    if (a->used == a->size) {
        a->size *= 2;
        void* temp = realloc(a->array, a->size*sizeof(Token));
        if (temp == NULL) printf("no memory");
        else a->array=temp;
    }
    a->array[a->used++] = element;
}

void freeArray(TokenArray *a) {
    free(a->array);
    a->array = NULL;
    a->used = a->size = 0;
}
void main(){
    TokenArray tar;
    initArray(&tar,5);
    Token t;
    initToken(&t,"muda muda","GIOVANI GORGO");
    printf("%s\n",t.name);
    insertArray(&tar,t);
    printf("%s\n",tar.array[0].value);
    freeArray(&tar);
}


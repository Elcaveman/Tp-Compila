#ifndef LEXER_H
#define LEXER_H
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define True 1
#define False 0
//Token/Array Utililities
typedef struct token_t{
    char name[30];
    char value[50];
}Token;
typedef struct t_array{
    size_t used;
    size_t size;
    Token *array;
}TokenArray;

// ultils : struct management
void initToken(Token* token_adr , char name[30] , char value[50]);
void initArray(TokenArray *a, size_t initialSize);
void insertArray(TokenArray *a, Token element);
void freeArray(TokenArray *a);
// utils : literals check 
int isDigit(char character);
int isWord(char character);
int uncapCmp(char c1,char c2);
// utilis : token generation by comparing strings
int getNormalToken(Token* token_adr,char name[30],char value[50],char* buffer,int i);
// main function: gets a filename and returns all the tokens in it 
void RunLexer(TokenArray* token_list,const char* filename);

#endif
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "erno.h"

/* Error type includes: Syntax error / parse error .. */
void lineError(char* ErrorSource,int line, int column,char* ErrorMessage){
    int i;
    printf("%sError: at line %d column %d :",ErrorSource,line,column);
    printf("%s\n",ErrorMessage);
    scanf("%d",&i);
    //exit(ErrorCode);
    // exit(0);//error codes to be added later
}

void Error(char* ErrorMessage,int ErrorCode){
    int i;
    printf("%s\n",ErrorMessage);
    scanf("%d",&i);
    // exit(ErrorCode);
}
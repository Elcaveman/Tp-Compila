#ifndef ERNO_H
#define ERNO_H

#define INSUFFICIENT_MEMORY 0
#define INVALID_TOKEN 1
#define NUMBER_LEN_EXCEED 2 

void lineError(char* ErrorSource,int line, int column,char* ErrorMessage);
void Error(char* ErrorMessage,int ErrorCode);
#endif
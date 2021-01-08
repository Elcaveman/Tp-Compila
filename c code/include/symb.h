#ifndef SYMB_H
#define SYMB_H

#include "lexer.h"
// positive numbers are actual types
#define SYMTYPE_DECLARE_PROGRAM 0
#define SYMTYPE_DECLARE_USES 1
#define SYMTYPE_DECLARE_CONST 2
#define SYMTYPE_DECLARE_VAR 3
#define SYMTYPE_DECLARE_BLOCK 4
#define SYMTYPE_ASIGN 5 // includes := += ++...
typedef struct symb{
    Token identifer;
    Token value; // NULL except if type==ASIGN
    int type; // not a token since the types we use are limited and can be non terminals
    struct symb scope;
    //NULL if scope is global/ symb if block level 
}Symbol;

typedef struct s_array{
    size_t used;
    size_t size;
    Symbol *array;
}SymbolArray;
/*exemples de symbol:
    Block symbols:
        Global : [NULL,NULL,4,NULL] the root of SymbolArray 
        
        Function: [function_name,t_function,4,Global]
        
        Procedure inside a function:
        [proc_name,t_procedure,4,Function]
        
        If inside that Procedure: [NULL,t_IF,4,Procedure]
    Declarations:
        Program: [NULL,program_name,0,Global]
        Uses : [NULL,imported_prog,1,Global]
        var declaration in main: [var_name,NULL,2,Global]
        var declaration and init in main: [var_name,var_vallue,2,Global]
         
*/
// ultils : struct management
void initSymbol(Symbol* symb_adr , Token id,Token val , int type, Token scope);
void initSymbolArray(SymbolArray *a, size_t initialSize);
void insertSymbolArray(SymbolArray *a, Symbol element);
void freeSymbolArray(SymbolArray *a);
// semantic rules


#endif
#ifndef SYMB_H
#define SYMB_H

#include "lexer.h"
#define SYMTYPE_DECLARE_BLOCK 0
#define SYMTYPE_DECLARE_PROGRAM 1
#define SYMTYPE_DECLARE_USES 2
#define SYMTYPE_DECLARE_CONST 3
#define SYMTYPE_DECLARE_VAR 4
#define SYMTYPE_DECLARE_CHAR 5
#define SYMTYPE_DECLARE_INT 6
#define SYMTYPE_DECLARE_FLOAT 7
#define SYMTYPE_DECLARE_BOOL 8
#define SYMTYPE_DECLARE_STRING 9

#define SYMTYPE_DECLARE_INT 5
#define SYMTYPE_ASIGN 5 // includes := += ++...
typedef struct scope{
    Token type;
    Token name;
    //Global scope is presented as {NULL,NULL}
    //Nameless Scopes are: (IF,ELSE,WHILE,FOR)
    //Named Scopes are: Functions and Procedures
}Scope;

typedef struct symb{
    Token identifer;
    Token value;
    int type;
}Symbol;
typedef struct symb_array{
    size_t used;
    size_t size;
    Symbol *array;
}SymbolArray;

typedef struct scope_tree_node{
    Scope data;
    SymbolArray scope_symbols;
}ScopeNode;
typedef struct scope_tree{
    ScopeNode node;
    int sub_scopes_count;//aka len(sub_scopes)
    struct scope_tree* sub_scopes_head;
}ScopeTree;

/*exemples de symbol:
    Scope:
        Global : [NULL,NULL] the root of SymbolArray 
        Function: [t_function,function_name]
        If: [t_IF,NULL]

    symbols:
        Program: [NULL,program_name,PROGRAM]
        Uses : [NULL,imported_prog,USES]
        var declaration: [var_name,NULL,VAR]
        integer declaration and init: [var_name,var_value,INTEGER]
    asignement:
        var asignement after declaration:
            1- get current scope
            2- search for a symbol where:
                - sym.identifier == var.identifier
                - scope current_scope
                if found: sym.value = var.value
                else semantic-error: undeclared variable
*/
// ultils : struct management
void initSymbol(Symbol* symb_adr , Token id,Token val , int type, Symbol  scope);
void initSymbolArray(SymbolArray *a, size_t initialSize);
void insertSymbolArray(SymbolArray *a, Symbol element);
void freeSymbolArray(SymbolArray *a);
void initScope(Scope* scope_adr,Token type,Token name);
void initScopeTree(ScopeTree* scope_tree_adr,ScopeNode root);
void insertSubScope(ScopeTree* tree,ScopeTree* sub_tree);
void insertSymbolToTree(ScopeTree* tree,Symbol symbol);

/* semantic rules:
    I'll try to make my approach simmilar to describing data in a SQl
    0- Uses:
        Only usable for a symbol of type uses
        1- is only usable on a global scope
        2- the value of each symbol of that type is unique
    
    1- Unique type:
        Means there's only one symbol of this type at all time
        in our case: Program
    2- Unique Declaration in root+subtrees:
        A symbol of this kind has it's identifier set to unique whatever
        in the current scope and it's sub_scopes
        in our case: Functions , Procedures
        examples: 
          - function X;
            function Y;begin function X; end;
            
            This shouldn't work because X is already defined in the global scope
            and the scope {function,Y} is inside of the global scope

          - function X;begin function Y; end;
            function Z;begin function Y;end;
            This should work because the scopes {function,X} and {function,Z} are
            are not inclusive one another.

    3-  Unique Declaration in root:
        A symbol of this kind is declared only once in the same scope
        in our case: Varriables and constants
        examples:
            - var x:integer;
              var x =10;
              this won't work cause we declared x twice
            - begin
                var x:integer;
                procedure r();
                   begin
                     var x=10;
                   end;
               end.
               this will work because the scopes are different (the scopes need to 
               be equal not just inclusive) and also here's how the symbols look in 
               the scopeTree:
                    scope Global : {x,NULL,Integer}
                    scope procedure r : {x,10,VAR}
    4-  Unique assignement:
        A symbol of this kind can't have it's vallue changed once it's asgined in its
        scope
        in our case : constants
        example:
            - begin const x=10;
            if (true)then
                begin x=20;end;
            end.
            this won't work
            - begin const x=20;
            if (true)then
                begin var x=10; end;
            this will work because the declaration comes preceeds the asignement
            so it's not the same x basically
    5-  Unique data type for value:
        The value in a symbol is a token. the type of that token is also refered to as
        it's name. so a symbol of this kind has only one type of values accepted
        in our case symbols of type: INT ,FLOAT ,CHAR , STRING , BOOLEAN
            begin var x:integer;x=20; end. 
            parse tree-> {{
                    {NULL,NULL},
                    [{x,null,INTEGER}] -> [{x,20,INTEGER}]
                }
                0,{NULL,NULL}}
    6-  free
        A symbol of this kind is free of all the above conditions
*/
// scopes are also symbols they're just a bit special
int semantic_Program(ScopeTree* tree_root,ScopeTree* curent_tree,Symbol symbol);
int semantic_Uses(ScopeTree* tree_root,ScopeTree* curent_tree,Symbol symbol);
int semantic_UniqueDeclarationRS(ScopeTree* tree_root,ScopeTree* curent_tree,Symbol symbol, Token type);
int semantic_UniqueDeclarationR(ScopeTree* tree_root,ScopeTree* curent_tree,Symbol symbol, Token type);
int semantic_UniqueAssign(ScopeTree* tree_root,ScopeTree* curent_tree,Symbol symbol, Token type);
int semantic_UniqueDataType(ScopeTree* tree_root,ScopeTree* curent_tree,Symbol symbol, Token type);
int semantic_free(ScopeTree* tree_root,ScopeTree* curent_tree,Symbol symbol);

#endif
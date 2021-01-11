#ifndef PARSER_H
#define PARSER_H
#include "include/lexer.h"
#include "include/symb.h"
#include "include/erno.h"

//we'll be using a bottom up approach specifically the Operator precedence one. 
//the grammar for pascal is defined in pascal_grammar
int PROGRAM(TokenArray* t , scope_tree* root , int current_token);
int FPBLOCK(TokenArray* t , scope_tree* root , int current_token);
int FUNC(TokenArray* t , scope_tree* root , int current_token);
int PROC(TokenArray* t , scope_tree* root , int current_token);
int BLOCK(TokenArray* t , scope_tree* root , int current_token);
int FUNCARGS(TokenArray* t , scope_tree* root , int current_token);
int FUNCARG(TokenArray* t , scope_tree* root , int current_token);
int CONST(TokenArray* t , scope_tree* root , int current_token);
int VAR(TokenArray* t , scope_tree* root , int current_token);
int TUPLE(TokenArray* t , scope_tree* root , int current_token);
int DATATYPE(TokenArray* t , scope_tree* root , int current_token);
int LITERAL(TokenArray* t , scope_tree* root , int current_token);
int ASGNOP(TokenArray* t , scope_tree* root , int current_token);
int UNARYOP(TokenArray* t , scope_tree* root , int current_token);
int BINARYOP(TokenArray* t , scope_tree* root , int current_token);
int RELCOMPOP(TokenArray* t , scope_tree* root , int current_token);
int RELEQOP(TokenArray* t , scope_tree* root , int current_token);
int INSTS(TokenArray* t , scope_tree* root , int current_token);
int INST(TokenArray* t , scope_tree* root , int current_token);
int IF(TokenArray* t , scope_tree* root , int current_token);
int FOR(TokenArray* t , scope_tree* root , int current_token);
int WHILE(TokenArray* t , scope_tree* root , int current_token);
int PROMARY_EXP(TokenArray* t , scope_tree* root , int current_token);
int POSTFIX_EXP(TokenArray* t , scope_tree* root , int current_token);
int UNARY_EXP(TokenArray* t , scope_tree* root , int current_token);
int MULT_EXP(TokenArray* t , scope_tree* root , int current_token);
int ADD_EXP(TokenArray* t , scope_tree* root , int current_token);
int REL_EXP(TokenArray* t , scope_tree* root , int current_token);
int EQ_EXP(TokenArray* t , scope_tree* root , int current_token);
int AND_EXP(TokenArray* t , scope_tree* root , int current_token);
int OR_EXP(TokenArray* t , scope_tree* root , int current_token);
int ASGN_EXP(TokenArray* t , scope_tree* root , int current_token);
int EXPLIST_EXP(TokenArray* t , scope_tree* root , int current_token);


#endif
#include "include/lexer.h"
#include "include/erno.h"

void initToken(Token* token_adr , char name[30] , char value[50]){
    strcpy(token_adr->name,name);
    strcpy(token_adr->value,value);
}
void initArray(TokenArray *a, size_t initialSize) {
    a->array = malloc(initialSize * sizeof(Token));
    if (initialSize!=0 && a->array==NULL){
        Error("Insufficient memory",INSUFFICIENT_MEMORY);
    }
    else{
        a->used = 0;
        a->size = initialSize;
    }
}

void insertArray(TokenArray *a, Token element) {
    // a->used is the number of used entries, because a->array[a->used++] updates a->used only *after* the array has been accessed.
    // Therefore a->used can go up to a->size 
    if (a->used == a->size) {
        a->size *= 2;//e*n memory usage which is bad
        void* temp = realloc(a->array, a->size*sizeof(Token));
        if (temp == NULL) Error("Insufficient memory",INSUFFICIENT_MEMORY);
        else a->array=temp;
    }
    a->array[a->used++] = element;
}

void freeArray(TokenArray *a) {
    free(a->array);
    a->array = NULL;
    a->used = a->size = 0;
}
//Lexer utilities
int isDigit(char character){
    /* in terms of sheer performance switch case is faster than arithmetic operators */
    switch(character){
        case '0':
        case '1':
        case '2':
        case '3':
        case '4':
        case '5':
        case '6':
        case '7':
        case '8':
        case '9':return True;break;
        default: return False;
    }
}
int isWord(char character){
    if (('a'<=character && character<='z')||('A'<=character && character<='Z')||character == '_') return True;
    return False;
}
int uncapCmp(char c1,char c2){
    if(c1==c2)return True;
    else if(isWord(c1)&&isWord(c2)){
        if (c1<c2){
            if(c1+32==c2)return True;
        }
        else{
            if(c1==c2+32)return True;
        }
    }
    return False;
}
int getNormalToken(Token* token_adr,char name[30],char value[50],char* buffer,int i){
    Token t;
    int j=0;
    int len = strlen(value);
    while ((buffer[i+j]==value[j]||uncapCmp(buffer[i+j],value[j]))&& j<len){
        j++;
    }
    if (j==len){
        initToken(token_adr,name,value);
        return 1;
    }
    return 0;
    
}
// function to make Tokens from filename.pas 
void RunLexer(TokenArray* token_list,const char* filename){
    /* design error cuz lack of time: minimized .pas files won't work (no line breaks)*/
    FILE* file_pt;
    char buffer[420];
    
    initArray(token_list,100);

    int curr_line=1;
    int col = 1;

    int unbound_read = False;
    char unbound_stop;
    int escaped = False;

    file_pt = fopen(filename,"r");
    
    char normal_tokens[40][2][10] = {
        {"CLS_PAR",")"},{"DOT","."},
        {"ADR","@"},{"PTR","^"},{"OR","or"},{"NOT","not"},{"AND","and"},{"XOR","xor"},
        {"MOD","mod"},{"IF","if"},{"DO","do"},{"VAR","var"},{"THEN","then"},{"USES","uses"},{"WHILE","while"},{"CONST","const"},{"PROGRAM","program"},{"READ","read"},{"WRITE","write"},{"BEGIN","begin"},{"INT","integer"},{"FLOAT","real"},{"CHAR","char"},{"BOOl","bool"},{"STRING","string"}
        {"FUNCTION","function"},{"PROCEDURE","procedure"},{"WHILE","while"}
        {"TRUE","true"},{"FALSE","false"},
        };
    int total_Ntokens = 29;
    // char* leftovers = calloc(20,sizeof(char));/*maxx length of an identifier is 20*/
    // int leftovers_len=0;
    char* collector = malloc(sizeof(char)*20);
    while (fgets(buffer,420,(FILE*)file_pt)!=NULL){
        // we work with fgets like readline
        int i=0;
        while (i<strlen(buffer)){
            //remember to do a col+=i in the end
            if ((!unbound_read || (buffer[i]==unbound_stop) || buffer[i]=='\n')&&!escaped){
                Token t ;
                initToken(&t,"","");
                //if you get in here you must get one and only token!
                /*comment*/
                if (buffer[i]=='('){
                    if(buffer[i+1]=='*'){
                        initToken(&t,"MLCOMENT","");
                        insertArray(token_list,t);
                        i+=2;col+=2;
                        unbound_read = True;
                        unbound_stop = '*';
                    }
                    else{
                        initToken(&t,"OP_PAR","(");
                        insertArray(token_list,t);
                        i+=1;col+=1;
                    }
                }
                else if (buffer[i]=='*'){
                    if(buffer[i+1]==')'){
                        unbound_read = False;
                        i+=2;col+=2;
                    }
                    else{
                        initToken(&t,"MULT","*");
                        insertArray(token_list,t);
                        i+=1;col+=1;
                    }
                }
                else if (buffer[i]=='{'){
                    initToken(&t,"SLCOMMENT","");
                    insertArray(token_list,t);
                    unbound_read = True;
                    unbound_stop = '}';
                    i+=1;col+=1;
                }
                else if (buffer[i]=='}'){
                    if (unbound_read && unbound_stop =='}'){
                        unbound_read = False;
                        i+=1;col+=1;
                    }
                    else{
                        lineError("Lexer",curr_line,col,"} Invalid token");
                    }
                }
                /* New line */
                else if (buffer[i]=='\n'){
                    if (unbound_read && !strcmp(token_list->array[token_list->used-1].name,"MLCOMENT")){
                        lineError("Lexer",curr_line,col,"{comment} does not support multilines use (* *) instead");
                    }
                    else{
                        curr_line++;
                        col=1;//resets column count 
                        i+=1;
                    }  
                }
                else if (buffer[i]==' '){
                    //skip spaces
                    i++;col++;
                }
                /*literals*/
                //String literal
                else if (buffer[i]=='\"'){
                    if (!unbound_read){
                        initToken(&t,"STRING_LIT","");
                        insertArray(token_list,t);
                        unbound_read = True;
                        unbound_stop = '\"';
                        i+=1;col+=1;
                    }
                    else{
                        unbound_read = False;
                        i+=1;col+=1;
                    }
                }
                else if (buffer[i]=='\''){
                    if (!unbound_read){
                        // in pascal '' and "" have the same role
                        initToken(&t,"STRING_LIT","");
                        insertArray(token_list,t);
                        unbound_read = True;
                        unbound_stop = '\'';
                        i+=1;col+=1;
                    }
                    else{
                        unbound_read = False;
                        i+=1;col+=1;
                    }
                }
                // Numbers
                else if (isDigit(buffer[i])){//??
                    // max length of a number is 11
                    initToken(&t,"","");
                    //we can't know if it's a float or an int yet
                    //we insert the Token at the end of the treatement
                    //why not use unbound_read? 
                    //because the size of the token is actually limited
                    int death_counter = 11;
                    int survived = False;
                    while (death_counter>=0){
                        if(isDigit(buffer[i])||buffer[i]=='.'){
                            if(buffer[i]=='.')strcat(t.name,"FLOAT_LIT");
                            strncat(t.value,&buffer[i],1);
                            i+=1;col+=1;
                            death_counter--;
                        }
                        else{
                            //smth that isn't an integer|.
                            //if you can't get here before the death counter drops below 0 you get an error
                            //we skip this character anyways since it needs to be processed
                            if (strlen(t.name)==0)strcat(t.name,"INT_LIT");
                            survived=True;
                            break;
                        }
                    }
                    if (survived){
                        insertArray(token_list,t);
                        //as we said next character gets processed elsewhere
                    }
                    else{
                        lineError("Lexer",curr_line,col,"Number max length Exceeded (max = 11)");
                    }
                }
                //nested tokens
                else if (buffer[i]=='<'){
                    if (buffer[i+1]=='='){
                        initToken(&t,"LE","<=");
                        insertArray(token_list,t);
                        i+=2;col+=2;
                    }
                    else if (buffer[i+1]=='>'){
                        initToken(&t,"NT_EQ","<>");
                        insertArray(token_list,t);
                        i+=2;col+=2;
                    }
                    else{
                        initToken(&t,"LT","<");
                        insertArray(token_list,t);
                        i+=1;col+=1;
                    } " :i "
                }
                else if (buffer[i]=='>'){
                    if (buffer[i+1]=='='){
                        initToken(&t,"GE","<=");
                        insertArray(token_list,t);
                        i+=2;col+=2;
                    }
                    else{
                        initToken(&t,"GT",">");
                        insertArray(token_list,t);
                        i+=1;col+=1;
                    }
                }
                else if (buffer[i] == ':'){
                    if(buffer[i+1]=='='){
                        initToken(&t,"ASGN",":=");
                        insertArray(token_list,t);
                        i+=2;col+=2;
                        
                    }
                    else{
                        initToken(&t,"COLON",":");
                        insertArray(token_list,t);
                        i+=1;col+=1;
                    }
                }
                else if (buffer[i]=='+'){
                    if(buffer[i+1]=='='){
                        initToken(&t,"PLS_ASGN","+=");
                        insertArray(token_list,t);
                        i+=2;col+=2;
                        
                    }
                    else{
                        initToken(&t,"PLS","+");
                        insertArray(token_list,t);
                        i+=1;col+=1;
                    }
                }
                else if(buffer[i]=='-'){
                    if(buffer[i+1]=='='){
                        initToken(&t,"MNS_ASGN","-=");
                        insertArray(token_list,t);
                        i+=2;col+=2;
                    }
                    else{
                        initToken(&t,"MNS","-");
                        insertArray(token_list,t);
                        i+=1;col+=1;
                    }
                }
                else if(buffer[i]=='*'){
                    if(buffer[i+1]=='='){
                        initToken(&t,"MUL_ASGN","*=");
                        insertArray(token_list,t);
                        i+=2;col+=2;
                    }
                    else{
                        initToken(&t,"MUL","*");
                        insertArray(token_list,t);
                        i+=1;col+=1;
                    }
                }
                else if(buffer[i]=='/'){
                    if(buffer[i+1]=='='){
                        initToken(&t,"DIV_ASGN","/=");
                        insertArray(token_list,t);
                        i+=2;col+=2;
                    }
                    else{
                        initToken(&t,"DIV","/");
                        insertArray(token_list,t);
                        i+=1;col+=1;
                    }
                }
                else if(buffer[i]==';'){
                    initToken(&t,"SEMICOLON",";");
                    insertArray(token_list,t);
                    i+=1;col+=1;
                }
                //normal tokens
                //get normal token init's the token else it leaves it alone
                else{
                    int found = False;
                    for (int kk =0; kk<total_Ntokens;kk++){
                        int temp_len = strlen(normal_tokens[kk][1]);
                        if(getNormalToken(&t,normal_tokens[kk][0],normal_tokens[kk][1],buffer,i)){
                            insertArray(token_list,t);
                            i+=temp_len;col+=temp_len;
                            found = True;
                        }
                    }
                    if (!found){
                        //either it's an identifier of an invalid token
                        if (isWord(buffer[i])){
                            //first element found must be a word
                            initToken(&t,"IDENTIFER","");
                            int death_counter = 20;
                            int survived = False;
                            while (death_counter>=0){
                                if(isWord(buffer[i])||isDigit(buffer[i])){
                                    strncat(t.value,&buffer[i],1);
                                    i+=1;col+=1;death_counter--;
                                }
                                else {survived=True;break;}
                            }
                            if (survived){
                                insertArray(token_list,t);
                            }
                            else{
                                lineError("Lexer",curr_line,col,"identifiers in Pascal have a max length of 20");
                            }
                        }
                        else{
                            lineError("Lexer",curr_line,col,"Invalid Token");
                        }
                    }
                }
                if (strlen(t.name)!=0)
                printf("name: %s Value: %s\n",t.name,t.value);
            }
            else{
                // escaped characters cannot stop the unbound read 
                if (escaped) escaped = False;
                else if (buffer[i]=='\\') escaped = True;
                // must add a string to collect all the unbound data instead of m= length of string
                // writing to each character to the token_list (0(nlog(n)) au lieu de 0(n**m*log(n)))
                strncat(token_list->array[token_list->used-1].value,&buffer[i],1);
                i+=1;col+=1;
            }
        }
    }
    fclose(file_pt);
}
    
int main(){
    TokenArray tokens;
    RunLexer(&tokens,"pascal.pas");
    for (int i=0;i<tokens.used;i++) printf("name: %s Value: %s\n",tokens.array[i].name,tokens.array[i].value);
    return 0;
}

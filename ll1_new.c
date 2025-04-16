/**
 * 
 * LL(1) Parser Implementation in C
 * 
 */

 #include <stdio.h>
 #include <stdlib.h>
 #include <string.h>
 #include <ctype.h>
 /**
  * 
  * This program implements an LL(1) parser for a simple grammar.
  * The grammar is defined in the initialize_grammar function.
  * The program computes the FIRST and FOLLOW sets for the grammar,
  * builds the LL(1) parsing table, and then uses the parsing table to parse an input string.
  */
 #define MAX_STK_SIZE 100
 #define MAX_SYM_LEN 10
 #define MAX_PROD 10
 #define MAX_SYM 10
 #define MAX_RULES 10
 #define MAX_TOK 20
 /**
  * 
  * The following macros define the maximum sizes for various data structures used in the parser.
  * MAX_STK_SIZE: Maximum size of the stack used for parsing.
  * MAX_SYM_LEN: Maximum length of a symbol (terminal or non-terminal).
  * MAX_PROD: Maximum number of productions for a non-terminal.
  * MAX_SYM: Maximum number of symbols (terminals or non-terminals).
  * MAX_RULES: Maximum number of rules in the grammar.
  * MAX_TOK: Maximum number of tokens in the input string.
  * 
  */
 typedef struct {
     char left[MAX_SYM_LEN];
     char right[MAX_PROD][MAX_SYM][MAX_SYM_LEN];
     int num_productions;
 }Rules;
 typedef struct{
     char elem[MAX_STK_SIZE][MAX_SYM_LEN];
     int top;
 }stack;
 /**
  * * The Rules structure represents a production rule in the grammar.
  * Stack is used to store the rules and parse the string .
  *  
  */
 Rules grammar[MAX_RULES];
 int num_rules = 0;
 
 char terminals[MAX_SYM][MAX_SYM_LEN];
 int num_terminals = 0;
 
 
 char non_terminals[MAX_SYM][MAX_SYM_LEN];  
 int num_non_terminals = 0;
 /** store the terminal and non terminals as 2d arr. */
 
 /** Store first and follow sets. */
 char first_set[MAX_SYM][MAX_SYM_LEN][MAX_SYM_LEN];
 int first_count[MAX_SYM];
 
 char follow_set[MAX_SYM][MAX_SYM_LEN][MAX_SYM_LEN];
 int follow_count[MAX_SYM];
 
 
 char parse_table[MAX_SYM][MAX_SYM][MAX_SYM_LEN];
 /*
 *The parse table is a 3d array that stores Non Terminal, Terminal and The Prod Rules as dimensions.
 */
 void init_stack(stack *s) {
     s->top = -1;
 }
 /** Native stack functions defined for parsing. */
 void push(stack *s, char *elem) {
     if (s->top < MAX_STK_SIZE - 1) {
         s->top++;
         strcpy(s->elem[s->top], elem);
     } else {
         printf("Stack overflow\n");
     }
 }
 
 
 
 void pop(stack *s,char *elem){
     if (s->top >-1){
         strcpy(elem, s->elem[s->top]);
         s->top--;
     }else{
         elem[0] = '\0'; // Stack is empty
     }
 }
 
 void display_stack(stack *s) {
     printf("Stack: ");
     for (int i = s->top; i >= 0; i--) {
         printf("%s ", s->elem[i]);
     }
     printf("\n");
 }
 
 void tos(stack *s, char *elem) {
     if (s->top >-1) {
         strcpy(elem, s->elem[s->top]);
     } else {
         elem[0] = '\0'; // Stack is empty
     }
 }
 
 int check_empty(stack *s) {
     return s->top == -1;
 }

 
 
 
 int is_terminal(char *symbol) {
     if (strcmp(symbol, "ε") == 0) {
         return 0; 
     }
     for (int i=0;i<num_non_terminals;i++){
         if(strcmp(symbol,non_terminals[i])==0){
             return 0; // It's a non-terminal
         }
     }
     return 1;
 
 }
 
 /**
  * function to find the index of symbol in a array of symbol.return -1 id nor found.
  */
 int get_idx(char sym[MAX_SYM][MAX_SYM_LEN],int cnt,char *symbol) {
     for (int i = 0; i < cnt; i++) {
         if (strcmp(sym[i], symbol) == 0) {
             return i;
         }
     }
     return -1;
 }
 
 
 
 
 /** Function to find the first set of a non-terminal. */
 void first(){
     // flag is for the computing first set till we exhaust the production.or a var to find if there is change in the set.
     int flag;
     do {
         flag=0;
         //loop through all profuction rules.
         for (int i=0;i<num_rules;i++){
             int lhs= get_idx(non_terminals,num_non_terminals,grammar[i].left);
             for(int j=0;j<grammar[i].num_productions;++j){
                 // spl case if RHS is epsilon.
                 // so we just add to first set , if not already there set flag as 1
                 if (strcmp(grammar[i].right[j][0],"ε")==0){
                     //If production directly derive epsilon add to first set!
                     if (get_idx(first_set[lhs],first_count[lhs],"ε")==-1){
                         strcpy(first_set[lhs][first_count[lhs]++],"ε");
                         flag=1;
                     }
                     continue;// go to next prod rule
                 }
                 //Loop throgh symbols in RHS 
                 for(int k=0;;++k){
                     char* sym=grammar[i].right[j][k];
                     if(strlen(sym)==0){
                         break;
                     }
                     if(is_terminal(sym)){
                         // if terminal add to first set and break
                         if(get_idx(first_set[lhs],first_count[lhs],sym)==-1){
                             strcpy(first_set[lhs][first_count[lhs]++],sym);
                             flag=1;
                         }
                         break;
                     }else{
                         // if non-terminal add all its first elems.
                         int sym_idx=get_idx(non_terminals,num_non_terminals,sym);
                         int is_epsilon=0;
                         for (int m=0;m<first_count[sym_idx];++m){
                             if (strcmp(first_set[sym_idx][m],"ε")==0){
                                 is_epsilon=1;
                             }
                             else if(get_idx(first_set[lhs],first_count[lhs],first_set[sym_idx][m])==-1){
                                 strcpy(first_set[lhs][first_count[lhs]++],first_set[sym_idx][m]);
                                 flag=1;
 
                             }
                         }
                         // stop if the symbol cant derive epsilon.
                         if(!is_epsilon){
                             break;
                         }
                         if (grammar[i].right[j][k+1][0]=='\0'){
                             // if last symbol in production is non-terminal add epsilon to first set
                             if (get_idx(first_set[lhs],first_count[lhs],"ε")==-1){
                                 strcpy(first_set[lhs][first_count[lhs]++],"ε");
                                 flag=1;
                             }
                         }
 
                     }
                 }
 
             }
 
         }
 
     }while(flag);
 }
 
 void get_first(char res[MAX_SYM][MAX_SYM_LEN],int *res_cnt,char str[MAX_SYM][MAX_SYM_LEN]){
     * res_cnt=0;
     //IF EMPTY STRING RETURN ε
     if(str[0][0]=='\0'){
         strcpy(res[(*res_cnt)++],"ε");
         return;
     }
     // if first is epsilon add to result and return
     if(strcmp(str[0],"ε")==0){
         strcpy(res[(*res_cnt)++],"ε");
         return;
     }
     int i=0;
     int ep=1;
     //loop thru each symbol 
     while(str[i][0]!='\0'){
         ep=0;
         //symbol is a terminal
         if (is_terminal(str[i])){
             if(strcmp(str[i],"ε")!=0){
                 if(get_idx(res,*res_cnt,str[i])==-1){
                     strcpy(res[(*res_cnt)++],str[i]);
                 }
                 return;
             }else{
                 ep=1;
             }
             
 
         }else{
             // non terminal -> add all first except epsilon
             int ix=get_idx(non_terminals,num_non_terminals,str[i]);
             int is_ep=0;
             for (int j=0;j<first_count[ix];++j){
                 if(strcmp(first_set[ix][j],"ε")==0){
                     is_ep=1;
                 }else{
                     //add non epsilon term to res if not already present
                     if (get_idx(res,*res_cnt,first_set[ix][j])==-1){
                         strcpy(res[(*res_cnt)++],first_set[ix][j]);
                     }
                 }
             }
             //this is just to prevent adding epsilon to the result set if it is not needed.
             if (is_ep){
                 ep=1;
             }else{
                 return;
             }
         }
         i++; // move to next symbol
 
     }
     /** Add epsilon when all sym derive epsilon if not already added. */
     if (ep && get_idx(res,*res_cnt,"ε")==-1){
         strcpy(res[(*res_cnt)++],"ε");
     }
 
 }
 
 void follow(){
     for (int i=0;i<num_non_terminals;i++){
         follow_count[i]=0;
     }
     strcpy(follow_set[0][follow_count[0]++],"$"); // start symbol follow set has $
     int flag=0;
 
     do{
         flag=0;
         for (int i=0;i<num_rules;++i){
             int lhs=get_idx(non_terminals,num_non_terminals,grammar[i].left);
             for(int m=0;m<grammar[i].num_productions;++m){
 
                 for (int k=0;grammar[i].right[m][k][0]!='\0';++k){
                     if(!is_terminal(grammar[i].right[m][k])){
                         int b_idx=get_idx(non_terminals,num_non_terminals,grammar[i].right[m][k]);
                         // using the rule 2 A->aX(beta)
                         char beta[MAX_SYM][MAX_SYM_LEN];
                         int t=0;
                         for (;grammar[i].right[m][k+1+t][0]!='\0';++t){
                             strcpy(beta[t],grammar[i].right[m][k+1+t]);
 
                         }
                         beta[t][0]='\0'; // add null terminator to beta
 
                         // Find first of beta
                         char first_beta[MAX_SYM][MAX_SYM_LEN];
                         int first_beta_cnt=0;
                         get_first(first_beta,&first_beta_cnt,beta);
                         // Loop through first of beta and add to follow set of lhs if not already present
                         int is_epsilon=0;
                         for(int f=0;f<first_beta_cnt;++f){
                             if(strcmp(first_beta[f],"ε")==0){
                                 is_epsilon=1;
                             }else if(get_idx(follow_set[b_idx],follow_count[b_idx],first_beta[f])==-1){
                                 strcpy(follow_set[b_idx][follow_count[b_idx]++],first_beta[f]);
                                 flag=1;
                             }
                         
                         }
                         // refer notes for rule 1 and 2
                         // Rule 2 in notes A->aX(beta) if first of beta contains epsilon add follow of lhs to follow of b_idx
                         if(is_epsilon || beta[0][0]=='\0'){
                             for (int q=0;q<follow_count[lhs];++q){
                                 if (get_idx(follow_set[b_idx],follow_count[b_idx],follow_set[lhs][q])==-1){
                                     strcpy(follow_set[b_idx][follow_count[b_idx]++],follow_set[lhs][q]);
                                     flag=1;
                                 }
                             }
                         }
 
 
 
                     }
                 }
             }
 
 
 
 
 
         }
 
     }while(flag);
 
 
 
 
 
 
 
 }
 
 
 
 
 void build_parse_table(){
     //The table is a 3D array: first dimension indexes non-terminals; second indexes terminals; third is a character array large enough to store the production string.
     //The third dimension is used to store the production string for the corresponding non-terminal and terminal.
     //The first two dimensions are used to index the non-terminals and terminals respectively.
     for(int i =0;i<num_non_terminals;++i){
         for (int j=0;j<num_terminals;++j){
             parse_table[i][j][0]='\0'; // initialize parse table with empty strings
     }
 }
 
     for(int i =0;i<num_rules;++i){
         int lhs=get_idx(non_terminals,num_non_terminals,grammar[i].left);
         for(int j=0;j<grammar[i].num_productions;++j){
             char fr[MAX_SYM][MAX_SYM_LEN];
             int fr_cnt=0;
             get_first(fr,&fr_cnt,grammar[i].right[j]);
 
             //Print Production Rule
             printf("Production: %s -> ",grammar[i].left);
             for(int p=0;grammar[i].right[j][p][0]!='\0';++p){
                 printf("%s ",grammar[i].right[j][p]);
             }
             printf("\n");   
             // Print First Set
             printf("First Set: ");
             for (int k=0;k<fr_cnt;++k){
                 printf("%s ",fr[k]);
             }
             printf("\n");
             int is_epsilon=0;
             //for each terminal in first set add production to parse table
             for (int t=0;t<fr_cnt;++t){
                 if(strcmp(fr[t],"ε")==0){
                     is_epsilon=1;
                     continue;
                 }
                 int terminal_idx=get_idx(terminals,num_terminals,fr[t]);
                 if(parse_table[lhs][terminal_idx][0]!='\0'){
                     printf("Conflict in Parse table at [%s,%s]\n",non_terminals[lhs],terminals[terminal_idx]);
                 
                 }
                 sprintf(parse_table[lhs][terminal_idx],"%s ->",grammar[i].left);
                 for (int w=0;grammar[i].right[j][w][0]!='\0';++w){
                     strcat(parse_table[lhs][terminal_idx]," ");
                     strcat(parse_table[lhs][terminal_idx],grammar[i].right[j][w]);
                 }
 
             }
             //printf("has_epsilon: %d\n", is_epsilon);
 
             if(is_epsilon){
                 printf("Adding follow entry for %s\n",grammar[i].left);
                 for (int foll_idx=0;foll_idx<follow_count[lhs];++foll_idx){
                 char* foll_sym=follow_set[lhs][foll_idx];
                 printf("  FOLLOW symbol: %s\n", foll_sym);
                 int terminal_idx=get_idx(terminals,num_terminals,foll_sym);
                 printf("  term_idx: %d\n", terminal_idx);
                 if (terminal_idx!=-1){
                     if (parse_table[lhs][terminal_idx][0]!='\0'){
                         printf("Conflict in Parse table at [%s,%s]\n",non_terminals[lhs],terminals[terminal_idx]);
                     }
                     sprintf(parse_table[lhs][terminal_idx],"%s ->",grammar[i].left);
                     for (int w=0;grammar[i].right[j][w][0]!='\0';++w){
                         strcat(parse_table[lhs][terminal_idx],"");
                         strcat(parse_table[lhs][terminal_idx],grammar[i].right[j][w]);
                     }
                 
                 }       
             }
 
 
 
         }
     }
 
 
 }
     }
 
 
 
 void print_first_follow(){
     printf("First Sets:\n");
     for (int i=0;i<num_non_terminals;++i){
         printf("FIRST:(%s)={ ",non_terminals[i]);
         for (int j=0;j<first_count[i];++j){
             printf("%s ",first_set[i][j]);
         }
         printf("}\n");
     }
     printf("Follow Sets:\n");
     for (int i=0;i<num_non_terminals;++i){
         printf("FOLLOW(%s)={ ",non_terminals[i]);
         for (int j=0;j<follow_count[i];++j){
             printf("%s ",follow_set[i][j]);
         }
         printf("}\n");
     }
     printf("Parse Table:\n");   
 }
 void print_parse_table(){
     printf("Parse Table:\n");
     printf("               ");
     for(int i=0;i<num_terminals;++i){
         printf("%-12s ",terminals[i]);
     }
     printf("\n");
     for (int i=0;i<num_non_terminals;++i){
         printf("%-8s",non_terminals[i]);
         for(int j=0;j<num_terminals;++j){
             printf("%-12s ",parse_table[i][j][0]!='\0'?parse_table[i][j]:"-");
         }
         printf("\n");
     }
 }
 
 
 void initialize_grammar() {
     // Example grammar:
     // E -> T E'
     // E' -> + T E' | ε
     // T -> F T'
     // T' -> * F T' | ε
     // F -> ( E ) | id
     
     // Add non-terminals
     strcpy(non_terminals[num_non_terminals++], "E");
     strcpy(non_terminals[num_non_terminals++], "E'");
     strcpy(non_terminals[num_non_terminals++], "T");
     strcpy(non_terminals[num_non_terminals++], "T'");
     strcpy(non_terminals[num_non_terminals++], "F");
     
     // Add terminals
     strcpy(terminals[num_terminals++], "+");
     strcpy(terminals[num_terminals++], "*");
     strcpy(terminals[num_terminals++], "(");
     strcpy(terminals[num_terminals++], ")");
     strcpy(terminals[num_terminals++], "id");
     strcpy(terminals[num_terminals++], "$");  // End marker
     
     // Rule 1: E -> T E'
     strcpy(grammar[num_rules].left, "E");
     strcpy(grammar[num_rules].right[0][0], "T");
     strcpy(grammar[num_rules].right[0][1], "E'");
     grammar[num_rules].right[0][2][0] = '\0';
     grammar[num_rules].num_productions = 1;
     num_rules++;
     
     // Rule 2: E' -> + T E' | ε
     strcpy(grammar[num_rules].left, "E'");
     
     strcpy(grammar[num_rules].right[0][0], "+");
     strcpy(grammar[num_rules].right[0][1], "T");
     strcpy(grammar[num_rules].right[0][2], "E'");
     grammar[num_rules].right[0][3][0] = '\0';
     
     strcpy(grammar[num_rules].right[1][0], "ε");
     grammar[num_rules].right[1][1][0] = '\0';
     
     grammar[num_rules].num_productions = 2;
     num_rules++;
     
     // Rule 3: T -> F T'
     strcpy(grammar[num_rules].left, "T");
     strcpy(grammar[num_rules].right[0][0], "F");
     strcpy(grammar[num_rules].right[0][1], "T'");
     grammar[num_rules].right[0][2][0] = '\0';
     grammar[num_rules].num_productions = 1;
     num_rules++;
     
     // Rule 4: T' -> * F T' | ε
     strcpy(grammar[num_rules].left, "T'");
     
     strcpy(grammar[num_rules].right[0][0], "*");
     strcpy(grammar[num_rules].right[0][1], "F");
     strcpy(grammar[num_rules].right[0][2], "T'");
     grammar[num_rules].right[0][3][0] = '\0';
     
     strcpy(grammar[num_rules].right[1][0], "ε");
     grammar[num_rules].right[1][1][0] = '\0';
     
     grammar[num_rules].num_productions = 2;
     num_rules++;
     
     // Rule 5: F -> ( E ) | id
     strcpy(grammar[num_rules].left, "F");
     
     strcpy(grammar[num_rules].right[0][0], "(");
     strcpy(grammar[num_rules].right[0][1], "E");
     strcpy(grammar[num_rules].right[0][2], ")");
     grammar[num_rules].right[0][3][0] = '\0';
     
     strcpy(grammar[num_rules].right[1][0], "id");
     grammar[num_rules].right[1][1][0] = '\0';
     
     grammar[num_rules].num_productions = 2;
     num_rules++;
 }
 
 
 // tokenise an input string for parsing.
 int tokenize(char *input,char tokens[MAX_TOK][MAX_SYM_LEN]) {
     int count = 0;
     int i=0;
 
     while(input[i]!='\0' && count <MAX_TOK){
         // Skip whitespace
         while(isspace(input[i])) {
             i++;}
        if(input[i]=='\0'){
            break;
        }
         //Check other terminals;
         if (input[i]=='+' ||input[i]=='$'|| input[i]=='*' || input[i]=='(' || input[i]==')'){
             tokens[count][0]=input[i];
             tokens[count][1]='\0';
             count++;
             i++;
         }else if (isalnum(input[i])){
             strcpy(tokens[count],"id");
             count++;
             while(isalnum(input[i])){
                 i++;
             }   
 
 
     }
     else{
         printf("Invalid character: %c at %d\n", input[i],i);
         return -1;

     }
 }
     // Add end marker
     strcpy(tokens[count++], "$");
     return count;
 
 }
 
 int parse(char*ip){
     char tokens[MAX_TOK][MAX_SYM_LEN];
     int token_count=tokenize(ip,tokens);
     if (token_count<=0){
         printf("No tokens found\n");
         return 0;
     }
     stack s;
     init_stack(&s);
     push(&s,"$"); // push end marker to stack
     push(&s,non_terminals[0]);// start sym
     int tok_ix=0;
     printf("\nParsing: %s\n", ip);
     printf("Tokens: ");
     for (int i=0;i<token_count;++i){
         printf("%s ",tokens[i]);
     }
     printf("\n\n");
     printf("%-20s %-20s %s\n", "Stack", "Input", "Action");
     printf("%-20s %-20s %s\n", "--------------------", "--------------------", "--------------------");
 
     while(!check_empty(&s)){
         char stack_str[100]="";
         for(int i=0;i<=s.top;++i){
             strcat(stack_str,s.elem[i]);
             strcat(stack_str," ");
         }
         char input_str[100]="";
         for(int i=tok_ix;i<token_count;++i){
             strcat(input_str,tokens[i]);
             strcat(input_str," ");
         }   
         char top[MAX_SYM_LEN];
         tos(&s,top);
         //if tos match cur token
         if(strcmp(top,tokens[tok_ix])==0){
             printf("%-20s %-20s Match: %s\n", stack_str, input_str, top);
             pop(&s,top); // pop from stack
             tok_ix++; // move to next token
         }
         //if tos is non terminal
         else if(!is_terminal(top)){
             int non_term_idx=get_idx(non_terminals,num_non_terminals,top);
             int term_idx=get_idx(terminals,num_terminals,tokens[tok_ix]);
             if(non_term_idx <0 || term_idx<0){
                 printf("Error: Invalid non-terminal or terminal\n");
                 return 0;
             }
             if (parse_table[non_term_idx][term_idx][0]=='\0'){
                 printf("Error: No production for %s with %s\n",top,tokens[tok_ix]);
                 return 0;
             }
             // get the rhs of production.
             char prod[MAX_SYM_LEN*MAX_SYM_LEN];
             strcpy(prod,parse_table[non_term_idx][term_idx]);
             printf("%-20s %-20s Use: %s\n", stack_str, input_str, prod);
             pop(&s,top); // pop from stack non term 
             char rhs[MAX_SYM][MAX_SYM_LEN];
             int rhs_cnt=0;
             char* ptr=prod;
             // skip the arrow in the production string.
             while (*ptr != '\0' && (*ptr != '-' || *(ptr+1) != '>')) ptr++;
             if (*ptr == '\0') {
                 printf("Error: Invalid production format\n");
                 return 0;
             }
             ptr += 2; // Skip the "->" part
             while(*ptr!='\0'){
                 while(*ptr == ' ' || *ptr == '\t') ptr++; // Skip whitespace
                 char * end=ptr;
                 while(*end!=' ' && *end!='\0'){
                     end++;
                 }
                 int len=end-ptr;
                 if(len>0 && len<MAX_SYM_LEN){
                     strncpy(rhs[rhs_cnt],ptr,len);
                     rhs[rhs_cnt][len]='\0'; // null terminate the string
                     rhs_cnt++;
                 }
                 ptr=end;
             }
             for (int i = rhs_cnt - 1; i >= 0; i--) {
                 if (strcmp(rhs[i], "ε") != 0) {  // Don't push epsilon
                     push(&s, rhs[i]);
                 }
             }
 
 
         }else{
             // tos is term but i/p not matching
             printf("Error: Expected %s but found %s\n",top,tokens[tok_ix]);
             return 0;
         }
     }
     // check if all tokens are consumed.
     if (strcmp(tokens[tok_ix], "$") == 0) {
        printf("\nParsing successful! Input string is accepted by the grammar.\n");
        return 1;
    } else {
        printf("\nParsing error: Extra input after end of valid string.\n");
        return 0;
    } 
 
 
 }
 
 
 
 int main() {
     stack s;
     char input[MAX_STK_SIZE][MAX_SYM_LEN];
     char item[MAX_SYM_LEN];
     char res[MAX_SYM][MAX_SYM_LEN];
     int res_cnt=0;
     init_stack(&s);
     initialize_grammar();
     first();
     follow();
     build_parse_table();
     print_first_follow();
     print_parse_table();
    parse("id+id*id");

 
     return 0;
 }
 
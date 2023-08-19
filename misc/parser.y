%{
  #include <iostream>
  #include <vector>
  #include "../inc/assembler.h"
  #include "../inc/assembler_first.h"
  #include "../inc/assembler_second.h"
  
  using namespace std;
  extern int yylex();
  extern FILE *yyin;
  void yyerror(const char *s);
  extern char* yytext;
  extern int yylineno;
  Assembler* assembler;
%}

%union {
  int intValue;
  char* stringValue;
}
%token <intValue> NUM
%token <stringValue> SYMBOL
%token <stringValue> LABEL

%token <intValue> GPR
%token <intValue> CSR

%token GLOBAL
%token EXTERN
%token SECTION
%token WORD
%token SKIP
%token END

%token HALT
%token INT
%token IRET
%token CALL
%token RET
%token JMP
%token BEQ
%token BNE
%token BGT
%token PUSH
%token POP
%token XCHG
%token ADD
%token SUB
%token MUL 
%token DIV
%token NOT
%token AND
%token OR
%token XOR
%token SHL
%token SHR
%token LD
%token ST
%token CSRRD
%token CSRWR

%token EOL


%%
lines: line 
| lines line 

line:
   EOL               {yylineno++;}
| directive EOL      {yylineno++;}
| instruction EOL    {yylineno++;}
| LABEL EOL          {yylineno++; assembler->process_label($1);}
| END EOL            {yylineno++; assembler->process_end_dir();}//bez eol potencijalno 

directive:
  GLOBAL globalist {}
| EXTERN externlist {}
| SECTION SYMBOL          {assembler->process_section_dir($2);}
| WORD wordlist
| SKIP NUM                {assembler->process_skip_dir($2);}

externlist:
  SYMBOL                  {assembler->process_extern_dir($1);}
| externlist ',' SYMBOL   {assembler->process_extern_dir($3);}

globalist:  
  SYMBOL                  {assembler->process_global_dir($1);}
| globalist ',' SYMBOL    {assembler->process_global_dir($3);}


wordlist: 
  SYMBOL                  {assembler->process_word_dir($1);}
| NUM                     {assembler->process_word_dir($1);}
| wordlist ',' SYMBOL     {assembler->process_word_dir($3);}
| wordlist  ',' NUM       {assembler->process_word_dir($3);}

instruction:
  no_arg_ins              {assembler->process_instruction(1);}
| jmp_ins                 {assembler->process_instruction(1);}
| reg_ins                 {assembler->process_instruction(1);}
| data_ins                {assembler->process_instruction(1);}
| pseudo_ins_3            {assembler->process_instruction(3);}
| pseudo_ins_2            {assembler->process_instruction(2);}

pseudo_ins_3:             
IRET                      {assembler->process_IRET();}

pseudo_ins_2:
LD NUM ',' '%' GPR        {assembler->process_LD_MEM_DIR($2, $5);}
| LD SYMBOL ',' '%' GPR   {assembler->process_LD_MEM_DIR($2, $5);}

no_arg_ins:
  HALT                     {assembler->process_HALT();}
| INT                      {assembler->process_INT();}
| RET                      {assembler->process_RET();}

jmp_ins:
  call_ins
| jmp_ins_
| beq_ins
| bne_ins
| bgt_ins

call_ins:
  CALL NUM      {assembler->process_CALL($2);}          
| CALL SYMBOL   {assembler->process_CALL($2);} 

jmp_ins_:
  JMP NUM       {assembler->process_JMP($2);} 
| JMP SYMBOL    {assembler->process_JMP($2);} 


beq_ins:
  BEQ '%' GPR ',' '%' GPR ',' NUM      {assembler->process_BEQ($3, $6, $8);}
| BEQ '%' GPR ',' '%' GPR ',' SYMBOL   {assembler->process_BEQ($3, $6, $8);}

bne_ins:
  BNE '%' GPR ',' '%' GPR ',' NUM      {assembler->process_BNE($3, $6, $8);}
| BNE '%' GPR ',' '%' GPR ',' SYMBOL   {assembler->process_BNE($3, $6, $8);}

bgt_ins:
  BGT '%' GPR ',' '%' GPR ',' NUM      {assembler->process_BGT($3, $6, $8);}
| BGT '%' GPR ',' '%' GPR ',' SYMBOL   {assembler->process_BGT($3, $6, $8);}

reg_ins:
  PUSH '%' GPR                  {assembler->process_PUSH($3);}
| POP '%' GPR                   {assembler->process_POP($3);}
| XCHG '%' GPR ',' '%' GPR      {assembler->process_XCHG($3, $6);}
| ADD '%' GPR ',' '%' GPR       {assembler->process_ADD($3, $6);}
| SUB '%' GPR ',' '%' GPR       {assembler->process_SUB($3, $6);}
| MUL '%' GPR ',' '%' GPR       {assembler->process_MUL($3, $6);}
| DIV '%' GPR ',' '%' GPR       {assembler->process_DIV($3, $6);}
| NOT '%' GPR                   {assembler->process_NOT($3);}
| AND '%' GPR ',' '%' GPR       {assembler->process_AND($3, $6);}
| OR '%' GPR ',' '%' GPR        {assembler->process_OR($3, $6);}
| XOR '%' GPR ',' '%' GPR       {assembler->process_XOR($3, $6);}
| SHL '%' GPR ',' '%' GPR       {assembler->process_SHL($3, $6);}
| SHR '%' GPR ',' '%' GPR       {assembler->process_SHR($3, $6);}
| CSRRD CSR ',' '%' GPR         {assembler->process_CSRRD($2, $5);}
| CSRWR '%' GPR ',' CSR         {assembler->process_CSRWR($3, $5);}

data_ins:
  ld_ins
| st_ins

ld_ins:
  LD '$' NUM ',' '%' GPR                      {assembler->process_LD_IMM($3, $6);}
| LD '$' SYMBOL ',' '%' GPR                   {assembler->process_LD_IMM($3, $6);}
| LD '%' GPR ',' '%' GPR                      {assembler->process_LD_REG_DIR($3, $6);}
| LD '[' '%' GPR ']' ',' '%' GPR              {assembler->process_LD_REG_IND($4, $8);}
| LD '[' '%' GPR '+' NUM ']' ',' '%' GPR      {assembler->process_LD_REG_IND_OFF($4, $6, $10);}
| LD '[' '%' GPR '+' SYMBOL ']' ',' '%' GPR   {cout << "CANNOT BE SYMBOL REG_IND_OFF" << endl; exit(-1);}

st_ins:
  ST '%' GPR ',' '$' NUM                      {cout << "CANNOT BE STORE WITH IMMEDIATE"; exit(-1);}
| ST '%' GPR ',' '$' SYMBOL                   {cout << "CANNOT BE STORE WITH IMMEDIATE"; exit(-1);}
| ST '%' GPR ',' NUM                          {assembler->process_ST_MEM_DIR($3, $5);}
| ST '%' GPR ',' SYMBOL                       {assembler->process_ST_MEM_DIR($3, $5);}
| ST '%' GPR ',' '%' GPR                      {cout << "NOT POSSIBLE REG DIR INTO REG"; exit(-1);}
| ST '%' GPR ',' '[' '%' GPR ']'              {assembler->process_ST_REG_IND($3, $7);}
| ST '%' GPR ',' '[' '%' GPR '+' NUM ']'      {assembler->process_ST_REG_IND_OFF($3, $7, $9);}
| ST '%' GPR ',' '[' '%' GPR '+' SYMBOL ']'   {cout << "CANNOT BE SYMBOL REG_IND_OFF" << endl; exit(-1);}


%%

void yyerror(const char* msg) {
    std::cerr << "Syntax error: " << msg << " at token '" << yytext << "'" << std::endl;
    std::cerr << "Syntax error at line " << yylineno << ": " << msg << std::endl;
}

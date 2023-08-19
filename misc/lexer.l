%{
#include <iostream>
#include "parser.tab.h"
#include "../inc/assembler_first.h"
using namespace std;
%}

%option noyywrap

%%
.global                   {return GLOBAL;}
.extern                   {return EXTERN;}
.section                  {return SECTION;}
.word                     {return WORD;}
.skip                     {return SKIP;}
.end                      {return END;}

halt                      {return HALT;}
int                       {return INT;}
iret                      {return IRET;}
call                      {return CALL;}
ret                       {return RET;}
jmp                       {return JMP;}
beq                       {return BEQ;}
bne                       {return BNE;}
bgt                       {return BGT;}
push                      {return PUSH;}
pop                       {return POP;}
xchg                      {return XCHG;}
add                       {return ADD;}
sub                       {return SUB;}
mul                       {return MUL;}
div                       {return DIV;}
not                       {return NOT;}
and                       {return AND;}
or                        {return OR;}
xor                       {return XOR;}
shl                       {return SHL;}
shr                       {return SHR;}
ld                        {return LD;}
st                        {return ST;}
csrrd                     {return CSRRD;}
csrwr                     {return CSRWR;}

[$]                       {return '$';}
0x[0-9A-Fa-f]+            {sscanf(yytext,"%x",&yylval.intValue); return NUM;}
[-]?[0-9]+                {yylval.intValue = atoi(yytext); return NUM;}
r[0-9][0-5]?              {yylval.intValue = atoi(yytext+1); return GPR;}
sp                        {yylval.intValue = 14; return GPR;}
pc                        {yylval.intValue = 15; return GPR;}
%status                   {yylval.intValue = 0; return CSR;}
%handler                  {yylval.intValue = 1; return CSR;}
%cause                    {yylval.intValue = 2; return CSR;}

[a-zA-Z_][a-zA-Z0-9_]*[:] {yylval.stringValue = strdup(yytext); return LABEL;} // redosled
[a-zA-Z_][a-zA-Z_0-9]*    {yylval.stringValue = strdup(yytext); return SYMBOL;} // 

\n                        {return EOL;}

"["                       {return '[';}
"]"                       {return ']';}  
[%]                       {return '%';}
[+]                       {return '+';}
[*]                       {return '*';}
[:]                       {return ':';}
[,]                       {return ',';}
[#][^\n]*
[ \t]
. {std::cout<<"WRONG TOKEN"<<yytext<<std::endl;exit(-1);}
%%
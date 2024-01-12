%option noyywrap
%option yylineno
%{
#include <iostream>
#include <vector>
#include <memory>
#include <cstring>

#include "ast_nodes.hpp"
#include "semantic_analyser.hpp"
#include "code_generator.hpp"
#include "parser.tab.h"


const std::string error_alert = "\x1b[31merror\x1b[0m: ";

%}

IDENTIFIER              [_a-z]
DIGIT                   [0-9]
PUNCT                   [\(\)\:\;\[\],]

%x COMMENT

%%
"PROCEDURE"             { return PROCEDURE; }
"IS"                    { return IS;}
"IN"                    { return IN; }
"END"                   { return END; }
"PROGRAM"               { return PROGRAM; }

"READ"                  { return READ; }
"WRITE"                 { return WRITE; }

"IF"                    { return IF; }
"THEN"                  { return THEN; }
"ELSE"                  { return ELSE; }
"ENDIF"                 { return ENDIF; }

"WHILE"                 { return WHILE; }
"DO"                    { return DO; }
"ENDWHILE"              { return ENDWHILE; }

"REPEAT"                { return REPEAT; }
"UNTIL"                 { return UNTIL; }

"T"                     { return T; }

":="                    { return ASSIGN; }

"="                     { return EQ; }
"!="                    { return NEQ; }
"<"                     { return LT; }
">"                     { return GT; }
"<="                    { return LEQ; }
">="                    { return GEQ; }

"+"                     { return ADD; }
"-"                     { return SUB; }
"/"                     { return DIV; }
"%"                     { return MOD; }
"*"                     { return MUL; }
{IDENTIFIER}+           { 
                            yylval.pidentifier = new std::string(yytext);
                            return pidentifier;
                        }
{DIGIT}+                { yylval.num = std::stoll(yytext); return num; }
{PUNCT}                 { return yytext[0]; }

"#"                     { BEGIN(COMMENT); }
<COMMENT>\n             { BEGIN(INITIAL); }
<COMMENT>.              { }

[ \t\r\n]+              { /*ignore whitespaces*/ }
.                       { return ERROR; }
%%
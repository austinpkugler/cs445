%{
// Based on CS445 - Calculator Example Program by Robert Heckendorn
// The order of includes is mandatory
#include "TokenData.hpp"
#include "Flags/Flags.hpp"
#include "Semantics/Semantics.hpp"
#include "Semantics/SymTable.hpp"
#include "SyntaxError/SyntaxError.hpp"
#include "Tree/Tree.hpp"

#include <sstream>
#include <string>
#include <iostream>

#include "c-.tab.h"

int lineCount = 1;
char *lastToken;

static int setValue(int lineNum, int tokenClass, char *sValue)
{
    // Create the pass-back data space
    yylval.tokenData = new TokenData;

    // Fill it up
    yylval.tokenData->lineNum = lineNum;
    yylval.tokenData->tokenContent = std::string(sValue);

    lastToken = sValue;

    if (tokenClass == 260)
    {
        std::string chars = Const::removeFirstAndLastChar(lastToken);
        if (chars.length() > 1 && chars[0] != '\\')
        {
            std::stringstream msg;
            msg << "character is " << chars.length() << " characters long and not a single character: '" << lastToken << "'.  The first char will be used.";
            Emit::warn(lineCount, msg.str());
        }
    }

    return tokenClass;
}

%}

%option noyywrap

%%

\/\/.*\n                    { lineCount++; }                 // Ignore comments
[ \t]                       { }                 // Ignore whitespace
"\n"                        { lineCount++; }    // Newlines

"int"                       { return INT; }
"bool"                      { return BOOL; }
"char"                      { return CHAR; }
"static"                    { return setValue(lineCount, STATIC, yytext); }

"if"                        { return setValue(lineCount, IF, yytext); }
"then"                      { return setValue(lineCount, THEN, yytext); }
"else"                      { return setValue(lineCount, ELSE, yytext); }
"while"                     { return setValue(lineCount, WHILE, yytext); }
"for"                       { return setValue(lineCount, FOR, yytext); }
"to"                        { return setValue(lineCount, TO, yytext); }
"by"                        { return setValue(lineCount, BY, yytext); }
"do"                        { return setValue(lineCount, DO, yytext); }

"return"                    { return setValue(lineCount, RETURN, yytext); }
"break"                     { return setValue(lineCount, BREAK, yytext); }

"and"                       { return setValue(lineCount, AND, yytext); }
"or"                        { return setValue(lineCount, OR, yytext); }
"not"                       { return setValue(lineCount, NOT, yytext); }

"{"                         { return setValue(lineCount, LCURLY, yytext); }
"}"                         { return setValue(lineCount, RCURLY, yytext); }
"("                         { return setValue(lineCount, LPAREN, yytext); }
")"                         { return setValue(lineCount, RPAREN, yytext); }
"["                         { return setValue(lineCount, LBRACK, yytext); }
"]"                         { return setValue(lineCount, RBRACK, yytext); }

"*"                         { return setValue(lineCount, MUL, yytext); }
"/"                         { return setValue(lineCount, DIV, yytext); }
"%"                         { return setValue(lineCount, MOD, yytext); }
"+"                         { return setValue(lineCount, ADD, yytext); }
"-"                         { return setValue(lineCount, SUB, yytext); }
"++"                        { return setValue(lineCount, INC, yytext); }
"--"                        { return setValue(lineCount, DEC, yytext); }

"+="                        { return setValue(lineCount, ADDASGN, yytext); }
"-="                        { return setValue(lineCount, SUBASGN, yytext); }
"*="                        { return setValue(lineCount, MULASGN, yytext); }
"/="                        { return setValue(lineCount, DIVASGN, yytext); }
"="                         { return setValue(lineCount, ASGN, yytext); }

"=="                        { return setValue(lineCount, EQ, yytext); }
"!="                        { return setValue(lineCount, NEQ, yytext); }
"<"                         { return setValue(lineCount, LT, yytext); }
"<="                        { return setValue(lineCount, LEQ, yytext); }
">"                         { return setValue(lineCount, GT, yytext); }
">="                        { return setValue(lineCount, GEQ, yytext); }

";"                         { return setValue(lineCount, SEMICOLON, yytext); }
":"                         { return setValue(lineCount, COLON, yytext); }
","                         { return setValue(lineCount, COMMA, yytext); }
"?"                         { return setValue(lineCount, QUESTION, yytext); }

"true"|"false"              { return setValue(lineCount, BOOLCONST, yytext); }

"''" { Emit::error(lineCount, "Empty character ''.  Characters ignored."); }

[a-zA-Z][a-zA-Z0-9]*        { return setValue(lineCount, ID, yytext); }             // Variable names (the id)
[0-9]+                      { return setValue(lineCount, NUMCONST, yytext); }       // Number literals
\"([^\\"\n]|\\.)*\"         { return setValue(lineCount, STRINGCONST, yytext); }    // String literals
\'((\\\')|([^\n\']))*\'     { return setValue(lineCount, CHARCONST, yytext); }      // Char literals

. {
    std::stringstream msg;
    msg << "Invalid or misplaced input character: '" << yytext[0] << "'. Character Ignored.";
    Emit::error(lineCount, msg.str());
}

%%

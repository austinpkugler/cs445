%{
// Based off CS445 - Calculator Example Program by Robert Heckendorn
#include "TokenData.hpp"

#include <iostream>
#include <string>
#include <stdio.h>

// From yacc
extern int yylex();
extern int yydebug;
extern FILE *yyin;

// From c-.l scanner
extern int lineCount;
extern int errorCount;

#define YYERROR_VERBOSE
void yyerror(const char *msg)
{
    std::cout << "ERROR(" << lineCount + 1 << "): " << msg << std::endl;
    errorCount++;
}

%}

%union {
    TokenData *tokenData;
}

%token <tokenData> NUMCONST BOOLCONST CHARCONST STRINGCONST KEYWORD ID TOKEN

%%

statementList   : statementList statement
                | statement
                ;

statement       : '\n'
                | NUMCONST      { std::cout << "Line " << $1->tokenLineNum << " Token: NUMCONST Value: " << $1->numValue << "  Input: " << $1->tokenContent << std::endl; }
                | BOOLCONST     { std::cout << "Line " << $1->tokenLineNum << " Token: BOOLCONST Value: " << $1->numValue << "  Input: " << $1->tokenContent << std::endl; }
                | CHARCONST     {
                                    if ($1->charLengthWarning)
                                    {
                                        std::cout << "WARNING(" << $1->tokenLineNum << "): character is " << $1->tokenContent.length() - 2 << " characters long and not a single character: '" << $1->tokenContent << "'.  The first char will be used.\n";
                                    }
                                    std::cout << "Line " << $1->tokenLineNum << " Token: CHARCONST Value: '" << $1->charValue << "'  Input: " << $1->tokenContent << std::endl;
                                }
                | STRINGCONST   { std::cout << "Line " << $1->tokenLineNum << " Token: STRINGCONST Value: \"" << $1->stringValue << "\"  Len: " << $1->stringValue.length() << "  Input: " << $1->tokenContent << std::endl; }
                | KEYWORD       { std::cout << "Line " << $1->tokenLineNum << " Token: " << $1->stringValue << std::endl; }
                | ID            { std::cout << "Line " << $1->tokenLineNum << " Token: ID Value: " << $1->tokenContent << std::endl; }
                | TOKEN         { std::cout << "Line " << $1->tokenLineNum << " Token: " << $1->stringValue << std::endl; }
                ;

%%

int main(int argc, char *argv[])
{
    if (argc > 1)
    {
        if (!(yyin = fopen(argv[1], "r")))
        {
            // Failed to open file
            printf("ERROR: failed to open \'%s\'\n", argv[1]);
            exit(1);
        }
    }

    yyparse();
    return 0;
}

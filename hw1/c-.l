%{
// Based off CS445 - Calculator Example Program by Robert Heckendorn
// The order of includes is mandatory
#include "TokenData.hpp"
#include "c-.tab.h"
#include "Util.hpp"

#include <string>

int lineCount = 1;
int errorCount;

static int setValue(int tokenLineNum, int tokenClass, char *sValue)
{
    // Create the pass-back data space
    yylval.tokenData = new TokenData;

    // Fill it up
    yylval.tokenData->tokenLineNum = tokenLineNum;
    yylval.tokenData->tokenContent = strdup(sValue);

    switch (tokenClass)
    {
        case NUMCONST:
            yylval.tokenData->numValue = std::stoi(yylval.tokenData->tokenContent);
            break;
        case BOOLCONST:
            yylval.tokenData->numValue = yylval.tokenData->tokenContent == "true";
            break;
        case CHARCONST:
        {
            std::string chars = Util::removeFirstAndLastChar(yylval.tokenData->tokenContent);
            yylval.tokenData->charValue = Util::parseFirstChar(chars);
            if (chars.length() > 1 && chars[0] != '\\')
            {
                yylval.tokenData->charLengthWarning = true;
            }
            break;
        }
        case STRINGCONST:
            yylval.tokenData->stringValue = Util::parseChars(Util::removeFirstAndLastChar(yylval.tokenData->tokenContent));
            break;
        case KEYWORD:
            yylval.tokenData->stringValue = Util::toUpper(yylval.tokenData->tokenContent);
            break;
        case ID:
            break;
        case TOKEN:
            if (yylval.tokenData->tokenContent == "+=")
            {
                yylval.tokenData->stringValue = "ADDASS";
            }
            else if (yylval.tokenData->tokenContent == "==")
            {
                yylval.tokenData->stringValue = "EQ";
            }
            else if (yylval.tokenData->tokenContent == "!=")
            {
                yylval.tokenData->stringValue = "NEQ";
            }
            else if (yylval.tokenData->tokenContent == ">=")
            {
                yylval.tokenData->stringValue = "GEQ";
            }
            else if (yylval.tokenData->tokenContent == "<=")
            {
                yylval.tokenData->stringValue = "LEQ";
            }
            else if (yylval.tokenData->tokenContent == "++")
            {
                yylval.tokenData->stringValue = "INC";
            }
            else if (yylval.tokenData->tokenContent == "--")
            {
                yylval.tokenData->stringValue = "DEC";
            }
            else
            {
                yylval.tokenData->stringValue = yylval.tokenData->tokenContent;
            }
            break;
    }

    return tokenClass;
}

%}

%option noyywrap

%%

true|false                      { return setValue(lineCount, BOOLCONST, yytext); }
int|bool|char|static            { return setValue(lineCount, KEYWORD, yytext); }
begin|end|if|then|else          { return setValue(lineCount, KEYWORD, yytext); }
while|for|break                 { return setValue(lineCount, KEYWORD, yytext); }
return|to|by|do                 { return setValue(lineCount, KEYWORD, yytext); }
and|or|not                      { return setValue(lineCount, KEYWORD, yytext); }

\/\/.*\n                        { lineCount++; }
\"((\\\")|([^\n\"]))*\"         { return setValue(lineCount, STRINGCONST, yytext); }
\'((\\\')|([^\n\']))*\'         { return setValue(lineCount, CHARCONST, yytext); }
[a-zA-Z][a-zA-Z0-9]*            { return setValue(lineCount, ID, yytext); }
[0-9]+                          { return setValue(lineCount, NUMCONST, yytext); }
\(|\)|\{|\}|\[|\]               { return setValue(lineCount, TOKEN, yytext); }
\;|\:|\,                        { return setValue(lineCount, TOKEN, yytext); }
\?|\*|\/|\%|\-|\+               { return setValue(lineCount, TOKEN, yytext); }
\/\=|\+\=|\-\=|\*\=|\+\+|\-\-   { return setValue(lineCount, TOKEN, yytext); }
\<|\<\=|\>|\>\=|\=\=|\=|\!\=    { return setValue(lineCount, TOKEN, yytext); }
\n                              { lineCount++; return setValue(lineCount, yytext[0], yytext ); }
[ \t]                           {}

. {
    printf("ERROR(%d): Invalid or misplaced input character: \'%c\'. Character Ignored.\n", lineCount, yytext[0]);
    errorCount++;
}

%%

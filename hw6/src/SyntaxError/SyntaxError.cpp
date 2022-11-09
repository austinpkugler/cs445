// Based on yyerror.cpp by Michael Wilder (see materials directory)
#include "SyntaxError.hpp"

static std::map<std::string , char *> niceTokenNameMap;

int SyntaxError::split(char *s, char *strs[], char breakchar)
{
    int num;

    strs[0] = s;
    num = 1;
    for (char *p = s; *p; p++)
    {
        if (*p == breakchar)
        {
            strs[num++] = p + 1;
            *p = '\0';
        }
    }
    strs[num] = NULL;

    return num;
}

void SyntaxError::trim(char *s)
{
    s[strlen(s)-1] = '\0';
}

void SyntaxError::initErrorProcessing()
{
    niceTokenNameMap["ADDASGN"] = (char *)"\"+=\"";
    niceTokenNameMap["AND"] = (char *)"\"and\"";
    niceTokenNameMap["ASGN"] = (char *)"'='";
    niceTokenNameMap["Begin"] = (char *)"\"begin\"";
    niceTokenNameMap["BOOL"] = (char *)"\"bool\"";
    niceTokenNameMap["BOOLCONST"] = (char *)"Boolean constant";
    niceTokenNameMap["BREAK"] = (char *)"\"break\"";
    niceTokenNameMap["BY"] = (char *)"\"by\"";
    niceTokenNameMap["CHAR"] = (char *)"\"char\"";
    niceTokenNameMap["CHARCONST"] = (char *)"character constant";
    niceTokenNameMap["CHSIGN"] = (char *)"-";
    niceTokenNameMap["DEC"] = (char *)"\"--\"";
    niceTokenNameMap["DIVASGN"] = (char *)"\"/=\"";
    niceTokenNameMap["DO"] = (char *)"\"do\"";
    niceTokenNameMap["ELSE"] = (char *)"\"else\"";
    niceTokenNameMap["End"] = (char *)"\"end\"";
    niceTokenNameMap["FOR"] = (char *)"\"for\"";
    niceTokenNameMap["GEQ"] = (char *)"\">=\"";
    niceTokenNameMap["ID"] = (char *)"identifier";
    niceTokenNameMap["IF"] = (char *)"\"if\"";
    niceTokenNameMap["INC"] = (char *)"\"++\"";
    niceTokenNameMap["INT"] = (char *)"\"int\"";
    niceTokenNameMap["LEQ"] = (char *)"\"<=\"";
    niceTokenNameMap["MULASGN"] = (char *)"\"*=\"";
    niceTokenNameMap["NEQ"] = (char *)"\"!=\"";
    niceTokenNameMap["NOT"] = (char *)"\"not\"";
    niceTokenNameMap["NUMCONST"] = (char *)"numeric constant";
    niceTokenNameMap["OR"] = (char *)"\"or\"";
    niceTokenNameMap["RETURN"] = (char *)"\"return\"";
    niceTokenNameMap["SIZEOF"] = (char *)"\"*\"";
    niceTokenNameMap["STATIC"] = (char *)"\"static\"";
    niceTokenNameMap["STRINGCONST"] = (char *)"string constant";
    niceTokenNameMap["SUBASGN"] = (char *)"\"-=\"";
    niceTokenNameMap["THEN"] = (char *)"\"then\"";
    niceTokenNameMap["TO"] = (char *)"\"to\"";
    niceTokenNameMap["WHILE"] = (char *)"\"while\"";
    niceTokenNameMap["LPAREN"] = (char *)"'('";
    niceTokenNameMap["RPAREN"] = (char *)"')'";
    niceTokenNameMap["MOD"] = (char *)"'%'";
    niceTokenNameMap["LCURLY"] = (char *)"'{'";
    niceTokenNameMap["RCURLY"] = (char *)"'}'";
    niceTokenNameMap["LBRACK"] = (char *)"'['";
    niceTokenNameMap["RBRACK"] = (char *)"']'";
    niceTokenNameMap["COLON"] = (char *)"':'";
    niceTokenNameMap["SEMICOLON"] = (char *)"';'";
    niceTokenNameMap["COMMA"] = (char *)"','";
    niceTokenNameMap["GT"] = (char *)"'>'";
    niceTokenNameMap["LT"] = (char *)"'<'";
    niceTokenNameMap["EQ"] = (char *)"\"==\"";
    niceTokenNameMap["MUL"] = (char *)"'*'";
    niceTokenNameMap["DIV"] = (char *)"'/'";
    niceTokenNameMap["ADD"] = (char *)"'+'";
    niceTokenNameMap["SUB"] = (char *)"'-'";
    niceTokenNameMap["$end"] = (char *)"end of input";
}

char * SyntaxError::niceTokenStr(char *tokenName)
{
    if (tokenName[0] == '\'')
    {
        return tokenName;
    }
    if (niceTokenNameMap.find(tokenName) == niceTokenNameMap.end())
    {
        printf("ERROR(SYSTEM): niceTokenStr fails to find string '%s'\n", tokenName);
        fflush(stdout);
        exit(1);
    }
    return niceTokenNameMap[tokenName];
}

bool SyntaxError::elaborate(char *s)
{
    return (strstr(s, "constant") || strstr(s, "identifier"));
}

void SyntaxError::tinySort(char *base[], int num, int step, bool up)
{
    for (int i = step; i < num; i += step)
    {
        for (int j = 0; j < i; j += step)
        {
            if (up ^ (strcmp(base[i], base[j]) > 0))
            {
                char *tmp;
                tmp = base[i]; base[i] = base[j]; base[j] = tmp;
            }
        }
    }
}

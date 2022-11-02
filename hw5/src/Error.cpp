// Based on yyerror.cpp by Michael Wilder (see materials directory)
#include "Error.hpp"

static std::map<std::string , char *> niceTokenNameMap;

int Error::split(char *s, char *strs[], char breakchar)
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

void Error::trim(char *s)
{
    s[strlen(s)-1] = '\0';
}

void Error::initErrorProcessing()
{
    niceTokenNameMap["ADDASS"] = (char *)"\"+=\"";
    niceTokenNameMap["AND"] = (char *)"\"and\"";
    niceTokenNameMap["ASGN"] = (char *)"\"<-\"";
    niceTokenNameMap["Begin"] = (char *)"\"begin\"";
    niceTokenNameMap["BOOL"] = (char *)"\"bool\"";
    niceTokenNameMap["BOOLCONST"] = (char *)"Boolean constant";
    niceTokenNameMap["BREAK"] = (char *)"\"break\"";
    niceTokenNameMap["BY"] = (char *)"\"by\"";
    niceTokenNameMap["CHAR"] = (char *)"\"char\"";
    niceTokenNameMap["CHARCONST"] = (char *)"character constant";
    niceTokenNameMap["CHSIGN"] = (char *)"-";
    niceTokenNameMap["DEC"] = (char *)"\"--\"";
    niceTokenNameMap["DIVASS"] = (char *)"\"/=\"";
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
    niceTokenNameMap["MULASS"] = (char *)"\"*=\"";
    niceTokenNameMap["NEQ"] = (char *)"\"!=\"";
    niceTokenNameMap["NOT"] = (char *)"\"not\"";
    niceTokenNameMap["NUMCONST"] = (char *)"numeric constant";
    niceTokenNameMap["OR"] = (char *)"\"or\"";
    niceTokenNameMap["RETURN"] = (char *)"\"return\"";
    niceTokenNameMap["SIZEOF"] = (char *)"\"*\"";
    niceTokenNameMap["STATIC"] = (char *)"\"static\"";
    niceTokenNameMap["STRINGCONST"] = (char *)"string constant";
    niceTokenNameMap["SUBASS"] = (char *)"\"-=\"";
    niceTokenNameMap["THEN"] = (char *)"\"then\"";
    niceTokenNameMap["TO"] = (char *)"\"to\"";
    niceTokenNameMap["WHILE"] = (char *)"\"while\"";
    niceTokenNameMap["$end"] = (char *)"end of input";
}

char * Error::niceTokenStr(char *tokenName)
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

bool Error::elaborate(char *s)
{
    return (strstr(s, "constant") || strstr(s, "identifier"));
}

void Error::tinySort(char *base[], int num, int step, bool up)
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

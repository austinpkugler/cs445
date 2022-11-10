// Based on yyerror.h by Michael Wilder (see materials directory)
#pragma once

#include <map>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <string>

class SyntaxError
{
    public:
        static void initErrorProcessing();
        static int split(char *s, char *strs[], char breakchar);
        static void trim(char *s);
        static char * niceTokenStr(char *tokenName);
        static bool elaborate(char *s);
        static void tinySort(char *base[], int num, int step, bool up);
        static bool getHasError() { return m_hasError; }
        static void setHasError(const bool hasError) { m_hasError = hasError; }

    private:
        inline static bool m_hasError = false;
};

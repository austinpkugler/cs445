%{
// Based off CS445 - Calculator Example Program by Robert Heckendorn
#include "Flags.hpp"
#include "TokenData.hpp"
#include "Tree/Tree.hpp"

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

// AST
Node *root;

#define YYERROR_VERBOSE
void yyerror(const char *msg)
{
    std::cout << "ERROR(" << lineCount + 1 << "): " << msg << std::endl;
    errorCount++;
}

%}

%union {
    Primitive::Type *type;
    TokenData *tokenData;
    Node *node;
}

%token <tokenData> NUMCONST BOOLCONST CHARCONST STRINGCONST ID
%token <tokenData> INT BOOL CHAR STATIC
%token <tokenData> ASGN ADDASGN SUBASGN MULASGN DIVASGN
%token <tokenData> IF THEN ELSE WHILE FOR TO BY DO
%token <tokenData> COLON SEMICOLON COMMA
%token <tokenData> RETURN BREAK
%token <tokenData> AND OR NOT
%token <tokenData> ADD SUB MUL DIV MOD INC DEC QUESTION
%token <tokenData> RPAREN LPAREN RBRACK LBRACK LCURLY RCURLY
%token <tokenData> EQ NEQ LT LEQ GT GEQ

%type <node> program declList decl varDecl scopedVarDecl varDeclList varDeclInit
%type <node> varDeclId funDecl parms parmList parmTypeList parmIdList parmId stmt
%type <node> stmtUnmatched stmtMatched expStmt compoundStmt localDecls stmtList
%type <node> selectStmtUnmatched selectStmtMatched iterStmtUnmatched iterStmtMatched iterRange
%type <node> returnStmt breakStmt exp assignop simpleExp andExp unaryRelExp relExp relOp sumExp
%type <node> sumOp mulExp mulOp unaryExp Unary factor mutable immutable call args argList constant

%type <type> typeSpec

%%

program                 : declList
                        {
                            $$ = $1;
                            root = $$;
                        }
                        ;

declList                : declList decl
                        {
                            $$ = $1;
                            $$->addSibling($2);
                        }
                        | decl
                        {
                            $$ = $1;
                        }
                        ;

decl                    : varDecl
                        {
                            $$ = $1;
                        }
                        | funDecl
                        {
                            $$ = $1;
                        }
                        ;

varDecl                 : typeSpec varDeclList SEMICOLON
                        {
                            
                        }
                        ;

scopedVarDecl           : STATIC typeSpec varDeclList SEMICOLON
                        | typeSpec varDeclList SEMICOLON
                        ;

varDeclList             : varDeclList COMMA varDeclInit
                        | varDeclInit
                        ;

varDeclInit             : varDeclId
                        | varDeclId COLON simpleExp
                        ;

varDeclId               : ID
                        | ID LBRACK NUMCONST RBRACK
                        ;

typeSpec                : BOOL
                        | CHAR
                        | INT
                        ;

funDecl                 : typeSpec ID LPAREN parms RPAREN compoundStmt
                        | ID LPAREN parms RPAREN compoundStmt
                        ;

parms                   : parmList
                        |
                        ;

parmList                : parmList SEMICOLON parmTypeList
                        | parmTypeList
                        ;

parmTypeList            : typeSpec parmIdList
                        ;

parmIdList              : parmIdList COMMA parmId
                        | parmId
                        ;

parmId                  : ID
                        | ID LBRACK RBRACK
                        ;

stmt                    : stmtUnmatched
                        | stmtMatched
                        ;

stmtUnmatched           : selectStmtUnmatched
                        | iterStmtUnmatched
                        ;

stmtMatched             : selectStmtMatched
                        | iterStmtMatched
                        | expStmt
                        | compoundStmt
                        | returnStmt
                        | breakStmt
                        ;

expStmt                 : exp SEMICOLON
                        | SEMICOLON
                        ;

compoundStmt            : LCURLY localDecls stmtList RCURLY
                        ;

localDecls              : localDecls scopedVarDecl
                        |
                        ;

stmtList                : stmtList stmt
                        |
                        ;

selectStmtUnmatched     : IF simpleExp THEN stmt
                        | IF simpleExp THEN stmtMatched ELSE stmtUnmatched
                        ;

selectStmtMatched       : IF simpleExp THEN stmtMatched ELSE stmtMatched
                        ;

iterStmtUnmatched       : WHILE simpleExp DO stmtUnmatched
                        | FOR ID ASGN iterRange DO stmtUnmatched
                        ;

iterStmtMatched         : WHILE simpleExp DO stmtMatched
                        | FOR ID ASGN iterRange DO stmtMatched
                        ;

iterRange               : simpleExp TO simpleExp
                        | simpleExp TO simpleExp BY simpleExp
                        ;

returnStmt              : RETURN SEMICOLON
                        | RETURN exp SEMICOLON
                        ;

breakStmt               : BREAK SEMICOLON
                        ;

exp                     : mutable assignop exp
                        | mutable INC
                        | mutable DEC
                        | simpleExp
                        ;

assignop                : ASGN
                        | ADDASGN
                        | SUBASGN
                        | MULASGN
                        | DIVASGN
                        ;

simpleExp               : simpleExp OR andExp
                        | andExp
                        ;

andExp                  : andExp AND unaryRelExp
                        | unaryRelExp
                        ;

unaryRelExp             : NOT unaryRelExp
                        | relExp
                        ;

relExp                  : sumExp relOp sumExp
                        | sumExp
                        ;

relOp                   : LT
                        | LEQ
                        | GT
                        | GEQ
                        | EQ
                        | NEQ
                        ;

sumExp                  : sumExp sumOp mulExp
                        | mulExp
                        ;

sumOp                   : ADD
                        | SUB
                        ;

mulExp                  : mulExp mulOp unaryExp
                        | unaryExp
                        ;

mulOp                   : MUL
                        | DIV
                        | MOD
                        ;

unaryExp                : Unary unaryExp
                        | factor
                        ;

Unary                 : SUB
                        | MUL
                        | QUESTION
                        ;

factor                  : mutable
                        | immutable
                        ;

mutable                 : ID
                        | ID LBRACK exp RBRACK
                        ;

immutable               : LPAREN exp RPAREN
                        | call
                        | constant
                        ;

call                    : ID LPAREN args RPAREN
                        ;

args                    : argList
                        |
                        ;

argList                 : argList COMMA exp
                        | exp
                        ;

constant                : NUMCONST
                        | BOOLCONST
                        | CHARCONST
                        | STRINGCONST
                        ;

%%

int main(int argc, char *argv[])
{
    Flags flags(argc, argv);
    yydebug = flags.getDebugFlag();

    std::string file = flags.getFile();
    if (file.length())
    {
        if ((yyin = fopen(file.c_str(), "r")))
        {
            yyparse();
        }
        else
        {
            std::cout << "ERROR: failed to open \'" << file << "\'" << std::endl;
            exit(EXIT_FAILURE);
        }
    }
    else
    {
        yyparse();
    }

    return 0;
}

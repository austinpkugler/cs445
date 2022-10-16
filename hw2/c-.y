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
    Primitive::Type type;
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
%type <node> sumOp mulExp mulOp unaryExp unaryOp factor mutable immutable call args argList constant

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
                            $$ = $2;
                            Var *node = (Var *)$$;
                            node->setType($1);
                        }
                        ;

scopedVarDecl           : STATIC typeSpec varDeclList SEMICOLON
                        {
                            $$ = $3;
                            Var *node = (Var *)$$;
                            node->setType($2);
                            node->makeStatic();
                        }
                        | typeSpec varDeclList SEMICOLON
                        {
                            $$ = $2;
                            Var *node = (Var *)$$;
                            node->setType($1);
                        }
                        ;

varDeclList             : varDeclList COMMA varDeclInit
                        {
                            $$ = $1;
                            $$->addSibling($3);
                        }
                        | varDeclInit
                        {
                            $$ = $1;
                        }
                        ;

varDeclInit             : varDeclId
                        {
                            $$ = $1;
                        }
                        | varDeclId COLON simpleExp
                        {
                            $$ = $1;
                            $$->addChild($3);
                        }
                        ;

varDeclId               : ID
                        {
                            $$ = new Var($1->tokenLineNum, new Primitive(Primitive::Type::Void), $1->tokenContent);
                        }
                        | ID LBRACK NUMCONST RBRACK
                        {
                            $$ = new Var($1->tokenLineNum, new Primitive(Primitive::Type::Void, true), $1->tokenContent);
                        }
                        ;

typeSpec                : INT
                        {
                            $$ = Primitive::Type::Int;
                        }
                        | BOOL
                        {
                            $$ = Primitive::Type::Bool;
                        }
                        | CHAR
                        {
                            $$ = Primitive::Type::Char;
                        }
                        ;

funDecl                 : typeSpec ID LPAREN parms RPAREN compoundStmt
                        {
                            $$ = new Func($2->tokenLineNum, new Primitive($1), $2->tokenContent);
                            $$->addChild($4);
                            $$->addChild($6);
                        }
                        | ID LPAREN parms RPAREN compoundStmt
                        {
                            $$ = new Func($1->tokenLineNum, new Primitive(Primitive::Type::Void), $1->tokenContent);
                            $$->addChild($3);
                            $$->addChild($5);
                        }
                        ;

parms                   : parmList
                        {
                            $$ = $1;
                        }
                        |
                        {
                            $$ = nullptr;
                        }
                        ;

parmList                : parmList SEMICOLON parmTypeList
                        {
                            $$ = $1;
                            $$->addSibling($3);
                        }
                        | parmTypeList
                        {
                            $$ = $1;
                        }
                        ;

parmTypeList            : typeSpec parmIdList
                        {
                            $$ = $2;
                            Parm *node = (Parm *)$$;
                            node->setType($1);
                        }
                        ;

parmIdList              : parmIdList COMMA parmId
                        {
                            if ($1 == nullptr)
                            {
                                $$ = $3;
                            }
                            else
                            {
                                $$ = $1;
                                $$->addSibling($3);
                            }
                        }
                        | parmId
                        {
                            $$ = $1;
                        }
                        ;

parmId                  : ID
                        {
                            $$ = new Parm($1->tokenLineNum, new Primitive(Primitive::Type::Void), $1->tokenContent);
                        }
                        | ID LBRACK RBRACK
                        {
                            $$ = new Parm($1->tokenLineNum, new Primitive(Primitive::Type::Void, true), $1->tokenContent);
                        }
                        ;

stmt                    : stmtUnmatched
                        {
                            $$ = $1;
                        }
                        | stmtMatched
                        {
                            $$ = $1;
                        }
                        ;

stmtUnmatched           : selectStmtUnmatched
                        {
                            $$ = $1;
                        }
                        | iterStmtUnmatched
                        {
                            $$ = $1;
                        }
                        ;

stmtMatched             : selectStmtMatched
                        {
                            $$ = $1;
                        }
                        | iterStmtMatched
                        {
                            $$ = $1;
                        }
                        | expStmt
                        {
                            $$ = $1;
                        }
                        | compoundStmt
                        {
                            $$ = $1;
                        }
                        | returnStmt
                        {
                            $$ = $1;
                        }
                        | breakStmt
                        {
                            $$ = $1;
                        }
                        ;

expStmt                 : exp SEMICOLON
                        {
                            $$ = $1;
                        }
                        | SEMICOLON
                        {
                            $$ = nullptr;
                        }
                        ;

compoundStmt            : LCURLY localDecls stmtList RCURLY
                        {
                            $$ = new Compound($1->tokenLineNum);
                            $$->addChild($2);
                            $$->addChild($3);
                        }
                        ;

localDecls              : localDecls scopedVarDecl
                        {
                            if ($1 == nullptr)
                            {
                                $$ = $2;
                            }
                            else
                            {
                                $$ = $1;
                                $$->addSibling($2);
                            }
                        }
                        |
                        {
                            $$ = nullptr;
                        }
                        ;

stmtList                : stmtList stmt
                        {
                            if ($1 == nullptr)
                            {
                                $$ = $2;
                            }
                            else
                            {
                                $$ = $1;
                                $$->addSibling($2);
                            }
                        }
                        |
                        {
                            $$ = nullptr;
                        }
                        ;

selectStmtUnmatched     : IF simpleExp THEN stmt
                        {
                            $$ = new If($1->tokenLineNum);
                            $$->addChild($2);
                            $$->addChild($4);
                        }
                        | IF simpleExp THEN stmtMatched ELSE stmtUnmatched
                        {
                            $$ = new If($1->tokenLineNum);
                            $$->addChild($2);
                            $$->addChild($4);
                            $$->addChild($6);
                        }
                        ;

selectStmtMatched       : IF simpleExp THEN stmtMatched ELSE stmtMatched
                        {
                            $$ = new If($1->tokenLineNum);
                            $$->addChild($2);
                            $$->addChild($4);
                            $$->addChild($6);
                        }
                        ;

iterStmtUnmatched       : WHILE simpleExp DO stmtUnmatched
                        {
                            $$ = new While($1->tokenLineNum);
                            $$->addChild($2);
                            $$->addChild($4);
                        }
                        | FOR ID ASGN iterRange DO stmtUnmatched
                        {
                            $$ = new For($1->tokenLineNum);
                            Var *node = new Var($1->tokenLineNum, new Primitive(Primitive::Type::Int), $1->tokenContent);
                            $$->addChild(node);
                            $$->addChild($4);
                            $$->addChild($6);
                        }
                        ;

iterStmtMatched         : WHILE simpleExp DO stmtMatched
                        {
                            $$ = new While($1->tokenLineNum);
                            $$->addChild($2);
                            $$->addChild($4);
                        }
                        | FOR ID ASGN iterRange DO stmtMatched
                        {
                            $$ = new For($1->tokenLineNum);
                            Var *node = new Var($2->tokenLineNum, new Primitive(Primitive::Type::Int), $2->tokenContent);
                            $$->addChild(node);
                            $$->addChild($4);
                            $$->addChild($6);
                        }
                        ;

iterRange               : simpleExp TO simpleExp
                        {
                            $$ = new Range($1->getTokenLineNum());
                            $$->addChild($1);
                            $$->addChild($3);
                        }
                        | simpleExp TO simpleExp BY simpleExp
                        {
                            $$ = new Range($1->getTokenLineNum());
                            $$->addChild($1);
                            $$->addChild($3);
                            $$->addChild($5);
                        }
                        ;

returnStmt              : RETURN SEMICOLON
                        {
                            $$ = new Return($1->tokenLineNum);
                        }
                        | RETURN exp SEMICOLON
                        {
                            $$ = new Return($1->tokenLineNum);
                            $$->addChild($2);
                        }
                        ;

breakStmt               : BREAK SEMICOLON
                        {
                            $$ = new Break($1->tokenLineNum);
                        }
                        ;

exp                     : mutable assignop exp
                        {
                            $$ = $2;
                            $$->addChild($1);
                            $$->addChild($3);
                        }
                        | mutable INC
                        {
                            $$ = new UnaryAsgn($1->getTokenLineNum(), UnaryAsgn::Type::Inc);
                            $$->addChild($1);
                        }
                        | mutable DEC
                        {
                            $$ = new UnaryAsgn($1->getTokenLineNum(), UnaryAsgn::Type::Dec);
                            $$->addChild($1);
                        }
                        | simpleExp
                        {
                            $$ = $1;
                        }
                        ;

assignop                : ASGN
                        {
                            $$ = new Asgn($1->tokenLineNum, Asgn::Type::Asgn);
                        }
                        | ADDASGN
                        {
                            $$ = new Asgn($1->tokenLineNum, Asgn::Type::AddAsgn);
                        }
                        | SUBASGN
                        {
                            $$ = new Asgn($1->tokenLineNum, Asgn::Type::SubAsgn);
                        }
                        | MULASGN
                        {
                            $$ = new Asgn($1->tokenLineNum, Asgn::Type::MulAsgn);
                        }
                        | DIVASGN
                        {
                            $$ = new Asgn($1->tokenLineNum, Asgn::Type::DivAsgn);
                        }
                        ;

simpleExp               : simpleExp OR andExp
                        {
                            $$ = new Binary($1->getTokenLineNum(), Binary::Type::Or);
                            $$->addChild($1);
                            $$->addChild($3);
                        }
                        | andExp
                        {
                            $$ = $1;
                        }
                        ;

andExp                  : andExp AND unaryRelExp
                        {
                            $$ = new Binary($1->getTokenLineNum(), Binary::Type::And);
                            $$->addChild($1);
                            $$->addChild($3);
                        }
                        | unaryRelExp
                        {
                            $$ = $1;
                        }
                        ;

unaryRelExp             : NOT unaryRelExp
                        {
                            $$ = new Unary($1->tokenLineNum, Unary::Type::Not);
                            $$->addChild($2);
                        }
                        | relExp
                        {
                            $$ = $1;
                        }
                        ;

relExp                  : sumExp relOp sumExp
                        {
                            $$ = $2;
                            $$->addChild($1);
                            $$->addChild($3);
                        }
                        | sumExp
                        {
                            $$ = $1;
                        }
                        ;

relOp                   : LT
                        {
                            $$ = new Binary($1->tokenLineNum, Binary::Type::LT);
                        }
                        | LEQ
                        {
                            $$ = new Binary($1->tokenLineNum, Binary::Type::LEQ);
                        }
                        | GT
                        {
                            $$ = new Binary($1->tokenLineNum, Binary::Type::GT);
                        }
                        | GEQ
                        {
                            $$ = new Binary($1->tokenLineNum, Binary::Type::GEQ);
                        }
                        | EQ
                        {
                            $$ = new Binary($1->tokenLineNum, Binary::Type::EQ);
                        }
                        | NEQ
                        {
                            $$ = new Binary($1->tokenLineNum, Binary::Type::NEQ);
                        }
                        ;

sumExp                  : sumExp sumOp mulExp
                        {
                            $$ = $2;
                            $$->addChild($1);
                            $$->addChild($3);
                        }
                        | mulExp
                        {
                            $$ = $1;
                        }
                        ;

sumOp                   : ADD
                        {
                            $$ = new Binary($1->tokenLineNum, Binary::Type::Add);
                        }
                        | SUB
                        {
                            $$ = new Binary($1->tokenLineNum, Binary::Type::Sub);
                        }
                        ;

mulExp                  : mulExp mulOp unaryExp
                        {
                            $$ = $2;
                            $$->addChild($1);
                            $$->addChild($3);
                        }
                        | unaryExp
                        {
                            $$ = $1;
                        }
                        ;

mulOp                   : MUL
                        {
                            $$ = new Binary($1->tokenLineNum, Binary::Type::Mul);
                        }
                        | DIV
                        {
                            $$ = new Binary($1->tokenLineNum, Binary::Type::Div);
                        }
                        | MOD
                        {
                            $$ = new Binary($1->tokenLineNum, Binary::Type::Mod);
                        }
                        ;

unaryExp                : unaryOp unaryExp
                        {
                            $$ = $1;
                            $$->addChild($2);
                        }
                        | factor
                        {
                            $$ = $1;
                        }
                        ;

unaryOp                 : SUB
                        {
                            $$ = new Unary($1->tokenLineNum, Unary::Type::Chsign);
                        }
                        | MUL
                        {
                            $$ = new Unary($1->tokenLineNum, Unary::Type::Sizeof);
                        }
                        | QUESTION
                        {
                            $$ = new Unary($1->tokenLineNum, Unary::Type::Question);
                        }
                        ;

factor                  : mutable
                        {
                            $$ = $1;
                        }
                        | immutable
                        {
                            $$ = $1;
                        }
                        ;

mutable                 : ID
                        {
                            $$ = new Id($1->tokenLineNum, $1->tokenContent);
                        }
                        | ID LBRACK exp RBRACK
                        {
                            $$ = new Binary($1->tokenLineNum, Binary::Type::Index);
                            Id *node = new Id($1->tokenLineNum, $1->tokenContent, true);
                            $$->addChild(node);
                            $$->addChild($3);
                        }
                        ;

immutable               : LPAREN exp RPAREN
                        {
                            $$ = $2;
                        }
                        | call
                        {
                            $$ = $1;
                        }
                        | constant
                        {
                            $$ = $1;
                        }
                        ;

call                    : ID LPAREN args RPAREN
                        {
                            $$ = new Call($1->tokenLineNum, $1->tokenContent);
                            $$->addChild($3);
                        }
                        ;

args                    : argList
                        {
                            $$ = $1;
                        }
                        |
                        {
                            $$ = nullptr;
                        }
                        ;

argList                 : argList COMMA exp
                        {
                            $$ = $1;
                            $$->addSibling($3);
                        }
                        | exp
                        {
                            $$ = $1;
                        }
                        ;

constant                : NUMCONST
                        {
                            $$ = new Const($1->tokenLineNum, Const::Type::Int, $1->tokenContent);
                        }
                        | BOOLCONST
                        {
                            $$ = new Const($1->tokenLineNum, Const::Type::Bool, $1->tokenContent);
                        }
                        | CHARCONST
                        {
                            $$ = new Const($1->tokenLineNum, Const::Type::Char, $1->tokenContent);
                        }
                        | STRINGCONST
                        {
                            $$ = new Const($1->tokenLineNum, Const::Type::String, $1->tokenContent);
                        }
                        ;

%%

int main(int argc, char *argv[])
{
    Flags flags(argc, argv);
    yydebug = flags.getDebugFlag();

    std::string filename = flags.getFile();
    if (filename.length() > 0)
    {
        FILE* file = fopen(filename.c_str(), "r");
        if (file == NULL)
        {
            throw std::runtime_error("Cannot open file: \'" + filename + "\'");
        }

        yyin = file;
        fclose(file);
    }

    yyparse();

    if (flags.getPrintFlag() && root != nullptr)
    {
        /* if (root == nullptr)
        {
            throw std::runtime_error("Cannot print root: nullptr");
        } */
        root->printTree();
    }

    return 0;
}

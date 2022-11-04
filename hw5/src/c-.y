%{
// Based on CS445 - Calculator Example Program by Robert Heckendorn and yyerror.h by Michael Wilder
#include "Error.hpp"
#include "TokenData.hpp"
#include "Emit/Emit.hpp"
#include "Flags/Flags.hpp"
#include "Semantics/Semantics.hpp"
#include "Semantics/SymTable.hpp"
#include "Tree/Tree.hpp"

#include <iostream>
#include <sstream>
#include <string>
#include <stdio.h>

// From yacc
extern int yylex();
extern int yydebug;
extern FILE *yyin;

// From c-.l scanner
extern int lineCount;
extern char *lastToken;

// AST
Node *root;
bool hasSyntaxError = false;

#define YYERROR_VERBOSE
void yyerror(const char *msg)
{
    char *space;
    char *strs[100];
    int numstrs;

    // Make a copy of msg string
    space = strdup(msg);

    // Split out components
    numstrs = Error::split(space, strs, ' ');
    if (numstrs > 4)
    {
        Error::trim(strs[3]);
    }

    // Translate components
    for (int i = 3; i < numstrs; i += 2)
    {
        if (std::string(strs[i]) == "CHARCONST" && lastToken[0] == '\'' && lastToken[1] == '\'')
        {
            Emit::error(lineCount, "Empty character ''.  Characters ignored.");
        }
        else
        {
            strs[i] = Error::niceTokenStr(strs[i]);
        }
    }

    // Print components
    std::string typeStr = std::string(strs[3]);
    if (typeStr != "CHARCONST")
    {
        hasSyntaxError = true;
        printf("ERROR(%d): Syntax error, unexpected %s", lineCount, strs[3]);
        if (Error::elaborate(strs[3]))
        {
            if (lastToken[0]=='\'' || lastToken[0]=='"')
            {
                printf(" %s", lastToken);
            }
            else
            {
                printf(" \"%s\"", lastToken);
            }
        }

        if (numstrs > 4)
        {
            printf(",");
        }

        // Print sorted list of expected
        Error::tinySort(strs + 5, numstrs - 5, 2, true);
        for (int i = 4; i < numstrs; i++)
        {
            printf(" %s", strs[i]);
        }
        printf(".\n");
        fflush(stdout);
        Emit::incErrorCount();

        if (typeStr == "character constant")
        {
            std::string chars = Const::removeFirstAndLastChar(lastToken);
            if (chars.length() > 1 && chars[0] != '\\')
            {
                std::stringstream msg;
                msg << "character is " << chars.length() << " characters long and not a single character: '" << lastToken << "'.  The first char will be used.";
                Emit::warn(lineCount, msg.str());
            }
        }
    }
    free(space);
}

%}

%union
{
    Data::Type type;
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
                        | error
                        {
                            $$ = nullptr;
                        }
                        ;

varDecl                 : typeSpec varDeclList SEMICOLON
                        {
                            $$ = $2;
                            Var *var = (Var *)$$;
                            if (var)
                            {
                                var->setType($1);
                            }
                            yyerrok;
                        }
                        | error varDeclList SEMICOLON
                        {
                            $$ = nullptr;
                            yyerrok;
                        }
                        | typeSpec error SEMICOLON
                        {
                            $$ = nullptr;
                            yyerrok;
                        }
                        ;

scopedVarDecl           : STATIC typeSpec varDeclList SEMICOLON
                        {
                            $$ = $3;
                            Var *var = (Var *)$$;
                            if (var)
                            {
                                var->setType($2);
                                var->makeStatic();
                            }
                            yyerrok;
                        }
                        | typeSpec varDeclList SEMICOLON
                        {
                            $$ = $2;
                            Var *var = (Var *)$$;
                            if (var)
                            {
                                var->setType($1);
                            }
                            yyerrok;
                        }
                        ;

varDeclList             : varDeclList COMMA varDeclInit
                        {
                            $$ = $1;
                            $$->addSibling($3);
                            yyerrok;
                        }
                        | varDeclInit
                        {
                            $$ = $1;
                        }
                        | varDeclList COMMA error
                        {
                            $$ = nullptr;
                        }
                        | error
                        {
                            $$ = nullptr;
                        }
                        ;

varDeclInit             : varDeclId
                        {
                            $$ = $1;
                        }
                        | varDeclId COLON simpleExp
                        {
                            Var *var = (Var *)($1);
                            if (var)
                            {
                                var->makeInitialized();
                            }
                            $$ = var;
                            if ($$)
                            {
                                $$->addChild($3);
                            }
                        }
                        | error COLON simpleExp
                        {
                            $$ = nullptr;
                            yyerrok;
                        }
                        ;

varDeclId               : ID
                        {
                            $$ = new Var($1->lineNum, $1->tokenContent, new Data(Data::Type::Undefined, false, false));
                        }
                        | ID LBRACK NUMCONST RBRACK
                        {
                            $$ = new Var($1->lineNum, $1->tokenContent, new Data(Data::Type::Undefined, true, false));
                        }
                        | ID LBRACK error
                        {
                            $$ = nullptr;
                        }
                        | error RBRACK
                        {
                            $$ = nullptr;
                            yyerrok;
                        }
                        ;

typeSpec                : INT
                        {
                            $$ = Data::Type::Int;
                        }
                        | BOOL
                        {
                            $$ = Data::Type::Bool;
                        }
                        | CHAR
                        {
                            $$ = Data::Type::Char;
                        }
                        ;

funDecl                 : typeSpec ID LPAREN parms RPAREN compoundStmt
                        {
                            $$ = new Func($2->lineNum, $2->tokenContent, new Data($1, false, false));
                            $$->addChild($4);
                            $$->addChild($6);
                        }
                        | ID LPAREN parms RPAREN compoundStmt
                        {
                            $$ = new Func($1->lineNum, $1->tokenContent, new Data(Data::Type::Void, false, false));
                            $$->addChild($3);
                            $$->addChild($5);
                        }
                        | typeSpec error
                        {
                            $$ = nullptr;
                        }
                        | typeSpec ID LPAREN error
                        {
                            $$ = nullptr;
                        }
                        | ID LPAREN error
                        {
                            $$ = nullptr;
                        }
                        | ID LPAREN parms RPAREN error
                        {
                            $$ = nullptr;
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
                        | parmList SEMICOLON error
                        {
                            $$ = nullptr;
                        }
                        | error
                        {
                            $$ = nullptr;
                        }
                        ;

parmTypeList            : typeSpec parmIdList
                        {
                            $$ = $2;
                            Parm *parm = (Parm *)$$;
                            if (parm)
                            {
                                parm->setType($1);
                            }
                        }
                        | typeSpec error
                        {
                            $$ = nullptr;
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
                            yyerrok;
                        }
                        | parmId
                        {
                            $$ = $1;
                        }
                        | parmIdList COMMA error
                        {
                            $$ = nullptr;
                        }
                        | error
                        {
                            $$ = nullptr;
                        }
                        ;

parmId                  : ID
                        {
                            $$ = new Parm($1->lineNum, $1->tokenContent, new Data(Data::Type::Undefined, false, false));
                        }
                        | ID LBRACK RBRACK
                        {
                            $$ = new Parm($1->lineNum, $1->tokenContent, new Data(Data::Type::Undefined, true, false));
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
                        | IF error THEN stmt
                        {
                            $$ = nullptr;
                            yyerrok;
                        }
                        | IF error THEN stmtMatched ELSE stmtUnmatched
                        {
                            $$ = nullptr;
                            yyerrok;
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
                        | IF error
                        {
                            $$ = nullptr;
                        }
                        | IF error ELSE stmtMatched
                        {
                            $$ = nullptr;
                            yyerrok;
                        }
                        | IF error THEN stmtMatched ELSE stmtMatched
                        {
                            $$ = nullptr;
                            yyerrok;
                        }
                        | WHILE error DO stmtMatched
                        {
                            $$ = nullptr;
                            yyerrok;
                        }
                        | WHILE error
                        {
                            $$ = nullptr;
                        }
                        | FOR ID ASGN error DO stmtMatched
                        {
                            $$ = nullptr;
                            yyerrok;
                        }
                        | FOR error
                        {
                            $$ = nullptr;
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
                        | error SEMICOLON
                        {
                            $$ = nullptr;
                            yyerrok;
                        }
                        ;

compoundStmt            : LCURLY localDecls stmtList RCURLY
                        {
                            $$ = new Compound($1->lineNum);
                            $$->addChild($2);
                            $$->addChild($3);
                            yyerrok;
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
                            $$ = new If($1->lineNum);
                            $$->addChild($2);
                            $$->addChild($4);
                        }
                        | IF simpleExp THEN stmtMatched ELSE stmtUnmatched
                        {
                            $$ = new If($1->lineNum);
                            $$->addChild($2);
                            $$->addChild($4);
                            $$->addChild($6);
                        }
                        ;

selectStmtMatched       : IF simpleExp THEN stmtMatched ELSE stmtMatched
                        {
                            $$ = new If($1->lineNum);
                            $$->addChild($2);
                            $$->addChild($4);
                            $$->addChild($6);
                        }
                        ;

iterStmtUnmatched       : WHILE simpleExp DO stmtUnmatched
                        {
                            $$ = new While($1->lineNum);
                            $$->addChild($2);
                            $$->addChild($4);
                        }
                        | FOR ID ASGN iterRange DO stmtUnmatched
                        {
                            $$ = new For($1->lineNum);
                            Var *var = new Var($2->lineNum, $2->tokenContent, new Data(Data::Type::Int, false, false));
                            var->makeInitialized();
                            $$->addChild(var);
                            $$->addChild($4);
                            $$->addChild($6);
                        }
                        ;

iterStmtMatched         : WHILE simpleExp DO stmtMatched
                        {
                            $$ = new While($1->lineNum);
                            $$->addChild($2);
                            $$->addChild($4);
                        }
                        | FOR ID ASGN iterRange DO stmtMatched
                        {
                            $$ = new For($1->lineNum);
                            Var *var = new Var($2->lineNum, $2->tokenContent, new Data(Data::Type::Int, false, false));
                            var->makeInitialized();
                            $$->addChild(var);
                            $$->addChild($4);
                            $$->addChild($6);
                        }
                        ;

iterRange               : simpleExp TO simpleExp
                        {
                            $$ = new Range($1->getLineNum());
                            $$->addChild($1);
                            $$->addChild($3);
                        }
                        | simpleExp TO simpleExp BY simpleExp
                        {
                            $$ = new Range($1->getLineNum());
                            $$->addChild($1);
                            $$->addChild($3);
                            $$->addChild($5);
                        }
                        | simpleExp TO error
                        {
                            $$ = nullptr;
                        }
                        | error BY error
                        {
                            $$ = nullptr;
                            yyerrok;
                        }
                        | simpleExp TO simpleExp BY error
                        {
                            $$ = nullptr;
                        }
                        ;

returnStmt              : RETURN SEMICOLON
                        {
                            $$ = new Return($1->lineNum);
                        }
                        | RETURN exp SEMICOLON
                        {
                            $$ = new Return($1->lineNum);
                            $$->addChild($2);
                            yyerrok;
                        }
                        | RETURN error SEMICOLON
                        {
                            $$ = nullptr;
                            yyerrok;
                        }
                        ;

breakStmt               : BREAK SEMICOLON
                        {
                            $$ = new Break($1->lineNum);
                        }
                        | BREAK error SEMICOLON
                        {
                            $$ = nullptr;
                            yyerrok;
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
                            $$ = new UnaryAsgn($1->getLineNum(), UnaryAsgn::Type::Inc);
                            $$->addChild($1);
                        }
                        | mutable DEC
                        {
                            $$ = new UnaryAsgn($1->getLineNum(), UnaryAsgn::Type::Dec);
                            $$->addChild($1);
                        }
                        | simpleExp
                        {
                            $$ = $1;
                        }
                        | error assignop exp
                        {
                            $$ = nullptr;
                            yyerrok;
                        }
                        | mutable assignop error
                        {
                            $$ = nullptr;
                        }
                        | error INC
                        {
                            $$ = nullptr;
                            yyerrok;
                        }
                        | error DEC
                        {
                            $$ = nullptr;
                            yyerrok;
                        }
                        ;

assignop                : ASGN
                        {
                            $$ = new Asgn($1->lineNum, Asgn::Type::Asgn);
                        }
                        | ADDASGN
                        {
                            $$ = new Asgn($1->lineNum, Asgn::Type::AddAsgn);
                        }
                        | SUBASGN
                        {
                            $$ = new Asgn($1->lineNum, Asgn::Type::SubAsgn);
                        }
                        | MULASGN
                        {
                            $$ = new Asgn($1->lineNum, Asgn::Type::MulAsgn);
                        }
                        | DIVASGN
                        {
                            $$ = new Asgn($1->lineNum, Asgn::Type::DivAsgn);
                        }
                        ;

simpleExp               : simpleExp OR andExp
                        {
                            if ($1)
                            {
                                $$ = new Binary($1->getLineNum(), Binary::Type::Or);
                                $$->addChild($1);
                                $$->addChild($3);
                            }
                            else
                            {
                                $$ = nullptr;
                            }
                        }
                        | andExp
                        {
                            $$ = $1;
                        }
                        | simpleExp OR error
                        {
                            $$ = nullptr;
                        }
                        ;

andExp                  : andExp AND unaryRelExp
                        {
                            if ($1)
                            {
                                $$ = new Binary($1->getLineNum(), Binary::Type::And);
                                $$->addChild($1);
                                $$->addChild($3);
                            }
                            else
                            {
                                $$ = nullptr;
                            }
                        }
                        | unaryRelExp
                        {
                            $$ = $1;
                        }
                        | andExp AND error
                        {
                            $$ = nullptr;
                        }
                        ;

unaryRelExp             : NOT unaryRelExp
                        {
                            $$ = new Unary($1->lineNum, Unary::Type::Not);
                            $$->addChild($2);
                        }
                        | relExp
                        {
                            $$ = $1;
                        }
                        | NOT error
                        {
                            $$ = nullptr;
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
                        | sumExp relOp error
                        {
                            $$ = nullptr;
                        }
                        ;

relOp                   : LT
                        {
                            $$ = new Binary($1->lineNum, Binary::Type::LT);
                        }
                        | LEQ
                        {
                            $$ = new Binary($1->lineNum, Binary::Type::LEQ);
                        }
                        | GT
                        {
                            $$ = new Binary($1->lineNum, Binary::Type::GT);
                        }
                        | GEQ
                        {
                            $$ = new Binary($1->lineNum, Binary::Type::GEQ);
                        }
                        | EQ
                        {
                            $$ = new Binary($1->lineNum, Binary::Type::EQ);
                        }
                        | NEQ
                        {
                            $$ = new Binary($1->lineNum, Binary::Type::NEQ);
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
                        | sumExp sumOp error
                        {
                            $$ = nullptr;
                        }
                        ;

sumOp                   : ADD
                        {
                            $$ = new Binary($1->lineNum, Binary::Type::Add);
                        }
                        | SUB
                        {
                            $$ = new Binary($1->lineNum, Binary::Type::Sub);
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
                        | mulExp mulOp error
                        {
                            $$ = nullptr;
                        }
                        ;

mulOp                   : MUL
                        {
                            $$ = new Binary($1->lineNum, Binary::Type::Mul);
                        }
                        | DIV
                        {
                            $$ = new Binary($1->lineNum, Binary::Type::Div);
                        }
                        | MOD
                        {
                            $$ = new Binary($1->lineNum, Binary::Type::Mod);
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
                        | unaryOp error
                        {
                            $$ = nullptr;
                        }
                        ;

unaryOp                 : SUB
                        {
                            $$ = new Unary($1->lineNum, Unary::Type::Chsign);
                        }
                        | MUL
                        {
                            $$ = new Unary($1->lineNum, Unary::Type::Sizeof);
                        }
                        | QUESTION
                        {
                            $$ = new Unary($1->lineNum, Unary::Type::Question);
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
                            $$ = new Id($1->lineNum, $1->tokenContent);
                        }
                        | ID LBRACK exp RBRACK
                        {
                            $$ = new Binary($1->lineNum, Binary::Type::Index);
                            Id *id = new Id($1->lineNum, $1->tokenContent);
                            $$->addChild(id);
                            $$->addChild($3);
                        }
                        ;

immutable               : LPAREN exp RPAREN
                        {
                            $$ = $2;
                            yyerrok;
                        }
                        | call
                        {
                            $$ = $1;
                        }
                        | constant
                        {
                            $$ = $1;
                        }
                        | LPAREN error
                        {
                            $$ = nullptr;
                        }
                        | error LPAREN
                        {
                            $$ = nullptr;
                            yyerrok;
                        }
                        ;

call                    : ID LPAREN args RPAREN
                        {
                            $$ = new Call($1->lineNum, $1->tokenContent);
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
                            yyerrok;
                        }
                        | exp
                        {
                            $$ = $1;
                        }
                        | argList COMMA error
                        {
                            $$ = nullptr;
                        }
                        ;

constant                : NUMCONST
                        {
                            $$ = new Const($1->lineNum, Const::Type::Int, $1->tokenContent);
                        }
                        | BOOLCONST
                        {
                            $$ = new Const($1->lineNum, Const::Type::Bool, $1->tokenContent);
                        }
                        | CHARCONST
                        {
                            Const *constN = new Const($1->lineNum, Const::Type::Char, $1->tokenContent);
                            if (constN->getCharLengthWarning())
                            {
                                std::stringstream msg;
                                msg << "character is " << constN->getLongConstValue().length() - 2 << " characters long and not a single character: '" << constN->getLongConstValue() << "'.  The first char will be used.";
                                Emit::warn(constN->getLineNum(), msg.str());
                            }
                            $$ = constN;
                        }
                        | STRINGCONST
                        {
                            $$ = new Const($1->lineNum, Const::Type::String, $1->tokenContent);
                        }
                        ;

%%

int main(int argc, char *argv[])
{
    Error::initErrorProcessing();

    Flags flags(argc, argv);
    yydebug = flags.getDebugFlag();

    std::string filename = flags.getFilename();
    if (argc > 1 && !(yyin = fopen(filename.c_str(), "r")))
    {
        Emit::error("ARGLIST", "source file \"" + filename + "\" could not be opened.");
        Emit::count();
        return EXIT_FAILURE;
    }

    std::cout << "====================================" << std::endl;
    std::cout << "FILE: " << filename.substr(filename.find_last_of("/\\") + 1) << std::endl;

    yyparse();

    if (flags.getPrintSyntaxTreeFlag() && root != nullptr)
    {
        root->printTree();
    }

    SymTable symTable = SymTable();
    symTable.debug(flags.getSymTableDebugFlag());

    Semantics analyzer = Semantics(&symTable);
    if (!hasSyntaxError)
    {
        analyzer.analyze(root);
    }

    if (flags.getPrintAnnotatedSyntaxTreeFlag() && root != nullptr && !Emit::getErrorCount() && !hasSyntaxError)
    {
        root->printTree(true);
    }

    Emit::count();

    delete root;
    fclose(yyin);

    return EXIT_SUCCESS;
}

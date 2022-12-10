//  TM Code emitting utilities
//  implementation for the TINY compiler
//  Compiler Construction: Principles and Practice
//  Kenneth C. Louden
//  Modified slightly by Jorge L. Williams
//  Modified Nov 13, 2020 Robert Heckendorn
//
//  The two comment string forms of the calls allow you to easily
//  compose a comment from text and a symbol name for example. 
//

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "EmitCode.hpp"

extern FILE *code;


//  TM location number for current instruction emission
static int emitLoc = 0;   // next empty slot in Imem growing to lower memory
static int litLoc = 1;    // next empty slot in Dmem growing to higher memory


//  Procedure emitComment prints a comment line 
// with a comment that is the concatenation of c and d
// 
void emitComment(const char *c, const char *cc)
{
    fprintf(code, "* %s %s\n", c, cc);
}


void emitComment(const char *c, int n)
{
    fprintf(code, "* %s %d\n", c, n);
}


//  Procedure emitComment prints a comment line 
// with comment c in the code file
// 
void emitComment(const char *c)
{
    fprintf(code, "* %s\n", c);
}



// emitRO emits a REGISTER-ONLY TM instruction
// op = the opcode
// r = target register
// s = 1st source register
// t = 2nd source register
// c = a comment
// 
void emitRO(const char *op, long long int r, long long int s, long long int t, const char *c, const char *cc)
{
    fprintf(code, "%3d:  %5s  %lld,%lld,%lld\t%s %s\n", emitLoc, op, r, s, t, c, cc);
    fflush(code);
    emitLoc++;
}

void emitRO(const char *op,long long int r,long long int s,long long int t, const char *c)
{
    emitRO(op, r, s, t, c, (char *)"");
}



// emitRM emits a REGISTER-TO-MEMORY TM instruction
// op = the opcode
// r = target register
// d = the offset
// s = the base register
// c = a comment
// 
void emitRM(const char *op, long long int r, long long int d, long long int s, const char *c, const char *cc)
{
    fprintf(code, "%3d:  %5s  %lld,%lld(%lld)\t%s %s\n", emitLoc, op, r, d, s, c, cc);
    fflush(code);
    emitLoc++;
}

void emitRM(const char *op,long long int r,long long int d,long long int s, const char *c)
{
    emitRM(op, r, d, s, c, (char *)"");
}


void emitGoto(int d,long long int s, const char *c, const char *cc)
{
    emitRM((char *)"JMP", (long long int)PC, d, s, c, cc);
}


void emitGoto(int d,long long int s, const char *c)
{
    emitGoto(d,  s, c, (char *)"");
}



// emitRMAbs converts an absolute reference 
// to a pc-relative reference when emitting a
// register-to-memory TM instruction
// op = the opcode
// r = target register
// a = the absolute location in memory
// c = a comment
// 
void emitRMAbs(const char *op, long long int r, long long int a, const char *c, const char *cc)
{
    fprintf(code, "%3d:  %5s  %lld,%lld(%lld)\t%s %s\n", emitLoc, op, r, a - (long long int)(emitLoc + 1),
	    (long long int)PC, c, cc);
    fflush(code);
    emitLoc++;
}


void emitRMAbs(const char *op,long long int r,long long int a, const char *c)
{
    emitRMAbs(op, r, a, c, (char *)"");
}


void emitGotoAbs(int a, const char *c, const char *cc)
{
    emitRMAbs((char *)"JMP", (long long int)PC, a, c, cc);
}


void emitGotoAbs(int a, const char *c)
{
    emitGotoAbs(a, c, (char *)"");
}


// emit a string literal instruction
// 
// IMPORTANT: assumes litterals are stored in the global space at an
// address which is an offset from the beginning of global space.
//
// The input is a NEGATIVE offset from R0 and the output LIT instruction takes
// a POSITIVE distance from R0.
//
// The string is stored from higher order address to lower order address!!
// An example assuming R0=9999:
// 
// 2: LIT "horse"
// 
// 9999  blah
// 9998  5
// 9997  h  <-- location of string
// 9996  o
// 9995  r
// 9994  s 
// 9993  e
// 9992  blah
// 9991  blah
// 9990  blah

int emitStrLit(int goffset, const char *s)
{
    fprintf(code, "%3d:  %5s  \"%s\"\n", goffset, (char *)"LIT", s);
    return goffset;
}


// 
//  Backpatching Functions
// 


// asks where the next instruction will go.   Same as emitSkip(0).
int emitWhereAmI()
{
    return emitLoc;
}


// emitSkip skips "howMany" code
// locations for later backpatch.
// It also returns the current code position.
// emitSkip(0) tells you where you are and reserves no space.
// 


int emitSkip(int howMany)
{
    int i = emitLoc;
    emitLoc += howMany;

    return i;
}


// emitNewLoc sets the emitLoc to a new location
// often loc = a previously skipped location
// 
void emitNewLoc(int loc)
{
    emitLoc = loc;
}


// this back patches a LDA at the instruction address addr that
// jumps to the current instruction location now that it is known.
// This is essentially a backpatched "goto"
void backPatchAJumpToHere(int addr, const char *comment)
{
    int currloc;

    currloc = emitWhereAmI();       // remember where we are
    emitNewLoc(addr);               // go to addr
    emitGotoAbs(currloc, comment);  // the LDA to here
    emitNewLoc(currloc);            // restore addr
}


// this back patches a JZR or JNZ at the instruction address addr that
// jumps to the current instruction location now that it is known.
void backPatchAJumpToHere(const char *cmd, int reg, int addr, const char *comment)
{
    int currloc;

    currloc = emitWhereAmI();       // remember where we are
    emitNewLoc(addr);               // go to addr
    emitRMAbs(cmd, reg, currloc, comment);  // cmd = JZR, JNZ
    emitNewLoc(currloc);            // restore addr
}

void emitIO()
{
    fprintf(code, "%s", R"""(* ** ** ** ** ** ** ** ** ** ** ** **
* IO Library
  1:     ST  3,-1(1)	Store return address 
  2:     IN  2,2,2	Grab int input 
  3:     LD  3,-1(1)	Load return address 
  4:     LD  1,0(1)	Adjust fp 
  5:    JMP  7,0(3)	Return 
  6:     ST  3,-1(1)	Store return address 
  7:     LD  3,-2(1)	Load parameter 
  8:    OUT  3,3,3	Output integer 
  9:     LD  3,-1(1)	Load return address 
 10:     LD  1,0(1)	Adjust fp 
 11:    JMP  7,0(3)	Return 
 12:     ST  3,-1(1)	Store return address 
 13:    INB  2,2,2	Grab bool input 
 14:     LD  3,-1(1)	Load return address 
 15:     LD  1,0(1)	Adjust fp 
 16:    JMP  7,0(3)	Return 
 17:     ST  3,-1(1)	Store return address 
 18:     LD  3,-2(1)	Load parameter 
 19:   OUTB  3,3,3	Output bool 
 20:     LD  3,-1(1)	Load return address 
 21:     LD  1,0(1)	Adjust fp 
 22:    JMP  7,0(3)	Return 
 23:     ST  3,-1(1)	Store return address 
 24:    INC  2,2,2	Grab char input 
 25:     LD  3,-1(1)	Load return address 
 26:     LD  1,0(1)	Adjust fp 
 27:    JMP  7,0(3)	Return 
 28:     ST  3,-1(1)	Store return address 
 29:     LD  3,-2(1)	Load parameter 
 30:   OUTC  3,3,3	Output char 
 31:     LD  3,-1(1)	Load return address 
 32:     LD  1,0(1)	Adjust fp 
 33:    JMP  7,0(3)	Return 
 34:     ST  3,-1(1)	Store return address 
 35:  OUTNL  3,3,3	Output a newline 
 36:     LD  3,-1(1)	Load return address 
 37:     LD  1,0(1)	Adjust fp 
 38:    JMP  7,0(3)	Return 
* ** ** ** ** ** ** ** ** ** ** ** **
)""");
}

char * toChar(const std::string comment)
{
    return const_cast<char *>((comment).c_str());
}

std::string toUpper(std::string s)
{
    std::transform(s.begin(), s.end(), s.begin(), ::toupper);
    return s;
}

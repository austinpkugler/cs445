// // // // // // // // // // // // // // // // // // // // 
//
// Introduction
//
// This symbol table library supplies basic insert and lookup for
// symbols linked to void * pointers of data.
//
// Plenty of room for improvement inlcuding: better debugging setup,
// passing of refs rather than values and purpose built char *
// routines, and C support.
//
// WARNING: lookup will return NULL pointer if key is not in table.
// This means the void * cannot have zero as a legal value! Attempting
// to save a NULL pointer will get a error.
//
// A main() is commented out and has testing code in it.
//
// Robert Heckendorn   Apr 3, 2021
//
#pragma once

#include <map>
#include <vector>
#include <string>
#include <stdio.h>
#include <stdlib.h>

void pointerPrintNothing(void *data);
void pointerPrintAddr(void *data);
void pointerPrintLongInteger(void *data);
void pointerPrintStr(void *data);

// // // // // // // // // // // // // // // // // // // // 
//
// Class: SymTable
//
// Is a stack of scopes.   The global scope is created when the table is
// is constructed and remains for the lifetime of the object instance.
// SymTable manages nested scopes as a result.
//
class SymTable
{
    public:
        SymTable();
        std::map<std::string, void*> getSyms(void);                     // Symbols getter
        void debug(bool state);                                         // Sets the debug flags
        bool test();                                                    // Runs tests to validate the SymTable class
        int depth();                                                    // What is the depth of the scope stack?
        void print(void (*printData)(void *));                          // Print all scopes using data printing function
        void enter(std::string name);                                   // Enter a scope with given name
        void leave();                                                   // Leave a scope (not allowed to leave global)
        void * lookup(std::string sym);                                 // Returns ptr associated with sym anywhere in symbol table, NULL if symbol not found
        void * lookupGlobal(std::string sym);                           // Returns ptr associated with sym in globals, NULL if symbol not found
        bool insert(std::string sym, void *ptr);                        // Inserts new ptr associated with symbol sym in current scope, false if already defined
        bool insertGlobal(std::string sym, void *ptr);                  // Inserts a new ptr associated with symbol sym, false if already defined
        void applyToAll(void (*action)(std::string , void *));          // Apply func to all symbol/data pairs in local scope
        void applyToAllGlobal(void (*action)(std::string , void *));    // Apply func to all symbol/data pairs in global scope

    private:
        class Scope;
        std::vector<Scope *> stack;
        bool debugFlg;
};

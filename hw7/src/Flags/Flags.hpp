#pragma once

#include <iostream>
#include <string>

class Flags
{
    public:
        Flags();
        Flags(int argc, char *argv[]);

        // Getters
        std::string getFilename() const { return m_filename; }
        bool getDebug() const { return m_debug; }
        bool getSymTableDebug() const { return m_symTableDebug; }
        bool getPrintSyntaxTree() const { return m_printSyntaxTree; }
        bool getPrintSyntaxTreeWithTypes() const { return m_printSyntaxTreeWithTypes; }
        bool getPrintSyntaxTreeWithMem() const { return m_printSyntaxTreeWithMem; }
        std::string getFilenameBase() const;

    private:
        void resetAll();
        void emitHelp();

        std::string m_filename;
        bool m_debug;                       // -d
        bool m_symTableDebug;               // -D
        bool m_printSyntaxTree;             // -p
        bool m_printSyntaxTreeWithTypes;    // -P
        bool m_printSyntaxTreeWithMem;      // -M
};

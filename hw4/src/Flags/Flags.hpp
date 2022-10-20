#pragma once

#include <iostream>
#include <string>

class Flags
{
    public:
        Flags();
        Flags(int argc, char *argv[]);

        void resetAll();
        std::string getFile() { return m_file; }
        bool getDebugFlag() { return m_debug; }
        bool getSymTableDebugFlag() { return m_symTableDebug; }
        bool getPrintSyntaxTreeFlag() { return m_printSyntaxTree; }
        bool getPrintAnnotatedSyntaxTreeFlag() { return m_printAnnotatedSyntaxTree; }

    private:
        void emitHelp();

        std::string m_file = "";
        bool m_debug = false;                       // -d
        bool m_symTableDebug = false;               // -D
        bool m_printSyntaxTree = false;             // -p
        bool m_printAnnotatedSyntaxTree = false;    // -P
};

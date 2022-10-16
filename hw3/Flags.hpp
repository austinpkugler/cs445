#pragma once

#include <string>

class Flags
{
    public:
        Flags();
        Flags(int argc, char *argv[]);

        void resetAll();
        std::string getFile() { return m_file; }
        bool getDebugFlag() { return m_debugFlag; }
        bool getPrintFlag() { return m_printFlag; }
        bool getPrintSemanticsFlag() { return m_printSemanticsFlag; }
        bool getSymTblDebugFlag() { return m_symTblDebugFlag; }

    private:
        void emitHelp();

        std::string m_file = "";
        bool m_debugFlag = false;           // -d
        bool m_printFlag = false;           // -p
        bool m_printSemanticsFlag = false;  // -P
        bool m_symTblDebugFlag = false;     // -D
};

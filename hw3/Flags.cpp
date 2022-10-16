// Based on ourgeoptTest.cpp by Robert Heckendorn
#include "Flags.hpp"
#include "ourgetopt/ourgetopt.h"

#include <iostream>

Flags::Flags() {}

Flags::Flags(int argc, char *argv[])
{
    // From ourgetopt
    extern char *optarg;
    extern int optind;

    resetAll();

    bool helpFlag = false;  // -h
    bool errorFlag = false;

    char flag = ' ';
    while (true)
    {
        // Hunt for a string of options
        while ((flag = ourGetopt(argc, argv, (char *)"dph")) != EOF)
        {
            switch (flag)
            {
                case 'd':
                    m_debugFlag = true;
                    break;
                case 'p':
                    m_printFlag = true;
                    break;
                case 'h':
                    helpFlag = true;
                    break;
                case 'P':
                    m_printSemanticsFlag = true;
                    break;
                case 'D':
                    m_symTblDebugFlag = true;
                    break;
                default:
                    errorFlag = true;
            }
        }

        if (helpFlag)
        {
            emitHelp();
            exit(EXIT_SUCCESS);
        }
        else if (errorFlag)
        {
            std::cout << "Unable to interpret flag \'" << flag << "\'" << std::endl;
            emitHelp();
            exit(EXIT_FAILURE);
        }
        else
        {
            // Pick off a nonoption
            if (optind < argc) {
                m_file = argv[optind];
                optind++;
            }
            else
            {
                break;
            }
        }
    }
}

void Flags::resetAll()
{
    m_debugFlag = false;
    m_printFlag = false;
}

void Flags::emitHelp()
{
    std::cout << "usage: ./c- [options] [sourcefile]" << std::endl;
    std::cout << "options:" << std::endl;
    std::cout << "-d: \t - turn on parser debugging" << std::endl;
    std::cout << "-D: \t - turn on symbol table debugging" << std::endl;
    std::cout << "-h: \t - print this usage message" << std::endl;
    std::cout << "-p: \t - print the abstract syntax tree" << std::endl;
    std::cout << "-P: \t - print the abstract syntax tree plus type information" << std::endl;
}

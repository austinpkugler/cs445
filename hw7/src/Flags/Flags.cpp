// Based on ourgeoptTest.cpp by Robert Heckendorn
#include "Flags.hpp"

#include "ourgetopt/ourgetopt.hpp"

Flags::Flags() : m_debug(false), m_symTableDebug(false), m_printSyntaxTree(false), m_printSyntaxTreeWithTypes(false), m_printSyntaxTreeWithMem(false) {}

Flags::Flags(int argc, char *argv[])
{
    // From ourgetopt
    extern char *optarg;
    extern int optind;

    resetAll();

    bool helpFlag = false;
    bool errorFlag = false;

    char flag = ' ';
    while (true)
    {
        // Hunt for a string of options
        while ((flag = ourGetopt(argc, argv, (char *)"hdDpPM")) != EOF)
        {
            switch (flag)
            {
                case 'h':
                    helpFlag = true;
                    break;
                case 'd':
                    m_debug = true;
                    break;
                case 'D':
                    m_symTableDebug = true;
                    break;
                case 'p':
                    m_printSyntaxTree = true;
                    break;
                case 'P':
                    m_printSyntaxTreeWithTypes = true;
                    break;
                case 'M':
                    m_printSyntaxTreeWithMem = true;
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
            if (optind < argc)
            {
                m_filename = argv[optind];
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
    m_debug = false;                       // -d
    m_symTableDebug = false;               // -D
    m_printSyntaxTree = false;             // -p
    m_printSyntaxTreeWithTypes = false;    // -P
    m_printSyntaxTreeWithMem = false;      // -M
}

void Flags::emitHelp()
{
    std::cout << "usage: ./c- [options] [sourcefile]" << std::endl;
    std::cout << "options:" << std::endl;
    std::cout << "-h: \t - print this usage message" << std::endl;
    std::cout << "-d: \t - turn on parser debugging" << std::endl;
    std::cout << "-D: \t - turn on symbol table debugging" << std::endl;
    std::cout << "-p: \t - print the abstract syntax tree" << std::endl;
    std::cout << "-P: \t - print the abstract syntax tree plus type information" << std::endl;
    std::cout << "-M: \t - print the abstract syntax tree plus type and memory information" << std::endl;
}

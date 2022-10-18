#pragma once

#include "Decl.hpp"

class Var : public Decl
{
    public:
        /**
         * @param lineNum Line number of occurrence.
         * @param varName Name of the var
         * @param data Data associated with the decl (e.g. type, isArray, isStatic)
         */
        Var(const unsigned lineNum, const std::string varName, Data *data);

        // Overridden
        std::string stringify() const;

        // Setters
        void makeInitialized();
        void makeStatic();

    private:
        bool m_initialized = false;
};

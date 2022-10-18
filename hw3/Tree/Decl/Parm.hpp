#pragma once

#include "Decl.hpp"

class Parm : public Decl
{
    public:
        /**
         * @param lineNum Line number of occurrence.
         * @param parmName Name of the parm
         * @param data Data associated with the decl (e.g. type, isArray, isStatic)
         */
        Parm(const unsigned lineNum, const std::string parmName, Data *data);

        // Overridden
        std::string stringify() const;

        // Getters
        bool getIsUsed() const { return m_isUsed; }

        // Setters
        void makeUsed();

    private:
        bool m_isUsed = false;
};

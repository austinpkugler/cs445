#pragma once

#include "Decl.hpp"

class Parm : public Decl
{
    public:
        /**
         * @param lineNum Line number of occurrence.
         * @param isParmame Name of the parm
         * @param data Data associated with the decl (e.g. type, isArray, isStatic)
         */
        Parm(const unsigned lineNum, const std::string isParmame, Data *data);

        // Overridden
        std::string stringify() const override;
};

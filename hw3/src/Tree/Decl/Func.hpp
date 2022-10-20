#pragma once

#include "Decl.hpp"

class Func : public Decl
{
    public:
        /**
         * @param lineNum Line number of occurrence.
         * @param name Name of the function
         * @param data Data associated with the decl (e.g. type, isArray, isStatic)
         */
        Func(const unsigned lineNum, const std::string funcName, Data *data);

        // Overridden
        std::string stringify() const override;
};

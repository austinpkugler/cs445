#pragma once

#include "Exp.hpp"

class Range : public Exp
{
    public:
        /**
         * @param lineNum Line number of occurrence.
         */
        Range(const unsigned lineNum);

        // Overridden
        std::string stringify() const;
};

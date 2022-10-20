#pragma once

#include "Stmt.hpp"

class Break : public Stmt
{
    public:
        /**
         * @param lineNum Line number of occurrence.
         */
        Break(const unsigned lineNum);

        // Overridden
        std::string stringify() const override;
};

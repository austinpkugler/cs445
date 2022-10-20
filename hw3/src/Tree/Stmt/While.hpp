#pragma once

#include "Stmt.hpp"

class While : public Stmt
{
    public:
        /**
         * @param lineNum Line number of occurrence.
         */
        While(const unsigned lineNum);

        // Overridden
        std::string stringify() const override;
};

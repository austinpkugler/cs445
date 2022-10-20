#pragma once

#include "Stmt.hpp"

class Return : public Stmt
{
    public:
        /**
         * @param lineNum Line number of occurrence.
         */
        Return(const unsigned lineNum);

        // Overridden
        std::string stringify() const override;
};

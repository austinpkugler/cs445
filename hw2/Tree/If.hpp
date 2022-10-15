#pragma once

#include "Node.hpp"

class If : public Node
{
    public:
        /**
         * @param tokenLineNum Line number the if occurred in.
         */
        If(const unsigned tokenLineNum);

        std::string stringify() const;
};

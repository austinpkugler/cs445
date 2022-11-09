#pragma once

#include "Node.hpp"

class Range : public Node
{
    public:
        /**
         * @param tokenLineNum Line number the range occurred in.
         */
        Range(const unsigned tokenLineNum);

        std::string stringify() const;
};

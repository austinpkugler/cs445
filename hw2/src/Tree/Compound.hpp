#pragma once

#include "Node.hpp"

class Compound : public Node
{
    public:
        /**
         * @param tokenLineNum Line number the call occurred in.
         */
        Compound(const unsigned tokenLineNum);

        std::string stringify() const;
};

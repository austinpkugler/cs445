#pragma once

#include "Node.hpp"

class Call : public Node
{
    public:
        /**
         * @param tokenLineNum Line number the call occurred in.
         * @param value Name of the function called.
         */
        Call(const unsigned tokenLineNum, const std::string value);

        std::string stringify() const;
};

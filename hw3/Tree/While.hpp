#pragma once

#include "Node.hpp"

class While : public Node
{
    public:
        /**
         * @param tokenLineNum Line number the while occurred in.
         */
        While(const unsigned tokenLineNum);

        std::string stringify() const;
};

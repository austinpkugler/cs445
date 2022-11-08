#pragma once

#include "Node.hpp"

class Break : public Node
{
    public:
        /**
         * @param tokenLineNum Line number the break occurred in.
         */
        Break(const unsigned tokenLineNum);

        std::string stringify() const;
};

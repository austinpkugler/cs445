#pragma once

#include "Node.hpp"

class Return : public Node
{
    public:
        /**
         * @param tokenLineNum Line number the return occurred in.
         */
        Return(const unsigned tokenLineNum);

        std::string stringify() const;
};

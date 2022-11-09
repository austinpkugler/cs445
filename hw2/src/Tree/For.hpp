#pragma once

#include "Node.hpp"

class For : public Node
{
    public:
        /**
         * @param tokenLineNum Line number the for occurred in.
         */
        For(const unsigned tokenLineNum);

        std::string stringify() const;
};

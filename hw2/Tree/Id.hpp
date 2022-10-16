#pragma once

#include "Node.hpp"

class Id : public Node
{
    public:
        /**
         * @param tokenLineNum Line number the call occurred in.
         * @param idName Name of the Id.
         * @param isArray Whether the Id references an array.
         */
        Id(const unsigned tokenLineNum, const std::string idName, const bool isArray=false);
        std::string stringify() const;

    private:
        const bool m_isArray;
};

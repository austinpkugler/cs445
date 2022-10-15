#pragma once

#include "Node.hpp"

class Id : public Node
{
    public:
        /**
         * @param tokenLineNum Line number the call occurred in.
         * @param value Name of the Id.
         * @param isArray Whether the Id references an array.
         * @param arraySize Size of the array, -1 if not an array.
         */
        Id(const unsigned tokenLineNum, const std::string value, const bool isArray=false, const int arraySize=-1);
        std::string stringify() const;

    private:
        const bool m_isArray;
        const int m_arraySize;
};

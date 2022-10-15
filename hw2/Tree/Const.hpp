#pragma once

#include "Node.hpp"

class Const : public Node
{
    public:
        enum class Type { Int, Bool, Char, String };

        Const(unsigned tokenLineNum, const Type type, const std::string value);

        bool getCharLengthWarning() const { return m_charLengthWarning; }
        void printNode() const;

    private:
        const Type m_type;
        bool m_charLengthWarning = false;
};

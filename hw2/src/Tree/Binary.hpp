#pragma once

#include "Node.hpp"

class Binary : public Node
{
    public:
        enum class Type { Mul, Div, Mod, Add, Sub, Index, And, Or, LT, LEQ, GT, GEQ, EQ, NEQ };

        /**
         * @param tokenLineNum Line number the binary operation occurred in.
         * @param type Datatype of the binary operation.
         */
        Binary(const unsigned tokenLineNum, const Type type);

        std::string stringify() const;

    private:
        const Type m_type;
};

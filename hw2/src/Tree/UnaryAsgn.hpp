#pragma once

#include "Node.hpp"

class UnaryAsgn : public Node
{
    public:
        enum class Type { Inc, Dec };

        /**
         * @param tokenLineNum Line number the unary assign operation occurred in.
         * @param type Datatype of the unary assign operation.
         */
        UnaryAsgn(const unsigned tokenLineNum, const Type type);

        std::string stringify() const;

    private:
        const Type m_type;
};

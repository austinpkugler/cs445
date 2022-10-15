#pragma once

#include "Node.hpp"

class Unary : public Node
{
    public:
        enum class Type { Chsign, Sizeof, Question };

        /**
         * @param tokenLineNum Line number the unary operation occurred in.
         * @param type Datatype of the unary operation.
         */
        Unary(const unsigned tokenLineNum, const Type type);

        std::string stringify() const;

    private:
        const Type m_type;
};

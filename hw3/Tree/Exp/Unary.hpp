#pragma once

#include "../Node.hpp"

class Unary : public Node
{
    public:
        enum class Type { Chsign, Sizeof, Question, Not };

        /**
         * @param tokenLineNum Line number the unary operation occurred in.
         * @param type Datatype of the unary operation.
         */
        Unary(const unsigned tokenLineNum, const Type type);

        // Overridden
        NodeKind getNodeKind() const { return NodeKind::Exp; }
        ExpKind getExpKind() const { return ExpKind::Unary; }
        std::string stringify() const;

    private:
        const Type m_type;
};

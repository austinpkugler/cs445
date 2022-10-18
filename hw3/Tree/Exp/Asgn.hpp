#pragma once

#include "../Node.hpp"

class Asgn : public Node
{
    public:
        enum class Type { Asgn, AddAsgn, SubAsgn, DivAsgn, MulAsgn };

        /**
         * @param tokenLineNum Line number the assign operation occurred in.
         * @param type Datatype of the assign operation.
         */
        Asgn(const unsigned tokenLineNum, const Type type);

        // Overridden
        NodeKind getNodeKind() const { return NodeKind::Exp; }
        ExpKind getExpKind() const { return ExpKind::Asgn; }
        std::string stringify() const;

    private:
        const Type m_type;
};

#pragma once

#include "../Node.hpp"
#include "../Data.hpp"

class Exp : public Node
{
    public:
        // Enums
        enum class Kind { Asgn, Binary, Call, Const, Id, Range, Unary, UnaryAsgn };

        /**
         * @param lineNum Line number of occurrence.
         * @param expKind The category of exp node.
         */
        Exp(const unsigned lineNum, const Exp::Kind expKind);

        // Getters
        Exp::Kind getExpKind() const { return m_expKind; }

    private:
        const Exp::Kind m_expKind;
};

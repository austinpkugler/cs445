#pragma once

#include "../Node.hpp"
#include "../Prim.hpp"

class Func : public Node
{
    public:
        /**
         * @param tokenLineNum Line number the function occurred in.
         * @param type Return type of the function.
         * @param funcName Name of the function.
         */
        Func(const unsigned tokenLineNum, Prim *type, const std::string funcName);

        // Overridden
        NodeKind getNodeKind() const { return NodeKind::Decl; }
        DeclKind getDeclKind() const { return DeclKind::Func; }
        std::string stringify() const;

        Prim::Type getPrimType() { return m_type->getType(); }

    private:
        const Prim *m_type;
};

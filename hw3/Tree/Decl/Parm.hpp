#pragma once

#include "../Node.hpp"
#include "../Prim.hpp"

class Parm : public Node
{
    public:
        /**
         * @param tokenLineNum Line number the parm occurred in.
         * @param type Datatype of the parm.
         * @param parmName Name of the parm.
         */
        Parm(const unsigned tokenLineNum, Prim *type, const std::string parmName);

        // Overridden
        NodeKind getNodeKind() const { return NodeKind::Decl; }
        DeclKind getDeclKind() const { return DeclKind::Parm; }
        std::string stringify() const;

        void setType(Prim::Type type);

    private:
        Prim *m_type;
};

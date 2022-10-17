#pragma once

#include "Node.hpp"
#include "Primitive.hpp"

class Parm : public Node
{
    public:
        /**
         * @param tokenLineNum Line number the parm occurred in.
         * @param type Datatype of the parm.
         * @param parmName Name of the parm.
         */
        Parm(const unsigned tokenLineNum, Primitive *type, const std::string parmName);

        // Overridden
        NodeKind getNodeKind() const { return NodeKind::Decl; }
        DeclKind getDeclKind() const { return DeclKind::Parm; }
        std::string stringify() const;

        void setType(Primitive::Type type);

    private:
        Primitive *m_type;
};

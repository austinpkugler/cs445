#pragma once

#include "Node.hpp"
#include "Primitive.hpp"

class Var : public Node
{
    public:
        /**
         * @param tokenLineNum Line number the variable occurred in.
         * @param type Primitive type of the variable.
         * @param varName Name of the variable.
         * @param isStatic Whether the variable is static.
         */
        Var(const unsigned tokenLineNum, Primitive *type, const std::string varName);

        // Overridden
        NodeKind getNodeKind() const { return NodeKind::Decl; }
        DeclKind getDeclKind() const { return DeclKind::Var; }
        std::string stringify() const;

        bool getIsStatic() const;
        bool getIsArray() const;
        void setType(const Primitive::Type type);
        void makeInitialized();
        void makeStatic();

    private:
        Primitive *m_type;
        bool m_initialized = false;
};

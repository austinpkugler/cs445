#pragma once

#include "Node.hpp"
#include "Primitive.hpp"

class Var : public Node
{
    public:
        /**
         * @param tokenLineNum Line number the variable occurred in.
         * @param type Primitive type of the variable.
         * @param value Name of the variable.
         * @param isStatic Whether the variable is static.
         */
        Var(const unsigned tokenLineNum, Primitive *type, const std::string value, const bool isStatic=false);

        void setType(const Primitive::Type type);
        void makeStatic();
        void printNode() const;
        std::string stringify() const;

    private:
        Primitive *m_type;
        bool m_isStatic;
};

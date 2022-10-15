#pragma once

#include "Node.hpp"
#include "Primitive.hpp"

class Parm : public Node
{
    public:
        /**
         * @param tokenLineNum Line number the parm occurred in.
         * @param type Datatype of the parm.
         * @param value Name of the parm.
         */
        Parm(const unsigned tokenLineNum, const Primitive::Type type, const std::string value);

        void printNode() const;
        std::string stringify() const;

    private:
        const Primitive m_type;
};

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
        Parm(const unsigned tokenLineNum, Primitive *type, const std::string value);

        void setType(Primitive::Type type);
        void printNode() const;
        std::string stringify() const;

    private:
        Primitive *m_type;
};

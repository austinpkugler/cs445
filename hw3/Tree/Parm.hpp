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

        void setType(Primitive::Type type);
        std::string stringify() const;

    private:
        Primitive *m_type;
};

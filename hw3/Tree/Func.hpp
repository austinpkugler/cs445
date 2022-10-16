#pragma once

#include "Node.hpp"
#include "Primitive.hpp"

class Func : public Node
{
    public:
        /**
         * @param tokenLineNum Line number the function occurred in.
         * @param type Return type of the function.
         * @param funcName Name of the function.
         */
        Func(const unsigned tokenLineNum, Primitive *type, const std::string funcName);

        std::string stringify() const;

    private:
        const Primitive *m_type;
};

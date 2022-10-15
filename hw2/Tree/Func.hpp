#pragma once

#include "Node.hpp"
#include "Primitive.hpp"

class Func : public Node
{
    public:
        /**
         * @param tokenLineNum Line number the function occurred in.
         * @param type Return type of the function.
         * @param value Name of the function.
         */
        Func(const unsigned tokenLineNum, const Primitive::Type type, const std::string value);

        void printNode() const;
        std::string stringify() const;

    private:
        const Primitive m_type;
};

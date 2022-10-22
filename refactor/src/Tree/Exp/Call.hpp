#pragma once

#include "Exp.hpp"

class Call : public Exp
{
    public:
        Call(const int lineNum, const std::string funcName);

        // Overridden
        Node::Kind getNodeKind() const override { return Node::Kind::Call; }
        std::string stringify() const override;

        // Getters
        std::string getName() const { return m_name; }

    private:
        const std::string m_name;
};
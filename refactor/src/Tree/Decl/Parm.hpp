#pragma once

#include "Decl.hpp"

class Parm : public Decl
{
    public:
        Parm(const int lineNum, const std::string parmName, Data *data);

        // Overridden
        Node::Kind getNodeKind() const override { return Node::Kind::Parm; }
        std::string stringify() const override;

        // Getters
        bool getIsUsed() const { return m_isUsed; }

        // Setters
        void makeUsed() { m_isUsed = true; }

    private:
        bool m_isUsed;
};

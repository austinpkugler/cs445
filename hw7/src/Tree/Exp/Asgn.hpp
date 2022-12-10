#pragma once

#include "Exp.hpp"

class Asgn : public Exp
{
    public:
        enum class Type { Asgn, AddAsgn, SubAsgn, DivAsgn, MulAsgn };

        Asgn(const int lineNum, const Asgn::Type type);

        // Overridden
        Node::Kind getNodeKind() const override { return Node::Kind::Asgn; }
        std::string stringify() const override;

        // Getters
        Asgn::Type getType() const { return m_type; }
        std::string getTypeString() const;
        std::string getSym() const;

    private:
        const Asgn::Type m_type;
};

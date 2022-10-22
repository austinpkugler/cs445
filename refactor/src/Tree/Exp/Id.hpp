#pragma once

#include "Exp.hpp"

class Id : public Exp
{
    public:
        Id(const int lineNum, const std::string isIdame, const bool isArray=false);

        // Overridden
        Node::Kind getNodeKind() const override { return Node::Kind::Id; }
        std::string stringify() const override;
        std::string stringifyWithType() const override;

        // Getters
        std::string getName() const { return m_name; }
        bool getIsArray() const { return m_isArray; }

    private:
        const std::string m_name;
        const bool m_isArray;
};

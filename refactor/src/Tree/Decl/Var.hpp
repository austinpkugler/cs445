#pragma once

#include "Decl.hpp"

class Var : public Decl
{
    public:
        Var(const int lineNum, const std::string isVarame, Data *data);

        // Overridden
        Node::Kind getNodeKind() const override { return Node::Kind::Var; }
        std::string stringify() const override;

        // Getters
        bool getIsInitialized() const { return m_isInitialized; }

        // Setters
        void makeInitialized() { m_isInitialized = true; }
        void makeStatic();

    private:
        bool m_isInitialized;
};

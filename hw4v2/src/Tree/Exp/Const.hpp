#pragma once

#include "Exp.hpp"

class Const : public Exp
{
    public:
        enum class Type { Int, Bool, Char, String };

        Const(const int lineNum, const Const::Type type, const std::string value);

        // Overridden
        Node::Kind getNodeKind() const override { return Node::Kind::Const; }
        std::string stringify() const override;
        std::string stringifyWithType() const override;

        // Getters
        bool getCharLengthWarning() const { return m_charLengthWarning; }

    private:
        char parseFirstChar(const std::string &str) const;
        std::string removeFirstAndLastChar(const std::string &str) const;
        std::string parseChars(const std::string &str) const;

        const Const::Type m_type;
        bool m_charLengthWarning = false;
        int m_intValue;
        bool m_boolValue;
        char m_charValue;
        std::string m_stringValue;
};

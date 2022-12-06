#pragma once

#include "Exp.hpp"

class Const : public Exp
{
    public:
        enum class Type { Int, Bool, Char, String };

        Const(const int lineNum, const Const::Type type, const std::string value);

        // Static
        static std::string removeFirstAndLastChar(const std::string &str);

        // Overridden
        Node::Kind getNodeKind() const override { return Node::Kind::Const; }
        std::string stringify() const override;
        std::string stringifyWithType() const override;

        // Getters
        Const::Type getType() const { return m_type; }
        bool getCharLengthWarning() const { return m_charLengthWarning; }
        std::string getLongConstValue() const { return m_longConstValue; }
        int getIntValue() const { return m_intValue; }
        bool getBoolValue() const { return m_boolValue; }
        std::string getStringValue() const { return m_stringValue; }

    private:
        char parseFirstChar(const std::string &str) const;
        std::string parseChars(const std::string &str) const;

        const Const::Type m_type;
        bool m_charLengthWarning = false;
        int m_intValue;
        bool m_boolValue;
        char m_charValue;
        std::string m_stringValue;
        std::string m_longConstValue;
};

#pragma once

#include "../Node.hpp"

class Const : public Node
{
    public:
        enum class Type { Int, Bool, Char, String };

        /**
         * @param tokenLineNum Line number the constant occurred in.
         * @param type Datatype of the constant.
         * @param constValue Stringified value of the constant.
         */
        Const(const unsigned tokenLineNum, const Type type, const std::string constValue);

        // Overridden
        NodeKind getNodeKind() const { return NodeKind::Exp; }
        ExpKind getExpKind() const { return ExpKind::Const; }
        std::string stringify() const;

        bool getCharLengthWarning() const { return m_charLengthWarning; }

    private:
        char parseFirstChar(const std::string &str) const;
        std::string removeFirstAndLastChar(const std::string &str) const;
        std::string parseChars(const std::string &str) const;

        const Type m_type;
        bool m_charLengthWarning = false;
};

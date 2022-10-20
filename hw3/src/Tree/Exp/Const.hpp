#pragma once

#include "Exp.hpp"

class Const : public Exp
{
    public:
        // Enums
        enum class Type { Int, Bool, Char, String };

        /**
         * @param lineNum Line number of occurrence.
         * @param type Type of the const.
         * @param value Stringified value of the const
         */
        Const(const unsigned lineNum, const Const::Type type, const std::string value);

        // Overridden
        std::string stringify() const;

        // Getters
        bool getCharLengthWarning() const { return m_charLengthWarning; }

    private:
        // Helpers
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

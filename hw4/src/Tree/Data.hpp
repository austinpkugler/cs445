#pragma once

#include <iostream>

class Data
{
    public:
        enum class Type { Undefined, Int, Bool, Char, String, Void };

        Data(Data::Type type, bool isArray, bool isStatic);

        // Static
        static std::string typeToString(Data::Type type);

        // Getters
        bool getIsArray() const { return m_isArray; }
        bool getIsStatic() const { return m_isStatic; }
        Type getType() const { return m_type; }
        std::string getCopyOf() const { return m_copyOf; }
        Data::Type getNextType() const;
        Data * getNextData() const;

        // Setters
        void setIsArray(const bool isArray) { m_isArray = isArray; }
        void setIsStatic(const bool isStatic) { m_isStatic = isStatic; }
        void setType(const Data::Type type);
        void setCopyOf(const std::string copyOf);

        // Print
        std::string stringify() const;
        // std::string stringifyWithType() const;

    private:
        bool m_isArray = false;
        bool m_isStatic = false;
        Data::Type m_type;
        std::string m_copyOf;
        Data *m_next;
};

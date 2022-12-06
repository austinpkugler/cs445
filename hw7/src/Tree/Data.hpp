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
        int getArraySize() const { return m_arraySize; }
        Type getType() const { return m_type; }
        Data::Type getNextType() const;
        Data * getNextData() const;

        // Setters
        void setIsArray(const bool isArray) { m_isArray = isArray; }
        void setIsStatic(const bool isStatic) { m_isStatic = isStatic; }
        void setArraySize(const int arraySize) { m_arraySize = arraySize; }
        void setType(const Data::Type type);
        void setCopyOf(const std::string copyOf);

        // Print
        std::string stringify() const;

    private:
        bool m_isArray;
        bool m_isStatic;
        int m_arraySize;
        Data::Type m_type;
        Data *m_next;
};

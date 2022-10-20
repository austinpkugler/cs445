#pragma once

#include <iostream>

class Data
{
    public:
        // Enums
        enum class Type { None, Int, Bool, Char, String, Void };

        /**
         * @param type Primitive datatype of the data.
         * @param isArray Whether the data is an array.
         * @param isStatic Whether the data is static.
         */
        Data(Data::Type type, bool isArray, bool isStatic);

        // Static
        static std::string stringifyType(Data::Type type);

        // Getters
        bool getIsArray() const { return m_isArray; }
        bool getIsStatic() const { return m_isStatic; }
        Type getType() const { return m_type; }
        std::string getCopyOf() const { return m_copyOf; }

        // Setters
        void setIsArray(bool isArray);
        void setIsStatic(bool isStatic);
        void setType(Data::Type type);
        void setCopyOf(std::string copyOf);

        // Print
        std::string stringify() const;

    private:
        bool m_isArray = false;
        bool m_isStatic = false;
        Data::Type m_type;
        std::string m_copyOf;
        Data *m_next;
};

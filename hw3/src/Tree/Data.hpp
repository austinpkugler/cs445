#pragma once

#include <iostream>

class Data
{
    public:
        // Enums
        enum class Type { None, Int, Bool, Char, String };

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

        // Setters
        void setIsArray(bool isArray);
        void setIsStatic(bool isStatic);
        void setType(Data::Type type);

        // Print
        std::string stringify() const;

    private:
        bool m_isArray = false;
        bool m_isStatic = false;
        Data::Type m_type;
        Data *m_next;
};

#pragma once

#include <iostream>

class Primitive
{
    public:
        enum class Type { Int, Bool, Char, String, Void };

        /**
         * @param type Type of primitive.
         * @param isArray Whether the primitive is an array.
         * @param isStatic Whether the primitive is static.
         */
        Primitive(Type type, const bool isArray=false);

        bool getIsArray() const { return m_isArray; }
        Type getType() const { return m_type; }
        void setType(Type type);
        std::string stringify() const;

    private:
        const bool m_isArray;
        Type m_type;
        Primitive *m_next;
};

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
        Primitive(Type type, const bool isArray=false, const bool isStatic=false);
        Primitive(Primitive *type, const bool isArray=false, const bool isStatic=false);

        Type getType() const { return m_type; }
        void setType(const Type type);
        void printNode() const;
        std::string stringify() const;

    private:
        const bool m_isArray;
        const bool m_isStatic;
        Type m_type;
        Primitive *m_next;
};

#pragma once

#include <iostream>

class Prim
{
    public:
        enum class Type { Int, Bool, Char, String, Void };

        /**
         * @param type Type of primitive.
         * @param isArray Whether the primitive is an array.
         * @param isStatic Whether the primitive is static.
         */
        Prim(Type type, bool isArray, bool isStatic);

        bool getIsArray() const { return m_isArray; }
        bool getIsStatic() const { return m_isStatic; }
        Type getType() const { return m_type; }

        void setIsArray(bool isArray);
        void setIsStatic(bool isStatic);
        void setType(Type type);

        std::string stringify() const;

    private:
        bool m_isArray = false;
        bool m_isStatic = false;
        Type m_type;
        Prim *m_next;
};

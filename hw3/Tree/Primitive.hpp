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
        Primitive(Type type, bool isArray, bool isStatic, bool isParm);

        bool getIsArray() const { return m_isArray; }
        bool getIsStatic() const { return m_isStatic; }
        bool getIsParm() const { return m_isParm; }
        Type getType() const { return m_type; }

        void setIsArray(bool isArray);
        void setIsStatic(bool isStatic);
        void setIsParm(bool isParm);
        void setType(Type type);

        std::string stringify() const;

    private:
        bool m_isArray = false;
        bool m_isStatic = false;
        bool m_isParm = false;
        Type m_type;
        Primitive *m_next;
};

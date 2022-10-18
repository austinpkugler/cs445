#pragma once

#include "Exp.hpp"

class Id : public Exp
{
    public:
        /**
         * @param lineNum Line number of occurrence.
         * @param idName Name of the id.
         * @param isArray Whether the id references an array. False by default.
         */
        Id(const unsigned lineNum, const std::string idName, const bool isArray=false);

        // Overridden
        std::string stringify() const;

        // Getters
        std::string getName() const { return m_name; }
        bool getIsArray() const { return m_isArray; }

    private:
        const std::string m_name;
        const bool m_isArray;
};

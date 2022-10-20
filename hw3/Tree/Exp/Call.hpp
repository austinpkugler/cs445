#pragma once

#include "Exp.hpp"

class Call : public Exp
{
    public:
        /**
         * @param lineNum Line number of occurrence.
         * @param isFuncame Name of the function being called.
         */
        Call(const unsigned lineNum, const std::string isFuncame);

        // Overridden
        std::string stringify() const;

        // Getters
        std::string getName() const { return m_name; }

    private:
        const std::string m_name;
};

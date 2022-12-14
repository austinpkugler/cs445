#pragma once

#include "Decl.hpp"

class Var : public Decl
{
    public:
        /**
         * @param lineNum Line number of occurrence.
         * @param isVarame Name of the var
         * @param data Data associated with the decl (e.g. type, isArray, isStatic)
         */
        Var(const unsigned lineNum, const std::string isVarame, Data *data);

        // Overridden
        std::string stringify() const override;

        // Getters
        bool getIsInitialized() const { return m_isInitialized; }
        bool getIsUsed() const { return m_isUsed; }

        // Setters
        void makeInitialized();
        void makeUsed();
        void makeStatic();

    private:
        bool m_isInitialized = false;
        bool m_isUsed = false;
};

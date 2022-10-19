#pragma once

#include "Decl.hpp"

class Var : public Decl
{
    public:
        /**
         * @param lineNum Line number of occurrence.
         * @param varName Name of the var
         * @param data Data associated with the decl (e.g. type, isArray, isStatic)
         */
        Var(const unsigned lineNum, const std::string varName, Data *data);

        // Overridden
        std::string stringify() const;

        // Getters
        bool getShowWarns() const { return m_showWarns; }
        bool getIsInitialized() const { return m_isInitialized; }
        bool getIsUsed() const { return m_isUsed; }

        // Setters
        void setShowWarns(bool showWarns);
        void makeInitialized();
        void makeUsed();
        void makeStatic();

    private:
        bool m_showWarns = true;
        bool m_isInitialized = false;
        bool m_isUsed = false;
};

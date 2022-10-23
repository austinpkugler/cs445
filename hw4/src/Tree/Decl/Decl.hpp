#pragma once

#include "../Data.hpp"
#include "../Node.hpp"

class Decl : public Node
{
    public:
        Decl(const int lineNum, const std::string name, Data *data);

        // Getters
        std::string getName() const { return m_name; }
        Data * getData() const { return m_data; }
        bool getShowErrors() const { return m_showErrors; }
        bool getIsUsed() const { return m_isUsed; }

        // Setters
        void setShowErrors(const bool showErrors) { m_showErrors = showErrors; }
        void setType(const Data::Type type);
        void makeUsed() { m_isUsed = true; }

    protected:
        const std::string m_name;
        Data *m_data;

    private:
        bool m_showErrors;
        bool m_isUsed;
};

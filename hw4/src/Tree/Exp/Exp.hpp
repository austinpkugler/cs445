#pragma once

#include "../Data.hpp"
#include "../Node.hpp"

class Exp : public Node
{
    public:
        Exp(const int lineNum, Data *m_data);

        // Getters
        Data * getData() const { return m_data; }

        // Setters
        void setData(Data *data) { m_data = data; }
    
        // Overridden
        std::string stringifyWithType() const override;

    protected:
        Data *m_data;
};

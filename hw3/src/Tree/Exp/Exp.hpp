#pragma once

#include "../Node.hpp"
#include "../Data.hpp"

class Exp : public Node
{
    public:
        // Enums
        enum class Kind { Asgn, Binary, Call, Const, Id, Unary, UnaryAsgn };

        /**
         * @param lineNum Line number of occurrence.
         * @param expKind The category of exp node.
         */
        Exp(const unsigned lineNum, const Exp::Kind expKind, Data *m_data);

        // Getters
        Exp::Kind getExpKind() const { return m_expKind; }
        Data * getData() const { return m_data; }

        // Setters
        void setData(Data *data);

        // Virtual
        virtual std::string stringifyWithType() const;

    protected:
        Data *m_data;

    private:
        const Exp::Kind m_expKind;
};

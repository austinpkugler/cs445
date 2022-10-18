#pragma once

#include "../Node.hpp"
#include "../Data.hpp"

class Decl : public Node
{
    public:
        // Enums
        enum class Kind { Func, Parm, Var };

        /**
         * @param lineNum Line number of occurrence.
         * @param declKind The category of decl node.
         * @param name Name of the declaration (e.g. name of func, var, or parm)
         * @param data Data associated with the decl (e.g. type, isArray, isStatic)
         */
        Decl(const unsigned lineNum, const Decl::Kind declKind, const std::string name, Data *data);

        // Getters
        Decl::Kind getDeclKind() const { return m_declKind; }
        std::string getName() const { return m_name; }
        Data * getData() const { return m_data; }

        // Setters
        void setType(const Data::Type type);

    protected:
        const std::string m_name;
        Data *m_data;

    private:
        const Decl::Kind m_declKind;
};

#pragma once
#include "Ast.h"
#include "Cells.h"

namespace infocell {
namespace cells {

class ToolFinder;
class World;
class Library : public Object
{
public:
    Library(World& w, const std::string& name);
    Library(World& w, Ast::Scope& scope, const std::string& name);

    void include(Library& library);
    void mergeTo(Library& target);

    Ast::Scope& scope();
    TrieMap& functions();
    TrieMap& structs();
    TrieMap& variables();

    Object& getFunction(const std::string& nameStr);
    Object& getFunction(CellI& name);
    CellI& getStruct(const std::string& nameStr);
    CellI& getStruct(CellI& name);
    CellI& getVariable(const std::string& nameStr);
    CellI& getVariable(CellI& name);

    ToolFinder& toolFinder();

    ToolFinder& m_toolFinder;
};

} // namespace cells
} // namespace infocell

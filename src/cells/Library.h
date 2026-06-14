#pragma once
#include "World.h"
#include "ToolFinder.h"

namespace infocell {
namespace cells {

class Library : public Object
{
public:
    Library(World& w);
    Library(World& w, Ast::Scope& scope);

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
};

} // namespace cells
} // namespace infocell

#include "Library.h"

namespace infocell {
namespace cells {

Library::Library(World& w, Ast::Scope& scope) :
    Library(w)
{
    set(w.ids.scope, scope);
}

Library::Library(World& w) :
    Object(w, w.std.Library, "library")
{
    set(w.ids.functions, *new TrieMap(w, w.std.Cell, w.std.op.Function, "Functions"));
    set(w.ids.structs, *new TrieMap(w, w.std.Cell, w.std.Struct, "Structs"));
    set(w.ids.variables, *new TrieMap(w, w.std.Cell, w.std.op.Var, "Variables"));
}

void Library::include(Library& library)
{
    if (&library.scope() != &scope()) {
        library.scope().mergeTo(scope(), Ast::Scope::MergeMode::Link);
    }
    library.mergeTo(*this);
}

void Library::mergeTo(Library& target)
{
    Visitor::visitList(functions()[w.ids.list], [this, &target](CellI& kvPair, int i, bool&) {
        CellI& key   = kvPair[w.ids.key];
        CellI& value = kvPair[w.ids.value];
        target.functions().add(key, value);
    });
    Visitor::visitList(structs()[w.ids.list], [this, &target](CellI& kvPair, int i, bool&) {
        CellI& key   = kvPair[w.ids.key];
        CellI& value = kvPair[w.ids.value];
        target.structs().add(key, value);
    });
    Visitor::visitList(variables()[w.ids.list], [this, &target](CellI& kvPair, int i, bool&) {
        CellI& key   = kvPair[w.ids.key];
        CellI& value = kvPair[w.ids.value];
        target.variables().add(key, value);
    });
}

Ast::Scope& Library::scope()
{
    return static_cast<Ast::Scope&>(get(w.ids.scope));
}

TrieMap& Library::functions()
{
    return static_cast<TrieMap&>(get(w.ids.functions));
}

TrieMap& Library::structs()
{
    return static_cast<TrieMap&>(get(w.ids.structs));
}

TrieMap& Library::variables()
{
    return static_cast<TrieMap&>(get(w.ids.variables));
}

Object& Library::getFunction(const std::string& nameStr)
{
    return getFunction(w.name(nameStr));
}

Object& Library::getFunction(CellI& name)
{
    return static_cast<Object&>(functions().getValue(name));
}

CellI& Library::getStruct(const std::string& nameStr)
{
    return getStruct(w.name(nameStr));
}

CellI& Library::getStruct(CellI& name)
{
    return structs().getValue(name);
}

CellI& Library::getVariable(const std::string& nameStr)
{
    return getVariable(w.name(nameStr));
}

CellI& Library::getVariable(CellI& name)
{
    return variables().getValue(name);
}
} // namespace cells
} // namespace infocell
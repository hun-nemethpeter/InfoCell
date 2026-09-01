#include "Library.h"
#include "World.h"

namespace infocell {
namespace cells {

Library::Library(World& w, Ast::Scope& scope, const std::string& name) :
    Library(w, name)
{
    set(w.id.scope, scope);
}

Library::Library(World& w, const std::string& name) :
    Object(w, w.std.Library, "library"),
    m_toolFinder(*new ToolFinder(w))
{
    set(w.id.name, w.name(name));
    set(w.id.functions, *new TrieMap(w, w.std.Cell, w.std.op.Function, "Functions"));
    set(w.id.structs, *new TrieMap(w, w.std.Cell, w.std.Struct, "Structs"));
    set(w.id.variables, *new TrieMap(w, w.std.Cell, w.std.op.Var, "Variables"));
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
    for (CellI& kvPair : functions()) {
        CellI& key   = kvPair[w.id.key];
        CellI& value = kvPair[w.id.value];
        target.functions().add(key, value);
    }
    for (CellI& kvPair : structs()) {
        CellI& key   = kvPair[w.id.key];
        CellI& value = kvPair[w.id.value];
        target.structs().add(key, value);
    }
    for (CellI& kvPair : variables()) {
        CellI& key   = kvPair[w.id.key];
        CellI& value = kvPair[w.id.value];
        target.variables().add(key, value);
    }
}

Ast::Scope& Library::scope()
{
    return static_cast<Ast::Scope&>(get(w.id.scope));
}

TrieMap& Library::functions()
{
    return static_cast<TrieMap&>(get(w.id.functions));
}

TrieMap& Library::structs()
{
    return static_cast<TrieMap&>(get(w.id.structs));
}

TrieMap& Library::variables()
{
    return static_cast<TrieMap&>(get(w.id.variables));
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

ToolFinder& Library::toolFinder()
{
    return m_toolFinder;
}

} // namespace cells
} // namespace infocell
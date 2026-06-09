#pragma once
#include "Ast.h"
#include <fmt/core.h>

namespace infocell {
namespace cells {

template <typename... Args>
Ast::SubType& AstHelper::st_(const std::string& nameStr, Args&&... args)
{
    return subType(name(nameStr), std::forward<Args>(args)...);
}

template <typename... Args>
Ast::TemplatedType& AstHelper::tt_(const std::string& nameStr, Args&&... args)
{
    return templatedType(nameStr, std::forward<Args>(args)...);
}


template <typename... Args>
List& AstHelper::list(CellI& value, Args&&... args)
{
    List& ret = *new List(kb, value.struct_());
    ret.add(value);
    if constexpr (sizeof...(Args) > 0) {
        ret.add(std::forward<Args>(args)...);
    }

    return ret;
}

template <typename... Args>
Map& AstHelper::map(CellI& key, CellI& value, Args&&... args)
{
    Map& ret = *new Map(kb, key.struct_(), value.struct_(), fmt::format("Map<{}, {}>(...)", key.struct_().label(), value.struct_().label()));
    if constexpr (sizeof...(Args) > 0) {
        ret.add(std::forward<Args>(args)...);
    }

    return ret;
}
#pragma region Ast
template <typename... Args>
Ast::Block& Ast::block(Args&&... args)
{
    return *new Block(kb, kb.list(std::forward<Args>(args)...));
}

template <typename... Args>
Ast::TemplatedType& Ast::templatedType(const std::string& id, const std::string& key, CellI& type, Args&&... args)
{
    auto& ret = templatedType(id, kb.ast.slot(key, type));
    if constexpr (sizeof...(Args) > 0) {
        ret.addParam(std::forward<Args>(args)...);
    }
    return ret;
}

template <typename... Args>
Ast::TemplatedType& Ast::templatedType(const std::string& id, const std::string& key, const std::string& type, Args&&... args)
{
    auto& ret  = templatedType(id, kb.ast.slot(key, kb.ast.structName(type)));
    if constexpr (sizeof...(Args) > 0) {
        ret.addParam(std::forward<Args>(args)...);
    }
    return ret;
}

template <typename... Args>
Ast::StructBase& Ast::StructBase::description(Args&&... args)
{
    addBlock(*new Block(kb, kb.list(std::forward<Args>(args)...)));
    return *this;
}

template <typename... Args>
void Ast::Function::instructions(Args&&... args)
{
    addBlock(*new Block(kb, kb.list(std::forward<Args>(args)...)));
}
#pragma endregion

template <class MapType, class TAst>
Ast::Items<MapType, TAst>::Items(Brain& kb, const std::string& mapName, Base& parent) :
    kb(kb),
    m_mapName(mapName),
    m_parent(parent)
{
}

template <class MapType, class TAst>
bool Ast::Items<MapType, TAst>::has(CellI& id)
{
    if (m_parent.missing(m_mapName)) {
        return false;
    }

    return items().hasKey(id);
}

template <class MapType, class TAst>
TAst& Ast::Items<MapType, TAst>::get(const std::string& nameStr)
{
    return get(kb.name(nameStr));
}

template <class MapType, class TAst>
TAst& Ast::Items<MapType, TAst>::get(CellI& name)
{
    if (m_parent.missing(m_mapName)) {
        throw "No such scope";
    }

    if (items().hasKey(name)) {
        return static_cast<TAst&>(items().getValue(name));
    }

    throw "No such scope";
}

template <class MapType, class TAst>
TAst& Ast::Items<MapType, TAst>::add(const std::string& nameStr)
{
    TAst& ast = *new TAst(kb, nameStr);
    add(ast);

    return ast;
}

template <class MapType, class TAst>
void Ast::Items<MapType, TAst>::add(TAst& obj)
{
    CellI& name = obj[kb.ids.name];

    if (m_parent.missing(m_mapName)) {
        m_parent.set(m_mapName, *new TrieMap(kb, kb.std.Cell, kb.std.ast.Base, "TrieMap<Cell, Type::Ast::Base>(...)"));
    }
    if (items().hasKey(name)) {
        throw "Already registered!";
    }

    items().add(name, obj);
    obj.set("scope", m_parent);
}

template <class MapType, class TAst>
MapType& Ast::Items<MapType, TAst>::items()
{
    if (m_parent.missing(m_mapName)) {
        throw fmt::format("No {}!", m_mapName);
    } else {
        return static_cast<TrieMap&>(m_parent.get(m_mapName));
    }
}

} // namespace cells
} // namespace infocell

#pragma once
#include <fmt/core.h>
#include <functional>

#include "Cells.h"

namespace infocell {
namespace cells {

class World;

// ============================================================================
class Op
{
public:
    template <class T>
    using NewT = util::NewT<T>;

    class Base : public Object
    {
        Base(const Base&) = delete;
    protected:
        Base(World& w, CellI& classCell, const std::string& label = "");
    };

    template <typename T>
    class BaseT : public Base,
                  public NewT<T>
    {
    protected:
        BaseT(World& w, CellI& classCell, const std::string& label) :
            Base(w, classCell, label)
        {
        }
    };

    // ============================================================================
    class Add : public BaseT<Add>
    {
    public:
        Add(World& w, CellI& lhs, CellI& rhs);
    };

    class Call : public BaseT<Call>
    {
    public:
        Call(World& w, CellI& self, CellI& method);
        Call& operator()(CellI& name, CellI& value);
    };

    class ConstVar : public BaseT<ConstVar>
    {
    public:
        ConstVar(World& w, CellI& value);
    };

    class Equal : public BaseT<Equal>
    {
    public:
        Equal(World& w, CellI& lhs, CellI& rhs);
    };

    class Get : public BaseT<Get>
    {
    public:
        Get(World& w, CellI& cell, CellI& key);
        Call& operator/(Base& key);
    };

    class Member : public BaseT<Member>
    {
    public:
        Member(World& w, CellI& name, CellI& type);
    };

    class Parameter : public BaseT<Parameter>
    {
    public:
        Parameter(World& w, CellI& name, CellI& type);
    };

    class UnknownVar : public BaseT<UnknownVar>
    {
    public:
        UnknownVar(World& w, CellI& value);
    };

    // ============================================================================
    Op(World& w);

    Add& add(Base& lhs, Base& rhs);
    Call& call(CellI& self, CellI& method);
    Equal& equal(CellI& lhs, Base& rhs);
    Get& get(Base& cell, Base& key);

    ConstVar& const_(CellI& value);
    ConstVar& const_(const std::string& id);
    ConstVar& const_(int number);
    Member& member(CellI& name, CellI& type);
    Parameter& parameter(CellI& name, CellI& type);
    UnknownVar& unknown_(CellI& value);

    template <typename... Args>
    Map& parameters(CellI& name, CellI& value, Args&&... args);

protected:
    template <typename... Args>
    void addParameter(Map& map, CellI& name, CellI& value, Args&&... args);

    World& w;
};

} // namespace cells
} // namespace infocell

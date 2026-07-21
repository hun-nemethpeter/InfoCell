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
    class ConstVar : public BaseT<ConstVar>
    {
    public:
        ConstVar(World& w, CellI& value);
    };

    class Var : public BaseT<Var>
    {
    public:
        Var(World& w);
        Var(World& w, CellI& value);
    };

    // ============================================================================
    Op(World& w);

    ConstVar& const_(CellI& value);
    ConstVar& const_(const std::string& id);
    ConstVar& const_(int number);
    Var& var_();
    Var& var_(CellI& value);


protected:
    World& w;
};

} // namespace cells
} // namespace infocell

#include "Op.h"
#include "World.h"

#define SPDLOG_ACTIVE_LEVEL SPDLOG_LEVEL_TRACE
#include "util/Log.h"

namespace infocell {
namespace cells {

// ============================================================================
Op::Base::Base(World& w, CellI& classCell, const std::string& label) :
    Object(w, classCell, label)
{
}

// ============================================================================
Op::Add::Add(World& w, CellI& lhs, CellI& rhs) :
    BaseT<Add>(w, w.std.op.Add, "op.Add")
{
    set(w.id.lhs, lhs);
    set(w.id.rhs, rhs);
}

Op::Call::Call(World& w, CellI& self, CellI& method) :
    BaseT<Call>(w, w.std.op.Call, "op.Call")
{
    set(w.id.method, method);
    set(w.id.parameters, w.op.parameters(w.id.self, self));
    set(w.id.state, w.std.op.State.ready);
}

Op::Call& Op::Call::operator()(CellI& name, CellI& value)
{
    if (missing(w.id.parameters)) {
        set(w.id.parameters, w.op.parameters(name, value));
        return *this;
    }
    Map& parameters = static_cast<Map&>(get(w.id.parameters));
    w.op.addParameter(parameters, name, value);

    return *this;
}

Op::ConstVar::ConstVar(World& w, CellI& value) :
    BaseT<ConstVar>(w, w.std.op.ConstVar, "op.ConstVar")
{
    set(w.id.state, w.std.op.State.ready);
    set(w.id.value, value);
}

Op::Equal::Equal(World& w, CellI& lhs, CellI& rhs) :
    BaseT<Equal>(w, w.std.op.Equal, "op.Equal")
{
    set(w.id.lhs, lhs);
    set(w.id.rhs, rhs);
}

Op::Get::Get(World& w, CellI& cell, CellI& key) :
    BaseT<Get>(w, w.std.op.Get, "op.Get")
{
    set(w.id.cell, cell);
    set(w.id.key, key);
}

Op::Member::Member(World& w, CellI& name, CellI& type) :
    BaseT<Member>(w, w.std.op.Member, "op.Member")
{
    set(w.id.name, name);
    set(w.id.type, type);
}

Op::Parameter::Parameter(World& w, CellI& name, CellI& type) :
    BaseT<Parameter>(w, w.std.op.Parameter, "op.Parameter")
{
    set(w.id.name, name);
    set(w.id.type, type);
    set(w.id.state, w.std.op.State.missingInput);
}

Op::UnknownVar::UnknownVar(World& w, CellI& value) :
    BaseT<UnknownVar>(w, w.std.op.UnknownVar, "op.UnknownVar")
{
    set(w.id.state, w.std.op.State.ready);
    set(w.id.value, value);
}

// ============================================================================
Op::Op(World& w) :
    w(w)
{
}

Op::Add& Op::add(Base& lhs, Base& rhs)
{
    return Add::New(w, lhs, rhs);
}

Op::Call& Op::call(CellI& self, CellI& method)
{
    return Call::New(w, self, method);
}

Op::Equal& Op::equal(CellI& lhs, CellI& rhs)
{
    return Equal::New(w, lhs, rhs);
}

Op::Get& Op::get(Base& cell, Base& key)
{
    return Get::New(w, cell, key);
}

Op::ConstVar& Op::const_(CellI& value)
{
    return ConstVar::New(w, value);
}

Op::ConstVar& Op::const_(const std::string& nameStr)
{
    return const_(w.name(nameStr));
}

Op::ConstVar& Op::const_(int number)
{
    return const_(w.pools.numbers.get(number));
}

Op::Member& Op::member(CellI& name, CellI& type)
{
    return Member::New(w, name, type);
}

Op::Parameter& Op::parameter(CellI& name, CellI& type)
{
    return Parameter::New(w, name, type);
}

Op::UnknownVar& Op::unknown_(CellI& value)
{
    return UnknownVar::New(w, value);
}

} // namespace cells
} // namespace infocell

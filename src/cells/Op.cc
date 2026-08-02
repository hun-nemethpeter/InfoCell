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
Op::Call::Call(World& w, CellI& self, CellI& method) :
    BaseT<Call>(w, w.std.op.Call, "op.Call")
{
    set(w.id.self, self);
    set(w.id.method, method);
    set(w.id.state, w.std.op.State.start);
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
    set(w.id.state, w.std.op.State.start);
    set(w.id.value, value);
}

Op::UnknownVar::UnknownVar(World& w, CellI& value) :
    BaseT<UnknownVar>(w, w.std.op.UnknownVar, "op.UnknownVar")
{
    set(w.id.state, w.std.op.State.start);
    set(w.id.value, value);
}

// ============================================================================
Op::Op(World& w) :
    w(w)
{
}

Op::Call& Op::add(Base& lhs, Base& rhs)
{
    return call(lhs, w.std.op.Add)(w.id.other, rhs);
}

Op::Call& Op::call(CellI& self, CellI& method)
{
    return Call::New(w, self, method);
}

Op::Call& Op::equal(CellI& lhs, Base& rhs)
{
    return call(lhs, w.std.op.Equal)(w.id.other, rhs);
}

Op::Call& Op::get(Base& cell, Base& key)
{
    return call(cell, w.std.op.Get)(w.id.key, key);
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

Op::UnknownVar& Op::unknown_(CellI& value)
{
    return UnknownVar::New(w, value);
}

} // namespace cells
} // namespace infocell

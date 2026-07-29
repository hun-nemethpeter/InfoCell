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

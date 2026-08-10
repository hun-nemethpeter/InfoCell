#pragma once
#include "Op.h"

namespace infocell {
namespace cells {

template <typename... Args>
Map& Op::parameters(CellI& name, CellI& value, Args&&... args)
{
    World& w = name.w;
    Map& ret = *new Map(w, w.std.Cell, w.std.op.Parameter);
    ret.add(name, w.ast.parameterInit(name, value));
    if constexpr (sizeof...(Args) > 0) {
        addParameter(ret, std::forward<Args>(args)...);
    }

    return ret;
}

template <typename... Args>
void Op::addParameter(Map& map, CellI& name, CellI& value, Args&&... args)
{
    map.add(name, value.w.ast.parameterInit(name, value));
    if constexpr (sizeof...(Args) > 0) {
        addParameter(map, std::forward<Args>(args)...);
    }
}

} // namespace cells
} // namespace infocell

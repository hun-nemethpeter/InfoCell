#pragma once
#include "Op.h"

namespace infocell {
namespace cells {

template <typename... Args>
Map& Op::parameters(CellI& key, CellI& value, Args&&... args)
{
    World& w = key.w;
    Map& ret = *new Map(w, w.std.Cell, w.std.ast.Slot);
    ret.add(key, w.ast.parameterInit(key, value));
    if constexpr (sizeof...(Args) > 0) {
        addParameter(ret, std::forward<Args>(args)...);
    }

    return ret;
}

template <typename... Args>
void Op::addParameter(Map& map, CellI& key, CellI& value, Args&&... args)
{
    map.add(key, value.w.ast.parameterInit(key, value));
    if constexpr (sizeof...(Args) > 0) {
        addParameter(map, std::forward<Args>(args)...);
    }
}

} // namespace cells
} // namespace infocell

#include "NodeBase.h"

namespace infocell {
namespace cells {

NodeBase::NodeBase(World& w) :
    w(w)
{
}

CellI& NodeBase::getVariable(const std::string& name)
{
    return w.getVariable(name);
}

CellI& NodeBase::getStruct(const std::string& name)
{
    return w.getStruct(name);
}

CellI& NodeBase::getStruct(CellI& id)
{
    return w.getStruct(id);
}

CellI& NodeBase::toCellNumber(int number)
{
    return w.pools.numbers.get(number);
}

} // namespace cells
} // namespace infocell
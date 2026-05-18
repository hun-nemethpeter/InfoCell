#include "NodeBase.h"

namespace infocell {
namespace cells {

NodeBase::NodeBase(brain::Brain& kb) :
    kb(kb)
{
}

CellI& NodeBase::getVariable(const std::string& name)
{
    return kb.getVariable(name);
}

CellI& NodeBase::getStruct(const std::string& name)
{
    return kb.getStruct(name);
}

CellI& NodeBase::getStruct(CellI& id)
{
    return kb.getStruct(id);
}

CellI& NodeBase::toCellNumber(int number)
{
    return kb.pools.numbers.get(number);
}

} // namespace cells
} // namespace infocell
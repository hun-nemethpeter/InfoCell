#include "Cells.h"
#include "World.h"

namespace infocell {
namespace cells {

class OpActivator
{
public:
    OpActivator(CellI& m_currentCell);
    void activate();

protected:
    void activateOpActivate();
    void activateOpAdd();
    void activateOpAnd();
    void activateOpBlock();
    void activateOpCall();
    void activateOpDelete();
    void activateOpDivide();
    void activateOpDo();
    void activateOpEqual();
    void activateOpErase();
    void activateOpFunction();
    void activateOpGet();
    void activateOpGreaterThan();
    void activateOpGreaterThanOrEqual();
    void activateOpHas();
    void activateOpIf();
    void activateOpLessThan();
    void activateOpLessThanOrEqual();
    void activateOpMissing();
    void activateOpMultiply();
    void activateOpNew();
    void activateOpNot();
    void activateOpNotEqual();
    void activateOpNotSame();
    void activateOpOr();
    void activateOpReturn();
    void activateOpSame();
    void activateOpSet();
    void activateOpSubtract();
    void activateOpWhile();

    void saveOpState(List& opStates, CellI& op);
    void loadOpState(CellI& opState);

    void printIndent();
    void resetIndent();

    World& w;
    ID& id;
    Std& std;
    CellI* m_currentCell;
    CellI* m_previousCell;
    CellI* m_currentState     = nullptr;
    int m_indent              = 0;
    bool m_debugFunctionCalls = false;
};

} // namespace cells
} // namespace infocell

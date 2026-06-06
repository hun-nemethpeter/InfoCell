#pragma once
#include <stack>
#include <memory>

#include "Cells.h"

namespace infocell {
namespace cells {
namespace brain {
class Brain;
} // namespace brain

class ToolFinder
{
    struct StackNode
    {
        CellI& effectAst;
        CellI& slotItem;
    };

    struct Node
    {
        ~Node();

        CellI* m_data = 0;
        std::map<CellI*, Node*> m_children;
        bool m_isLeaf = false;
    };

    enum class SlotKind
    {
        StructSlot,
        NormalSlot
    };

    enum class ToolKind
    {
        // no return value e.g.: set(cell, key, value)
        Statement,
        // has a return value, needs unification, e.g.: add(lhs, rhs) -> return
        Expression
    };

    struct FindContext
    {
        Node* trieNode;
        CellI* slotList;
        CellI* slotItemPtr;
        SlotKind slotKind;
        CellI* effectAstPtr;
        std::stack<StackNode> stack;
        ToolKind toolKind;
        CellI* expressionToolPtr;
    };

public:
    ToolFinder(brain::Brain& kb);

    bool empty();
    CellI& serializeEffectAst(CellI& ast);
    void add(CellI& tool, CellI& compiledToolType);
    void add(CellI& effect, CellI& tool, CellI& compiledToolType);
    CellI* findToolByEffectAst(CellI& ast);
    void print();

private:
    CellI* findToolByEffectAstImpl(CellI& inputEffectAst, CellI*& outputEffectAst);
    void createTool(CellI& outCell, CellI& outRole, CellI& ast, CellI& toolDesc);
    void addValue(Node*& node, CellI& value);
    bool checkValue(FindContext& findContext, CellI& key, CellI& value);
    void handleStep(CellI*& effectAstPtr, CellI*& slotItemPtr, Node*& node, std::stack<StackNode>& stack);
    void printCb(Node* node);
    CellI* processToolAst(CellI& toolAst, Map& memberIds, CellI& compiledToolType);

    brain::Brain& kb;
    std::unique_ptr<Node> m_root;
};

} // namespace cells
} // namespace infocell
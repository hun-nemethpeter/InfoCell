#pragma once
#include <stack>
#include <memory>

#include "Cells.h"

namespace infocell {
namespace cells {
class World;

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
    class ConversionToolKey
    {
    public:
        ConversionToolKey(CellI& inputType, CellI& outputType);

        bool operator<(const ConversionToolKey& rhs) const;

        CellI* m_inputType;
        CellI* m_outputType;
    };

    class ConversionToolBlueprint
    {
    public:
        ConversionToolBlueprint(CellI& tool, CellI& compiledToolType, CellI& slotId);

        bool operator<(const ConversionToolBlueprint& blueprint) const;

        CellI* m_tool;
        CellI* m_compiledToolType;
        CellI* m_slotId;
    };

public:
    ToolFinder(World& w);

    bool empty();
    CellI& serializeEffectAst(CellI& ast);
    void add(CellI& tool, CellI& compiledToolType);
    void add(CellI& effect, CellI& tool, CellI& compiledToolType);
    CellI* findToolByEffectAst(CellI& ast);
    CellI& findConversionTools(CellI& from, CellI& to);
    void print();

private:
    CellI* findToolByEffectAstImpl(CellI& inputEffectAst, CellI*& outputEffectAst);
    void createTool(CellI& outCell, CellI& outRole, CellI& ast, CellI& toolDesc);
    void addValue(Node*& node, CellI& value);
    bool checkValue(FindContext& findContext, CellI& key, CellI& value);
    void handleStep(CellI*& effectAstPtr, CellI*& slotItemPtr, Node*& node, std::stack<StackNode>& stack);
    void printCb(Node* node);
    CellI* processToolAst(CellI& toolAst, Map& memberIds, CellI& compiledToolType);
    CellI& createConversionToolFromBlueprint(CellI& from, CellI& to, ConversionToolBlueprint& blueprint);


    World& w;
    std::unique_ptr<Node> m_root;
    std::multimap<ConversionToolKey, ConversionToolBlueprint> m_conversionTools;
};

std::ostream& operator<<(std::ostream& os, const ToolFinder::ConversionToolKey& key);
std::ostream& operator<<(std::ostream& os, const ToolFinder::ConversionToolBlueprint& blueprint);

} // namespace cells
} // namespace infocell
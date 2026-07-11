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
        CellI& ast;
        CellI& slotItem;
    };

    struct Node
    {
        ~Node();

        std::string print();

        bool m_isLeaf   = false;
        CellI* m_data   = nullptr;
        CellI* m_effect = nullptr;
        CellI* m_tool   = nullptr;
        Node* m_parent  = nullptr;
        std::map<CellI*, Node*> m_children;
    };

    enum class SlotKind
    {
        TypeSlot,
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
        std::deque<StackNode> stack;
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
    List& findToolsByEffectAst(CellI& ast);
    CellI& findConversionTools(CellI& from, CellI& to);
    void exploreSlotManipulations();

private:
    CellI* findBuildersForEffectAst(CellI& inputEffectAst);
    CellI* findBuilderForEffectAstOld(CellI& inputEffectAst, CellI*& outputEffectAst);
    void buildTool(CellI& outCell, CellI& outRole, CellI& ast, CellI& builder);
    void addValue(Node*& node, CellI& value);
    void saveCurrentPath(CellI& key, CellI& memberKey, Map& memberIds, std::deque<StackNode>& stack);
    bool checkValue(FindContext& findContext, CellI& key, CellI& value);
    void handleStep(CellI*& effectAstPtr, CellI*& slotItemPtr, Node*& node, std::deque<StackNode>& stack);
    CellI* createBuilder(CellI& toolAst, Map& memberIds, CellI& compiledToolType);
    void createConversionToolFromBlueprint(CellI& from, CellI& to, ConversionToolBlueprint& blueprint, List& results);
    void findConversionToolsByValue(CellI& from, CellI& to, List& results);
    void findConversionToolsByType(CellI& from, CellI& to, List& results);
    void findConversionToolsByContainer(CellI& from, CellI& to, List& results);
    std::string printTool(CellI& tool);

    World& w;
    std::unique_ptr<Node> m_root;
    List m_tools;
    std::multimap<ConversionToolKey, ConversionToolBlueprint> m_conversionTools;
};

std::ostream& operator<<(std::ostream& os, const ToolFinder::ConversionToolKey& key);
std::ostream& operator<<(std::ostream& os, const ToolFinder::ConversionToolBlueprint& blueprint);

} // namespace cells
} // namespace infocell
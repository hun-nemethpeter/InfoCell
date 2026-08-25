#pragma once
#include <stack>
#include <memory>

#include "Ast.h"
#include "Cells.h"

namespace infocell {
namespace cells {
class World;

class ToolFinder
{
    struct StackNode
    {
        CellI* effectPtr;
        CellI* slotItemPtr;
        CellI* paramItemPtr;
    };

    struct Node
    {
        ~Node();

        std::string print();

        bool m_isLeaf    = false;
        List* m_builders = nullptr;
        CellI* m_effect  = nullptr;
        CellI* m_tool    = nullptr;
        Node* m_parent   = nullptr;
        std::map<CellI*, Node*> m_children;
    };

public:
    enum class DescriptionKind
    {
        consequence,
        selfBuilder
    };

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
        ConversionToolBlueprint(CellI& tool, CellI& slotId);

        bool operator<(const ConversionToolBlueprint& blueprint) const;

        CellI* m_tool;
        CellI* m_slotId;
    };

public:
    ToolFinder(World& w);

    bool empty();
    List& serializeEffect(CellI& effect);
    void add(Object& tool);
    void add(CellI& tool, CellI& description, DescriptionKind descriptionKind);
    List& findToolsByDescription(CellI& description, DescriptionKind descriptionKind);
    CellI& findConversionTools(CellI& from, CellI& to);
    void exploreSlotManipulations();

private:
    class BuildToolInfo
    {
    public:
        BuildToolInfo(CellI& outCell, CellI& outKey, CellI& matchedEffect, CellI& builder);

        CellI& m_outCell;
        CellI& m_outKey;
        CellI& m_matchedEffect;
        CellI& m_builder;
    };

    enum class ParamValueKind
    {
        NonParamValue,
        ConstVar,
        UnknownVar,
        Self,
        Return,
        Parameter,
        Call
    };

    enum class MultiMatchState
    {
        Detect,
        Restore,
        Skip
    };

    List& recombine(Node* rootNode, CellI& description);
    void addPermutation(Node* rootNode, CellI& description);
    bool hasPermutation(Node* rootNode, CellI& description);
    List& add(CellI& tool, CellI& description, Node* rootNode);
    bool isConstantFoldingPossible(CellI& description);
    void createParametersMappingForAlternativeParameterOrder(CellI& alternativeParameterOrder, List& mappingList);
    bool checkUnknownsInTool(CellI& effect);
    List* findBuildersForDescription(CellI& description, DescriptionKind descriptionKind);
    void buildTool(const BuildToolInfo& buildToolInfo);
    void serializeKeyWithConstValue(List& result, CellI& key, CellI& value);
    void serializeKeyWithParamValue(List& result, CellI& key, CellI& value, ParamValueKind& paramValueKind);
    void addValue(Node*& node, CellI& value);
    void addKeyWithConstValue(Node*& node, CellI& key, CellI& value);
    void addKeyWithParamValue(Node*& node, CellI& key, CellI& value, ParamValueKind& paramValueKind);
    void saveCurrentPath(CellI& key, CellI& memberKey, Map& memberIds, std::deque<StackNode>& stack);
    bool checkConstValue(Node*& node, CellI& key, CellI& value);
    bool checkValue(Node*& node, CellI& key, CellI& value, bool& needPush, MultiMatchState& multiMatchState, CellI*& multiMatch);
    List& createBuilder(CellI& tool, Map& memberIds, bool hasReturnInEffect);
    void createConversionToolFromBlueprint(CellI& from, CellI& to, ConversionToolBlueprint& blueprint, List& results);
    void findConversionToolsByValue(CellI& from, CellI& to, List& results);
    void findConversionToolsByType(CellI& from, CellI& to, List& results);
    void findConversionToolsByContainer(CellI& from, CellI& to, List& results);
    std::string printTool(CellI& tool);
    Node* getRootNodeForDescriptionKind(DescriptionKind descriptionKind);

    World& w;
    ID& id;
    Std& std;
    std::unique_ptr<Node> m_consequenceRootNode;
    std::unique_ptr<Node> m_selfBuilderRootNode;
    List m_tools;
    std::multimap<ConversionToolKey, ConversionToolBlueprint> m_conversionTools;
};

std::ostream& operator<<(std::ostream& os, const ToolFinder::DescriptionKind& descriptionKind);
std::ostream& operator<<(std::ostream& os, const ToolFinder::ConversionToolKey& key);
std::ostream& operator<<(std::ostream& os, const ToolFinder::ConversionToolBlueprint& blueprint);

} // namespace cells
} // namespace infocell
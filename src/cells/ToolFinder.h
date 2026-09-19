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

    struct BuilderChainNode
    {
        CellI* m_transformerBuilder = nullptr;
        CellI* m_mutatingBuilders   = nullptr;
    };

    struct Node
    {
        ~Node();

        std::string printAsGrapviz(World& w);

        bool m_isLeaf    = false;
        List* m_builders = nullptr;
        CellI* m_effect  = nullptr;
        CellI* m_tool    = nullptr;
        Node* m_parent   = nullptr;
        std::list<BuilderChainNode> m_solver;
        std::map<CellI*, Node*> m_children;
    };

public:
    enum class DescriptionKind
    {
        consequence,
        selfBuilder,
        solver
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

    void mergeTo(ToolFinder& target);
    bool empty();
    List& serializeEffect(CellI& effect);
    void add(CellI& tool);
    List& add(CellI& tool, CellI& description, DescriptionKind descriptionKind);
    List& findToolsByDescription(CellI& description, DescriptionKind descriptionKind);
    List* solve(CellI& equation);
    CellI& findConversionTools(CellI& from, CellI& to);
    std::string printConsequenceNodesAsGrapviz();
    void exploreSlotManipulations();

    struct SolverStateNode;
    struct SolverState
    {
        SolverState(Node* rootNode, ToolFinder& toolFinder, CellI& description);

        void run();
        SolverStateNode& startSolverNode();
        void addNextState(SolverStateNode& solverStateNode);
        std::list<std::list<ToolFinder::BuilderChainNode>*>& results();
        void printAsDot();

        std::function<void(std::vector<std::function<void(SolverStateNode& solverNode, CellI& memberName, CellI& memberValue)>>& commandFns, SolverStateNode* solverNodePtr)> m_processFn;
        std::vector<std::function<void(SolverStateNode& solverNode, CellI& memberName, CellI& memberValue)>> m_commandFns;
        std::function<bool(CellI& memberRole)> m_filterFn;
        std::function<void(SolverStateNode& solverNode)> m_popFn;
        std::function<void(SolverStateNode& solverNode)> m_resultFn;
        ToolFinder& m_toolFinder;
        CellI& m_description;
        std::unique_ptr<SolverStateNode> m_startSolverNode;
        std::deque<SolverStateNode*> m_nextStates;
        std::list<std::list<ToolFinder::BuilderChainNode>*> m_results;
    };

    struct SolverPointer
    {
        SolverPointer();
        SolverPointer(CellI& description);
        SolverPointer(SolverPointer* parent, CellI* cellPtr, CellI* memberNodePtr);

        CellI& memberName();
        CellI& memberValue();
        std::string printKV();
        SolverPointer step(SolverStateNode& solverStateNode, int& popCount);
        bool isLast();

        SolverPointer* m_parent = nullptr;
        CellI* m_cellPtr        = nullptr;
        CellI* m_memberNodePtr  = nullptr;
    };

    struct SolverStateNode
    {
        enum class MatchStatus
        {
            created,
            accepted,
            checked,
            failed,
            finished
        };

        enum class InputCommand
        {
            and_,
            or_,
            push
        };

        struct SubCommand
        {
            enum class Kind
            {
                nop,
                checkKey,
                checkKeyValue,
                checkAndCaptureValue
            };

            bool evaluate(ToolFinder& toolFinder, Node*& node);
            std::string printKey1();
            std::string printKey2();

            Kind m_kind   = Kind::nop;
            CellI* m_key1 = nullptr;
            CellI* m_key2 = nullptr;
        };

        SolverStateNode(SolverState& state, SolverPointer solverPointer, Node* nodePtr, SolverStateNode* parent);

        void checkKey(CellI& key);
        void checkAndCaptureValue(CellI& key, CellI& value);
        void checkKeyValue(CellI& key, CellI& value);
        void or_();
        void push();
        SolverStateNode& addNext(SolverStateNode*& solverNodePtr);
        SolverStateNode& addChild(SolverStateNode& solverNode);
        SolverStateNode* step();
        SolverPointer& pointer();
        void pointer(CellI& description);
        void pointer(SolverPointer& solverPointer);
        bool evaluate();
        CellI* saveCurrentPath();
        bool empty();

        SolverState& m_state;
        InputCommand m_inputCommand = InputCommand::and_;
        Node* m_nodePtr             = nullptr;
        SolverStateNode* m_parent   = nullptr;
        std::vector<SubCommand> m_subCommands;
        std::vector<std::unique_ptr<SolverStateNode>> m_children;
        SolverPointer m_solverPointer;
        MatchStatus m_matchStatus = MatchStatus::created;
        CellI* m_capturedPath     = nullptr;
        CellI* m_capturedValue    = nullptr;
        CellI* m_capturedVar      = nullptr;
    };

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

    struct RecombineResult
    {
        bool m_isConstantFoldingPossible = false;
        CellI* m_builderForTool;
        CellI& m_recombinedTool;
    };

    std::list<RecombineResult> recombine(Node* rootNode, CellI& description);
    void exploreSlotManipulationFor(CellI& description);
    void addSolver(CellI& description, std::list<BuilderChainNode>& solver);
    std::list<std::list<ToolFinder::BuilderChainNode>*> getSolvers(CellI& description);
    void addPermutation(Node* rootNode, CellI& description);
    bool hasPermutation(Node* rootNode, CellI& description);
    List& add(CellI& tool, CellI& description, Node* rootNode);
    bool isConstantFoldingPossible(CellI& description);
    CellI& doConstantFolding(CellI& description);
    void createParametersMappingForAlternativeParameterOrder(CellI& alternativeParameterOrder, List& mappingList);
    bool checkUnknownsInTool(CellI& effect);
    std::unique_ptr<List> findBuildersForDescription(CellI& description, DescriptionKind descriptionKind);
    void buildTool(const BuildToolInfo& buildToolInfo);
    void serializeKeyWithConstValue(List& result, CellI& key, CellI& value);
    void serializeKeyWithParamValue(List& result, CellI& key, CellI& value, ParamValueKind& paramValueKind);
    void addValue(Node*& node, CellI& value);
    void addKeyWithConstValue(Node*& node, CellI& key, CellI& value);
    void addKeyWithParamValue(Node*& node, CellI& key, CellI& value, ParamValueKind& paramValueKind);
    void saveCurrentPath(CellI& key, CellI& memberKey, Map& memberIds, std::deque<StackNode>& stack);
    bool checkConstValue(Node*& node, CellI& value);
    bool checkConstKeyValue(Node*& node, CellI& key, CellI& value);
    List& createBuilder(CellI& tool, Map& memberIds, bool hasReturnInEffect);
    void createConversionToolFromBlueprint(CellI& from, CellI& to, ConversionToolBlueprint& blueprint, List& results);
    void findConversionToolsByType(CellI& from, CellI& to, List& results);
    void findConversionToolsByContainer(CellI& from, CellI& to, List& results);
    std::string printTool(CellI& tool);
    Node* getRootNodeForDescriptionKind(DescriptionKind descriptionKind);

    World& w;
    ID& id;
    Std& std;
    std::unique_ptr<Node> m_consequenceRootNode;
    std::unique_ptr<Node> m_selfBuilderRootNode;
    std::unique_ptr<Node> m_solverRootNode;
    List m_tools;
    std::multimap<ConversionToolKey, ConversionToolBlueprint> m_conversionTools;
};

std::ostream& operator<<(std::ostream& os, const ToolFinder::DescriptionKind& descriptionKind);
std::ostream& operator<<(std::ostream& os, const ToolFinder::ConversionToolKey& key);
std::ostream& operator<<(std::ostream& os, const ToolFinder::ConversionToolBlueprint& blueprint);
std::ostream& operator<<(std::ostream& os, const ToolFinder::SolverStateNode::MatchStatus& matchStatus);
std::ostream& operator<<(std::ostream& os, const ToolFinder::SolverStateNode::InputCommand& inputCommand);
std::ostream& operator<<(std::ostream& os, const ToolFinder::SolverStateNode::SubCommand::Kind& kind);

} // namespace cells
} // namespace infocell
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

        std::string print();

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
    void add(CellI& tool, CellI& description, DescriptionKind descriptionKind);
    List& findToolsByDescription(CellI& description, DescriptionKind descriptionKind);
    CellI* solve(CellI& equation);

    struct SolverStateNode;

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
        bool isUninitialized();
        bool operator==(const SolverPointer& rhs) const;

        SolverPointer* m_parent = nullptr;
        CellI* m_cellPtr        = nullptr;
        CellI* m_memberNodePtr  = nullptr;
    };

    struct SolverState
    {
        SolverState(ToolFinder& toolFinder, CellI& description);

        void run();
        SolverStateNode& startSolverNode();
        void addNextState(SolverStateNode& solverStateNode);
        std::list<std::list<ToolFinder::BuilderChainNode>*>& results();
        void printAsDot();

        ToolFinder& m_toolFinder;
        CellI& m_description;
        Node& m_rootNode;
        std::unique_ptr<SolverStateNode> m_startSolverNode;
        std::deque<SolverStateNode*> m_nextStates;
        std::list<std::list<ToolFinder::BuilderChainNode>*> m_results;
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
            check,
            or_,
            push
        };
        struct SubCommand
        {
            enum class Kind
            {
                nop,
                checkKey,
                checkKeyValue
            };

            bool evaluate(ToolFinder& toolFinder, Node*& node);
            Kind m_kind   = Kind::nop;
            CellI* m_key1 = nullptr;
            CellI* m_key2 = nullptr;
        };

        SolverStateNode(SolverState& state, SolverPointer solverPointer, Node* m_nodePtr, SolverStateNode* previous);

        void checkKey(CellI& key);
        void checkKeyValue(CellI& key, CellI& value);
        void or_();
        void push();
        SolverStateNode& addNext(SolverStateNode*& solverNodePtr);
        SolverStateNode& addChild(SolverStateNode& solverNodePtr);
        SolverStateNode* step();
        SolverPointer& pointer();
        void pointer(CellI& description);
        void pointer(SolverPointer& solverPointer);
        bool evaluate();
        bool empty();

        SolverState& m_state;
        InputCommand m_inputCommand = InputCommand::check;
        Node* m_nodePtr             = nullptr;
        CellI* m_inputCell          = nullptr;
        SolverStateNode* m_parent   = nullptr;
        SolverStateNode* m_previous = nullptr;
        std::vector<SubCommand> m_subCommands;
        std::vector<std::unique_ptr<SolverStateNode>> m_children;
        SolverPointer m_solverPointer;
        MatchStatus m_matchStatus = MatchStatus::created;
    };

    std::unique_ptr<ToolFinder::SolverState> getSolver2(CellI& description);
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

    struct RecombineResult
    {
        bool m_isConstantFoldingPossible = false;
        CellI* m_builderForTool;
        CellI& m_recombinedTool;
    };

    std::list<RecombineResult> recombine(Node* rootNode, CellI& description);
    void exploreSlotManipulationFor(CellI& description);
    void addSolver(CellI& description, std::list<BuilderChainNode>& solver);
    std::list<BuilderChainNode>* getSolver(CellI& description);
    void addPermutation(Node* rootNode, CellI& description);
    bool hasPermutation(Node* rootNode, CellI& description);
    List& add(CellI& tool, CellI& description, Node* rootNode);
    bool isConstantFoldingPossible(CellI& description);
    CellI& doConstantFolding(CellI& description);
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
    bool checkConstValue(Node*& node, CellI& value);
    bool checkConstKeyValue(Node*& node, CellI& key, CellI& value);
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
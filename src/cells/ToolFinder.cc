#include "ToolFinder.h"

#include "Compiler.h"
#include "World.h"
#include "cells/printers/ValuePrinter.h"

#include "util/Panic.h"
#define SPDLOG_ACTIVE_LEVEL SPDLOG_LEVEL_TRACE
#include "util/Log.h"
#include <fmt/ostream.h>

#include <sstream>

// spdlog needs this https://fmt.dev/latest/api/#ostream-api to be able to print
template <>
struct fmt::formatter<infocell::cells::ToolFinder::ConversionToolKey> : ostream_formatter
{ };

template <>
struct fmt::formatter<infocell::cells::ToolFinder::ConversionToolBlueprint> : ostream_formatter
{ };

template <>
struct fmt::formatter<infocell::cells::ToolFinder::DescriptionKind> : ostream_formatter
{ };

template <>
struct fmt::formatter<infocell::cells::ToolFinder::SolverStateNode::MatchStatus> : ostream_formatter
{ };

template <>
struct fmt::formatter<infocell::cells::ToolFinder::SolverStateNode::InputCommand> : ostream_formatter
{ };

template <>
struct fmt::formatter<infocell::cells::ToolFinder::SolverStateNode::SubCommand::Kind> : ostream_formatter
{ };

namespace infocell {
namespace cells {

std::string ToolFinder::Node::printAsGrapviz(World& w)
{
    std::stringstream ss;
    std::deque<std::string> path;
    Node* startNode = this;

    ss <<
        R"(
digraph structs {
    node  [shape=plaintext]
    graph [fontname = "Helvetica",
           fontsize = 36
          ];

)";

    struct Edge
    {
        Node* from;
        Node* to;
    };
    std::deque<Edge> edges;
    std::set<Node*> nodes;
    std::map<Node*, CellI*> nodeToCell;
    edges.push_back({ nullptr, startNode });
    nodeToCell[startNode] = &w.id.first;

    while (!edges.empty()) {
        Edge edge = edges.front();
        edges.pop_front();
        if (edge.from) {
            nodes.insert(edge.from);
        }
        nodes.insert(edge.to);
        for (auto& child : edge.to->m_children) {
            nodeToCell[child.second] = child.first;
            edges.push_back({ edge.to, child.second });
        }
    }

    for (Node* nodePtr : nodes) {
        Node& node = *nodePtr;
        ss << fmt::format(
            R"("node{}" [label=<
              <TABLE BORDER="0" CELLBORDER="0">
                  <TR>
                      <TD COLSPAN="5" bgcolor="#a2d2ff">{}</TD>
                  </TR>
                  <TR>
                      <TD COLSPAN="5" bgcolor="{}">{}</TD>
                  </TR>
)",
            fmt::ptr(nodePtr), nodeToCell[nodePtr]->label(), node.m_isLeaf ? "#a7c957" : "wheat", node.m_isLeaf ? "value" : "empty");
        ss << fmt::format(
            R"(
              </TABLE>
         >];
)");
    }

    edges.push_back({ nullptr, startNode });
    while (!edges.empty()) {
        Edge edge = edges.front();
        edges.pop_front();
        nodes.insert(edge.from);
        nodes.insert(edge.to);

        ss << fmt::format("\"node{}\" -> \"node{}\"\n", fmt::ptr(edge.from), fmt::ptr(edge.to));
        for (auto& child : edge.to->m_children) {
            edges.push_back({ edge.to, child.second });
        }
    }

    ss << "}";

    TRACE(toolFinderGraphviz, "{}", ss.str());

    return ss.str();
}

std::string ToolFinder::printTool(CellI& tool)
{
    std::stringstream ss;

    if (tool[id.name].label() == "Add") {
        ss << "m_lhs + m_rhs";
    } else if (tool[id.name].label() == "Subtract") {
        ss << "m_lhs - m_rhs";
    } else if (tool.has(w.id.primitiveTool)) {
        ss << tool[id.name].label() << "(";
        if (tool.has("members")) {
            int i = 0;
            for (CellI& member : tool[id.members]) {
                if (i++ > 0) {
                    ss << ", ";
                }
                ss << "m_" << member[id.key].label();
            }
        }
        ss << ")";
//        if (tool.has("returnType")) {
//            ss << " -> return";
//        }
    }
    return ss.str();
}

// ============================================================================
ToolFinder::Node::~Node()
{
    for (auto& pair : m_children) {
        delete pair.second;
    }
}

// ============================================================================
ToolFinder::ToolFinder(World& w) :
    w(w),
    id(w.id),
    std(w.std),
    m_tools(w, std.String)
{
    m_consequenceRootNode = std::make_unique<Node>();
    m_selfBuilderRootNode = std::make_unique<Node>();
    m_solverRootNode      = std::make_unique<Node>();
}

// ============================================================================
bool ToolFinder::empty()
{
    return m_consequenceRootNode->m_children.empty() && m_selfBuilderRootNode->m_children.empty();
}

// ============================================================================
void ToolFinder::mergeTo(ToolFinder& target)
{
    for (CellI& tool : m_tools) {
        target.add(tool);
    }
}

// ============================================================================
void ToolFinder::serializeKeyWithConstValue(List& result, CellI& key, CellI& value)
{
    if (&key == &id.__type__) {
        result.add(id.op);
        result.add(id.type);
    } else {
        result.add(key);
    }
    if (&value == &id.op) {
        result.add(id.op);
    }
    result.add(value);
}


// ============================================================================
void ToolFinder::serializeKeyWithParamValue(List& result, CellI& key, CellI& value, ParamValueKind& paramValueKind)
{
    result.add(key);
    if (&value == &id.op) {
        result.add(id.op);
    }
    if (&value.__type__() == &std.op.ConstVar) {
        result.add(value[id.value]);
        if (&value[id.value] == &id.op) {
            // handling the _(op) case, where op is a constant
            // it will be op op, so the escaping character here is op
            result.add(value[id.value]);
        }
        paramValueKind = ParamValueKind::ConstVar;
    } else if (&value.__type__() == &std.op.UnknownVar) {
        result.add(value[id.value]);
        paramValueKind = ParamValueKind::UnknownVar;
    } else if (&value.__type__() == &std.ast.Self) {
        result.add(id.op);
        result.add(id.variable);
        paramValueKind = ParamValueKind::Self;
    } else if (&value.__type__() == &std.ast.Return) {
        result.add(id.op);
        result.add(id.variable);
        paramValueKind = ParamValueKind::Return;
    } else if (&value.__type__() == &std.ast.Parameter) {
        result.add(id.op);
        result.add(id.variable);
        paramValueKind = ParamValueKind::Parameter;
    } else if (&value.__type__() == &std.op.Call || value.__type__().has(id.primitiveTool)) {
        result.add(id.op);
        result.add(id.push);
        paramValueKind = ParamValueKind::Call;
    } else {
        panic("Unknow param type!");
    }
}

// ============================================================================
List& ToolFinder::serializeEffect(CellI& effect)
{
    List& ret = *new List(w, std.Cell);
    SolverState solverState(nullptr, *this, effect);

    solverState.m_processFn = [this, &ret](std::vector<std::function<void(SolverStateNode & solverNode, CellI & memberName, CellI & memberValue)>>& commandFns, SolverStateNode* solverNodePtr) {
        SolverPointer pointer = solverNodePtr->pointer();
        auto& solverNode      = *solverNodePtr;
        CellI* currentCellPtr = pointer.m_cellPtr;
        CellI& currentCell    = *currentCellPtr;
        CellI& memberKV       = (*pointer.m_memberNodePtr)[id.value];
        CellI& member         = memberKV[id.value];
        CellI& memberName     = member[id.name];
        CellI& memberRelation = member[id.relation];
        CellI& memberRole     = member[id.role];
        CellI& memberValue    = currentCell[memberName];

        if (&memberRelation == &std.op.Member.Relation.external) {
            if (&memberRole == &std.op.Member.Role.constValue) {
                serializeKeyWithConstValue(ret, memberName, memberValue);
            } else if (&memberRole == &std.op.Member.Role.constVarValue) {
                serializeKeyWithConstValue(ret, memberName, memberValue[id.value]);
            } else if (&memberRole == &std.op.Member.Role.input) {
                ParamValueKind paramValueKind = ParamValueKind::NonParamValue;
                serializeKeyWithParamValue(ret, memberName, memberValue, paramValueKind);
                if (paramValueKind == ParamValueKind::Call) {
                    solverNode.push();
                }
            } else {
                panic("Unprocessed member!");
            }
        } else {
            ret.add(memberName);
            ret.add(id.op);
            ret.add(id.push);
            solverNode.push();
        }
    };
    solverState.m_filterFn = [this](CellI& cell, CellI& member) -> bool {
        if (cell.missing(member[id.name])) {
            return false;
        }
        CellI& memberRole = member[id.role];
        return &memberRole == &std.op.Member.Role.constValue || &memberRole == &std.op.Member.Role.constVarValue || &memberRole == &std.op.Member.Role.input;
    };
    solverState.m_popFn = [this, &ret](SolverStateNode&) {
        ret.add(id.op);
        ret.add(id.pop);
    };
    solverState.m_resultFn = [](SolverStateNode&) {
    };

    solverState.run();

    return ret;
}

std::ostream& operator<<(std::ostream& os, const ToolFinder::DescriptionKind& descriptionKind)
{
    switch (descriptionKind) {
    case ToolFinder::DescriptionKind::consequence:
        os << "consequence";
        break;
    case ToolFinder::DescriptionKind::selfBuilder:
        os << "selfBuilder";
        break;
    }

    return os;
}

// ============================================================================
ToolFinder::ConversionToolKey::ConversionToolKey(CellI& inputType, CellI& outputType) :
    m_inputType(&inputType),
    m_outputType(&outputType)
{
}

bool ToolFinder::ConversionToolKey::operator<(const ConversionToolKey& rhs) const
{
    return std::tie(m_inputType, m_outputType) < std::tie(rhs.m_inputType, rhs.m_outputType);
}

std::ostream& operator<<(std::ostream& os, const ToolFinder::ConversionToolKey& key)
{
    os << "ConversionToolKey [from: " << key.m_inputType->label() << ", to: " << key.m_outputType->label() << "]";
    return os;
}

ToolFinder::ConversionToolBlueprint::ConversionToolBlueprint(CellI& tool, CellI& slotId) :
    m_tool(&tool),
    m_slotId(&slotId)
{
}

bool ToolFinder::ConversionToolBlueprint::operator<(const ConversionToolBlueprint& blueprint) const
{
    return std::tie(m_tool, m_slotId) < std::tie(blueprint.m_tool, blueprint.m_slotId);
}

std::ostream& operator<<(std::ostream& os, const ToolFinder::ConversionToolBlueprint& blueprint)
{
    os << "ConversionToolBlueprint [tool: " << blueprint.m_tool->label() << ", input: " << blueprint.m_slotId->label() << "]";
    return os;
}

// ============================================================================
void ToolFinder::add(CellI& tool)
{
    if (tool.has(id.returnType)) {
        // so this can be a conversion tool
        CellI& returnType = tool[id.returnType];
        if (tool.has(id.parameters)) {
            for (CellI& parameterKV : tool[id.parameters].__type__()[id.members]) {
                CellI& parameter = parameterKV[id.value];
                CellI& inputType = parameter[id.type];
                ConversionToolKey key(inputType, returnType);
                ConversionToolBlueprint blueprint(tool, parameter[id.name]);
                m_conversionTools.insert({ key, blueprint });
            }
        }
    }
    m_tools.add(tool);

    TRACE(toolFinder, "{} =>", tool.label());

    auto& description = tool[id.description];
    if (description.has(id.consequences)) {
        for (CellI& consequence : description[id.consequences]) {
            add(tool, consequence, DescriptionKind::consequence);
        }
    }
    if (description.has(id.selfBuilders)) {
        bool firstBuilder = true;
        List& mappingList = *new List(w, std.Cell);
        Object& selfBuilders = *new Object(w, std.op.SelfBuilders, "SelfBuilders");
        tool.set(id.selfBuilders, selfBuilders);

        for (CellI& selfBuilder : description[id.selfBuilders]) {
            List& builders = add(tool, selfBuilder, DescriptionKind::selfBuilder);
            selfBuilders.set(id.builder, builders);

            if (firstBuilder) {
                firstBuilder = false;
                mappingList.add(id.emptyObject);
            } else {
                Map& alternativeParameterMapping = generateAlternativeParameterMapping(selfBuilder);
                mappingList.add(alternativeParameterMapping);
            }
        }
        selfBuilders.set(id.memberMapping, mappingList);
    }
}

// ============================================================================
Map& ToolFinder::generateAlternativeParameterMapping(CellI& selfBuilder)
{
    bool primitiveTool = selfBuilder.__type__().has(id.primitiveTool);
    Map& generatedMapping = *new Map(w, w.std.String, w.std.String);
    for (auto& parameterKV : selfBuilder.__type__()[id.parametersType][id.members]) {
        auto& fromKey       = parameterKV[id.key];

        CellI& parameterObj = primitiveTool ? selfBuilder : selfBuilder[id.parameters];
        auto& parameter     = parameterObj[fromKey];

        CellI* toKeyPtr = nullptr;
        if (&parameter.__type__() == &std.ast.Parameter) {
            toKeyPtr = &parameter[id.key];
        } else if (&parameter.__type__() == &std.ast.Self) {
            toKeyPtr = &id.self;
        } else {
            panic("Unknown parameter type!");
        }
        CellI& toKey = *toKeyPtr;
        generatedMapping.add(fromKey, toKey);
    }

    return generatedMapping;
}

// ============================================================================
void ToolFinder::addValue(Node*& node, CellI& value)
{
    Node*& childNode = node->m_children[&value];
    if (childNode == nullptr) {
        childNode           = new Node();
        childNode->m_parent = node;
    }
    node = childNode;
}

// ============================================================================
void ToolFinder::addKeyWithConstValue(Node*& node, CellI& key, CellI& value)
{
    TRACE(toolFinderExplore, "addKeyWithConstValue {}: {}", key.label(), value.label());
    if (&key == &id.__type__) {
        addValue(node, id.op);
        addValue(node, id.type);
    } else {
        addValue(node, key);
    }
    if (&value == &id.op) {
        addValue(node, id.op);
    }
    addValue(node, value);
}

// ============================================================================
void ToolFinder::addKeyWithParamValue(Node*& node, CellI& key, CellI& value, ParamValueKind& paramValueKind)
{
    addValue(node, key);
    if (&value == &id.op) {
        addValue(node, id.op);
    }
    if (&value.__type__() == &std.op.ConstVar) {
        addValue(node, id.op);
        addValue(node, id.value);
        addValue(node, value[id.value]);
        if (&value[id.value] == &id.op) {
            // handling the _(op) case, where op is a constant
            // it will be op op, so the escaping character here is op
            addValue(node, value[id.value]);
        }
        paramValueKind = ParamValueKind::ConstVar;
    } else if (&value.__type__() == &std.ast.Self) {
        if (value.has(id.value) && value[id.value].has(id.value)) {
            addValue(node, id.op);
            addValue(node, id.value);
            addValue(node, value[id.value][id.value]);
        } else {
            addValue(node, id.op);
            addValue(node, id.variable);
        }
        paramValueKind = ParamValueKind::Self;
    } else if (&value.__type__() == &std.ast.Return) {
        if (value.has(id.value) && value[id.value].has(id.value)) {
            addValue(node, id.op);
            addValue(node, id.value);
            addValue(node, value[id.value][id.value]);
        } else {
            addValue(node, id.op);
            addValue(node, id.variable);
        }
        paramValueKind = ParamValueKind::Return;
    } else if (&value.__type__() == &std.ast.Parameter) {
        if (value.has(id.value) && value[id.value].has(id.value)) {
            addValue(node, id.op);
            addValue(node, id.value);
            addValue(node, value[id.value][id.value]);
        } else {
            addValue(node, id.op);
            addValue(node, id.variable);
        }
        paramValueKind = ParamValueKind::Parameter;
    } else if (&value.__type__() == &std.op.Call || value.__type__().has(id.primitiveTool)) {
        addValue(node, id.op);
        addValue(node, id.push);
        paramValueKind = ParamValueKind::Call;
    } else {
        panic("Unknow param type!");
    }
}

// ============================================================================
List& ToolFinder::add(CellI& tool, CellI& description, DescriptionKind descriptionKind)
{
    auto& builders = add(tool, description, getRootNodeForDescriptionKind(descriptionKind));

    if (IS_LOG_ENABLED) {
        CellI& astAsList = serializeEffect(description);
        std::stringstream ss;
        for (CellI& value : astAsList) {
            if (value.has(id.primitiveTool)) {
                ss << value[id.name].label() << " ";
            } else {
                ss << value.label() << " ";
            }
        }
        TRACE(toolFinder, "  {}: {}", descriptionKind, ss.str());
    }
    return builders;
}

// ============================================================================
List& ToolFinder::add(CellI& tool, CellI& description, Node* rootNode)
{
    Map memberIds(w, std.Cell, std.Cell);
    bool hasReturnInEffect = false;
    Node* currentNode      = rootNode;
    auto saveCurrentPath   = [this, &memberIds](CellI& key, CellI& memberKey, SolverPointer& solverPointer) {
        auto getSlotForKey = [this](CellI& key) -> CellI& {
            if (&key == &id.self) {
                return w.ast.member(key);
            } else {
                return w.ast.parameter(key);
            }
        };
        if (!memberIds.hasKey(memberKey)) {
            List& path = *new List(w, std.Cell, fmt::format("path for {}", memberKey.label()));
            SolverPointer* solverPointerPtr = &solverPointer;
            while (solverPointerPtr) {
                path.addFront(getSlotForKey((*solverPointerPtr->m_memberNodePtr)[id.value][id.key]));
                solverPointerPtr = solverPointerPtr->m_parent;
            }
            memberIds.add(memberKey, path);
        }
    };

    SolverState solverState(rootNode, *this, description);

    solverState.m_processFn = [this, &memberIds, &hasReturnInEffect, &saveCurrentPath](std::vector<std::function<void(SolverStateNode & solverNode, CellI & memberName, CellI & memberValue)>>& commandFns, SolverStateNode* solverNodePtr) {
        SolverPointer pointer = solverNodePtr->pointer();
        auto& solverNode      = *solverNodePtr;
        CellI* currentCellPtr = pointer.m_cellPtr;
        CellI& currentCell    = *currentCellPtr;
        CellI& memberKV       = (*pointer.m_memberNodePtr)[id.value];
        CellI& member         = memberKV[id.value];
        CellI& memberName     = member[id.name];
        CellI& memberRelation = member[id.relation];
        CellI& memberRole     = member[id.role];
        CellI& memberValue    = currentCell[memberName];

        if (&memberRelation == &std.op.Member.Relation.external) {
            if (&memberRole == &std.op.Member.Role.constValue) {
                addKeyWithConstValue(solverNode.m_nodePtr, memberName, memberValue);
            } else if (&memberRole == &std.op.Member.Role.constVarValue) {
                addKeyWithConstValue(solverNode.m_nodePtr, memberName, memberValue[id.value]);
            } else if (&memberRole == &std.op.Member.Role.input) {
                ParamValueKind paramValueKind = ParamValueKind::NonParamValue;
                addKeyWithParamValue(solverNode.m_nodePtr, memberName, memberValue, paramValueKind);
                if (paramValueKind == ParamValueKind::Return) {
                    saveCurrentPath(memberName, id.return_, solverNode.m_solverPointer);
                    hasReturnInEffect = true;
                } else if (paramValueKind == ParamValueKind::Self) {
                    saveCurrentPath(memberName, id.self, solverNode.m_solverPointer);
                } else if (paramValueKind == ParamValueKind::Parameter) {
                    saveCurrentPath(memberName, memberValue[id.key], solverNode.m_solverPointer);
                } else if (paramValueKind == ParamValueKind::Call) {
                    solverNode.push();
                }
            } else {
                panic("Unprocessed member!");
            }
        } else {
            addValue(solverNode.m_nodePtr, memberName);
            addKeyWithConstValue(solverNode.m_nodePtr, id.op, id.push);
            solverNode.push();
        }
    };
    solverState.m_filterFn = [this](CellI& cell, CellI& member) -> bool {
        if (cell.missing(member[id.name])) {
            return false;
        }
        CellI& memberRole = member[id.role];
        return &memberRole == &std.op.Member.Role.constValue || &memberRole == &std.op.Member.Role.constVarValue || &memberRole == &std.op.Member.Role.input;
    };
    solverState.m_popFn = [this](SolverStateNode& solverNode) {
        addValue(solverNode.m_nodePtr, id.op);
        addValue(solverNode.m_nodePtr, id.pop);
    };
    solverState.m_resultFn = [&currentNode](SolverStateNode& solverNode) {
        currentNode = solverNode.m_nodePtr;
    };

    solverState.run();

    currentNode->m_isLeaf = 1;
    if (currentNode->m_builders == nullptr) {
        currentNode->m_builders = new List(w, std.List, "builders");
    }
    List& builders = *currentNode->m_builders;
    auto& builder  = createBuilder(tool, memberIds, hasReturnInEffect);
    builders.add(builder);

    currentNode->m_tool   = &tool;
    currentNode->m_effect = &description;

    return builders;
}

// ============================================================================
List& ToolFinder::createBuilder(CellI& tool, Map& memberIds, bool hasReturnInEffect)
{
    List& builder = *new List(w, std.Cell, fmt::format("builder for {}", tool.label()));

    if (hasReturnInEffect) {
        builder.add(w.ast.member(id.__type__));
        builder.add(w.ast.primitiveToolName(std.op.Equal[id.ast]));
        builder.add(w.ast.member(id.lhs));
        builder.add(id.push);
    }
    builder.add(w.ast.member(id.__type__));
    if (tool[id.ast].has(w.id.primitiveTool)) {
        builder.add(w.ast.primitiveToolName(tool[id.ast]));
    } else {
        builder.add(w.ast._(std.op.Call));
        builder.add(w.ast.member(id.method));
        builder.add(w.op.const_(tool));
    }

    for (CellI& parameterKV : tool[id.parameters].__type__()[id.members]) {
        CellI& key = parameterKV[id.key];
        builder.add(w.ast.parameter(key));
        builder.add(memberIds.getValue(key));
    }
    if (hasReturnInEffect) {
        builder.add(id.pop);
        builder.add(w.ast.member(id.rhs));
        builder.add(memberIds.getValue(id.return_));
    }

    return builder;
}

// ============================================================================
List& ToolFinder::findToolsByDescription(CellI& effect, DescriptionKind descriptionKind)
{
    List& ret = *new List(w, std.ast.Base);
    if (checkUnknownsInTool(effect)) {
        TRACE(toolFinderLookup, "Unknown in the effect");
    } else {
        TRACE(toolFinderLookup, "Only constants in the effect");
    }
    auto buildersPtr = findBuildersForDescription(effect, descriptionKind);
    if (!buildersPtr) {
        return ret;
    }
    for (auto& builder : *buildersPtr) {
        TRACE(toolFinder, "build with {}", builder.label());
        Object retVal(w, std.op.ConstVar);
        buildTool({ retVal, w.ast.member(id.value), effect, builder });
        auto& tool = retVal[id.value];
        DEBUG(toolFinderLookup, "result: {}", tool.printAsValue());
        if (checkUnknownsInTool(tool)) {
            tool.set(id.state, std.op.State.missingInput);
            TRACE(toolFinderLookup, "Unknown in the result");
            ret.add(tool);
        } else {
            tool.set(id.state, std.op.State.ready);
            TRACE(toolFinderLookup, "Constant result");
            if (tool.__type__().has(id.returnType)) {
                tool(); // constatnt folding
                CellI& resultVar = w.op.const_(tool[id.value]);
                resultVar.set(id.type, tool[id.value].__type__());
                ret.add(resultVar);
            } else {
                ret.add(tool);
            }
        }
    }

    return ret;
}

// ============================================================================
std::unique_ptr<List> ToolFinder::findBuildersForDescription(CellI& description, DescriptionKind descriptionKind)
{
    TRACE(toolFinderExplore, "findBuildersForDescription {}", description.label());

    std::unique_ptr<List> ret = std::make_unique<List>(w, std.Cell);
    SolverState solverState(getRootNodeForDescriptionKind(descriptionKind), *this, description);

    solverState.m_processFn = [this](std::vector<std::function<void(SolverStateNode& solverNode, CellI& memberName, CellI& memberValue)>>& commandFns, SolverStateNode* solverNodePtr) {
        SolverPointer pointer = solverNodePtr->pointer();
        auto& solverNode      = *solverNodePtr;
        CellI* currentCellPtr = pointer.m_cellPtr;
        CellI& currentCell    = *currentCellPtr;
        CellI& memberKV       = (*pointer.m_memberNodePtr)[id.value];
        CellI& member         = memberKV[id.value];
        CellI& memberName     = member[id.name];
        CellI& memberRelation = member[id.relation];
        CellI& memberRole     = member[id.role];
        CellI& memberValue    = currentCell[memberName];

        if (&memberRelation == &std.op.Member.Relation.external) {
            if (&memberRole == &std.op.Member.Role.constValue) {
                commandFns[0](solverNode, memberName, memberValue);
            } else if (&memberRole == &std.op.Member.Role.constVarValue) {
                commandFns[0](solverNode, memberName, memberValue[id.value]);
            } else if (&memberRole == &std.op.Member.Role.input) {
                if (&memberValue.__type__() == &std.op.ConstVar) {
                    commandFns[1](solverNode, memberName, memberValue);
                } else if (&memberValue.__type__() == &std.op.UnknownVar) {
                    commandFns[2](solverNode, memberName, memberValue);
                } else {
                    commandFns[3](solverNode, memberName, memberValue);
                }
            } else {
                panic("Unprocessed member!");
            }
        } else {
            commandFns[4](solverNode, memberName, memberValue);
        }
    };
    auto& commandFns = solverState.m_commandFns;
    // 0
    commandFns.push_back([](SolverStateNode& solverNode, CellI& memberName, CellI& memberValue) {
        solverNode.checkKeyValue(memberName, memberValue);
    });
    // 1
    commandFns.push_back([this](SolverStateNode& solverNode, CellI& memberName, CellI& memberValue) {
        // there are two option here:
        solverNode.or_();

        // 1. this is a const variable
        auto& child1 = solverNode.addChild(solverNode);
        child1.checkKey(memberName);
        child1.checkKeyValue(id.op, id.variable);

        // 2. there is an explicit value in the search trie
        if (memberValue.has(id.value)) {
            auto& child2 = solverNode.addChild(solverNode);
            child2.checkKey(memberName);
            child2.checkKeyValue(id.op, id.value);
            child2.checkKey(memberValue[id.value]);
        }
    });
    // 2
    commandFns.push_back([this](SolverStateNode& solverNode, CellI& memberName, CellI& memberValue) {
        // there are two option here:
        solverNode.or_();

        // 1. this is a unknown variable
        auto& child1 = solverNode.addChild(solverNode);
        child1.checkKey(memberName);
        child1.checkKeyValue(id.op, id.variable);

        // 2. there are explicit values in the search trie
        //    so we continue with every possible values
        if (memberValue.has(id.value)) {
            Node& currentNode     = *solverNode.m_nodePtr;
            auto memberNameFindIt = currentNode.m_children.find(&memberName);
            if (memberNameFindIt != currentNode.m_children.end()) {
                Node& memberNode      = *memberNameFindIt->second;
                auto opFindIt = memberNode.m_children.find(&id.op);
                if (opFindIt != memberNode.m_children.end()) {
                    Node& opNode = *opFindIt->second;
                    auto valueFindIt = opNode.m_children.find(&id.value);
                    if (valueFindIt != opNode.m_children.end()) {
                        Node& valueNode = *valueFindIt->second;
                        for (auto& [key, nextNode] : valueNode.m_children) {
                            auto& child = solverNode.addChild(solverNode);
                            child.checkKey(memberName);
                            child.checkKeyValue(id.op, id.value);
                            child.checkAndCaptureValue(*key, memberValue[id.value]);
                        }
                    }
                }
            }
        }
    });
    // 3
    commandFns.push_back([this](SolverStateNode& solverNode, CellI& memberName, CellI& memberValue) {
        // there are two option here:
        solverNode.or_();

        // 1. this is a variable
        auto& child1 = solverNode.addChild(solverNode);
        child1.checkKey(memberName);
        child1.checkKeyValue(id.op, id.variable);

        // 2. this is a function
        auto& child2 = solverNode.addChild(solverNode);
        child2.checkKey(memberName);
        child2.checkKeyValue(id.op, id.push);
        child2.push();
    });
    // 4
    commandFns.push_back([this](SolverStateNode& solverNode, CellI& memberName, CellI& memberValue) {
        solverNode.checkKey(memberName);
        solverNode.checkKeyValue(id.op, id.push);
        solverNode.push();
    });
    solverState.m_filterFn = [this](CellI& cell, CellI& member) -> bool {
        if (cell.missing(member[id.name])) {
            return false;
        }
        CellI& memberRole = member[id.role];
        return &memberRole == &std.op.Member.Role.constValue || &memberRole == &std.op.Member.Role.constVarValue || &memberRole == &std.op.Member.Role.input;
    };
    solverState.m_popFn = [this](SolverStateNode& solverNode) {
        solverNode.checkKeyValue(id.op, id.pop);
    };
    solverState.m_resultFn = [this, &ret](SolverStateNode& solverNode) {
        Node& node = *solverNode.m_nodePtr;
        if (solverNode.m_capturedValue) {
            List& builder = *new List(w, std.Cell, fmt::format("builder for unfify {} with {}", solverNode.m_capturedVar->label(), solverNode.m_capturedValue->label()));

            builder.add(w.ast.member(id.__type__));
            builder.add(w.ast.primitiveToolName(std.op.Equal[id.ast]));
            builder.add(w.ast.member(id.lhs));
            builder.add(*solverNode.m_capturedPath);
            builder.add(w.ast.member(id.rhs));
            builder.add(w.op.const_(*solverNode.m_capturedValue));
            ret->add(builder);
            return;
        }
        for (auto& builder : *node.m_builders) {
            ret->add(builder);
        }
    };

    solverState.run();

    return ret;
}

// ============================================================================
List* ToolFinder::solve(CellI& equation)
{
    TRACE(toolFinderExplore, "solving equation: {}", equation.printAsValue());
    auto solvers = getSolvers(equation);
    if (solvers.empty()) {
        DEBUG(toolFinderExplore, "No solver for {}", equation.printAsValue());
        return nullptr;
    }

    List& ret = *new List(w, w.std.List, "solvers");
    for (auto solver : solvers) {
        Object resultVar(w, std.op.ConstVar, "solved");
        resultVar.set(id.value, equation);
        for (BuilderChainNode& builderChainNode : *solver) {
            if (builderChainNode.m_transformerBuilder) {
                buildTool({ resultVar, w.ast.member(id.value), resultVar[id.value], *builderChainNode.m_transformerBuilder });
                TRACE(toolFinderExplore, "transfered as: {}", resultVar[id.value].printAsValue());
            }
            if (builderChainNode.m_mutatingBuilders) {
                struct TreeNode
                {
                    CellI& m_outCell;
                    CellI& m_outKey;
                    CellI& m_cellToRebuildFrom;
                    CellI& m_builderNode;
                };
                std::list<TreeNode> nodes;
                nodes.push_back({ resultVar, id.value, resultVar[id.value], *builderChainNode.m_mutatingBuilders });
                bool first = true;

                int i = 1;
                while (!nodes.empty()) {
                    TreeNode& treeNode = nodes.front();
                    CellI& builderNode = treeNode.m_builderNode;
                    buildTool({ treeNode.m_outCell, w.ast.member(treeNode.m_outKey), treeNode.m_cellToRebuildFrom, builderNode[id.builder] });

                    TRACE(toolFinderExplore, "mutate step #{}: {}", i++, resultVar[id.value].printAsValue());

                    if (builderNode.missing(id.children)) {
                        nodes.pop_front();
                        continue;
                    }

                    auto& outCell = treeNode.m_outCell[treeNode.m_outKey];
                    auto& parent  = treeNode.m_cellToRebuildFrom;

                    for (auto& childBuilderNode : builderNode[id.children]) {
                        nodes.push_back({ outCell, childBuilderNode[id.transformedKey], parent[childBuilderNode[id.originalKey]], childBuilderNode });
                    }
                    nodes.pop_front();
                }
                TRACE(toolFinderExplore, "mutated as: {}", resultVar[id.value].printAsValue());
            }
        }
        ret.add(resultVar[id.value]);
    }

    return &ret;
}

// ============================================================================
void ToolFinder::addSolver(CellI& description, std::list<BuilderChainNode>& solver)
{
    TRACE(toolFinderExplore, "addSolver {}", description.label());
    std::deque<StackNode> stack;
    Node* currentNode    = getRootNodeForDescriptionKind(DescriptionKind::solver);
    CellI* memberItemPtr = &description.__type__()[id.memberIds][id.first];
    CellI* currentPtr    = &description;

    while (memberItemPtr) {
        CellI& current        = *currentPtr;
        CellI& memberItem     = *memberItemPtr;
        CellI& memberKV       = memberItem[id.value];
        CellI& member         = memberKV[id.value];
        CellI& memberName     = member[id.name];
        CellI& memberRelation = member[id.relation];
        CellI& memberRole     = member[id.role];

        if (&memberRelation == &std.op.Member.Relation.external) {
            if (&memberRole == &std.op.Member.Role.constValue) {
                addKeyWithConstValue(currentNode, memberName, current[memberName]);
            } else if (&memberRole == &std.op.Member.Role.constVarValue) {
                addKeyWithConstValue(currentNode, memberName, current[memberName][id.value]);
            } else if (&memberRole == &std.op.Member.Role.input) {
                CellI& memberValue = current[memberName];
                TRACE(toolFinderExplore, "addValue {}", memberName.label());
                addValue(currentNode, memberName);
                if (&memberValue.__type__() == &std.op.ConstVar) {
                    if (memberValue.has(id.value)) {
                        addKeyWithConstValue(currentNode, id.op, id.value);
                        addValue(currentNode, memberValue[id.value]);
                    } else {
                        addKeyWithConstValue(currentNode, id.op, id.push);
                        addKeyWithConstValue(currentNode, id.__type__, std.op.ConstVar);
                        addKeyWithConstValue(currentNode, id.op, id.pop);
                    }
                } else if (&memberValue.__type__() == &std.op.UnknownVar) {
                    addKeyWithConstValue(currentNode, id.op, id.push);
                    addKeyWithConstValue(currentNode, id.__type__, std.op.UnknownVar);
                    addKeyWithConstValue(currentNode, id.op, id.pop);
                } else {
                    addKeyWithConstValue(currentNode, id.op, id.push);
                    stack.push_back({ currentPtr, memberItemPtr, nullptr });
                    currentPtr    = &memberValue;
                    memberItemPtr = &memberValue.__type__()[id.memberIds][id.first];
                    continue;
                }
            }
        } else {
            CellI& memberValue = current[memberName];
            addKeyWithConstValue(currentNode, id.op, id.push);
            stack.push_back({ currentPtr, memberItemPtr, nullptr });
            currentPtr    = &memberValue;
            memberItemPtr = &memberValue.__type__()[id.memberIds][id.first];
            continue;
        }

        memberItemPtr = memberItemPtr->getNextOrNullptr();
        while (!memberItemPtr && !stack.empty()) {
            currentPtr    = stack.back().effectPtr;
            memberItemPtr = stack.back().slotItemPtr;
            stack.pop_back();
            addKeyWithConstValue(currentNode, id.op, id.pop);
            memberItemPtr = memberItemPtr->getNextOrNullptr();
        }
    }
    if (!currentNode->m_solver.empty()) {
        std::cout << "";
    }
    currentNode->m_isLeaf = 1;
    currentNode->m_effect = &description;
    currentNode->m_solver = solver;
}

// ============================================================================
std::ostream& operator<<(std::ostream& os, const ToolFinder::SolverStateNode::SubCommand::Kind& kind)
{
    using Kind = ToolFinder::SolverStateNode::SubCommand::Kind;

    switch (kind) {
    case Kind::nop:
        os << "nop";
        break;
    case Kind::checkKey:
        os << "checkKey";
        break;
    case Kind::checkKeyValue:
        os << "checkKeyValue";
        break;
    };

    return os;
}

// ============================================================================
std::ostream& operator<<(std::ostream& os, const ToolFinder::SolverStateNode::MatchStatus& matchStatus)
{
    using MatchStatus = ToolFinder::SolverStateNode::MatchStatus;

    switch (matchStatus) {
    case MatchStatus::created:
        os << "created";
        break;
    case MatchStatus::accepted:
        os << "accepted";
        break;
    case MatchStatus::checked:
        os << "checked";
        break;
    case MatchStatus::failed:
        os << "failed";
        break;
    case MatchStatus::finished:
        os << "finished";
        break;
    };

    return os;
}

// ============================================================================
std::ostream& operator<<(std::ostream& os, const ToolFinder::SolverStateNode::InputCommand& command)
{
    using Command = ToolFinder::SolverStateNode::InputCommand;

    switch (command) {
    case Command::and_:
        os << "and";
        break;
    case Command::or_:
        os << "or";
        break;
    case Command::push:
        os << "push";
        break;
    };

    return os;
}

// ============================================================================
ToolFinder::SolverState::SolverState(Node* rootNode, ToolFinder& toolFinder, CellI& description) :
    m_toolFinder(toolFinder),
    m_description(description)
{
    auto& id = description.w.id;
    SolverPointer solverPointer(m_description);
    m_startSolverNode = std::make_unique<SolverStateNode>(*this, solverPointer, rootNode, nullptr);
}

// ============================================================================
std::list<std::list<ToolFinder::BuilderChainNode>*>& ToolFinder::SolverState::results()
{
    return m_results;
}

// ============================================================================
void ToolFinder::SolverState::printAsDot()
{
    auto& id  = m_description.w.id;
    auto& std = m_description.w.std;

    std::stringstream ss;
    ss <<
        R"(digraph structs {
    node  [shape=plaintext]
    graph [fontname = "Helvetica",
           fontsize = 36
          ];

)";
    struct Edge
    {
        SolverStateNode* from;
        SolverStateNode* to;
    };
    std::deque<Edge> edges;
    std::set<SolverStateNode*> states;
    edges.push_back({ nullptr, &startSolverNode() });

    while (!edges.empty()) {
        Edge edge = edges.front();
        edges.pop_front();
        states.insert(edge.from);
        states.insert(edge.to);
        for (auto& child : edge.to->m_children) {
            edges.push_back({ edge.to, child.get() });
        }
    }
    for (SolverStateNode* stateNodePtr : states) {
        if (!stateNodePtr) {
            ss << fmt::format(R"("state{}" [shape=Mdiamond style=filled,color="#bde0fe" label=Start];
)",
                              fmt::ptr(stateNodePtr));
            continue;
        }
        SolverStateNode& stateNode = *stateNodePtr;
        ss << fmt::format(
            R"("state{}" [label=<
              <TABLE BORDER="0" CELLBORDER="0">
                  <TR>
                      <TD COLSPAN="5" bgcolor="#a2d2ff">{}</TD>
                  </TR>
                  <TR>
                      <TD COLSPAN="1" bgcolor="wheat"></TD>
                      <TD COLSPAN="2" bgcolor="wheat">{}</TD>
                      <TD COLSPAN="2" bgcolor="wheat">{}</TD>
                  </TR>
)",
            fmt::ptr(stateNodePtr), (*stateNode.m_solverPointer.m_cellPtr).__type__().label(), stateNode.m_matchStatus, stateNode.m_inputCommand);
        CellI& currentMemberName = stateNode.m_solverPointer.memberName();
        int i = 0;
        for (auto& memberKV : (*stateNode.m_solverPointer.m_cellPtr).__type__()[id.memberIds]) {
            CellI& member             = memberKV[id.value];
            CellI& memberName         = member[id.name];
            CellI& memberRelation     = member[id.relation];
            CellI& memberRole         = member[id.role];
            std::string memberNameStr = memberName.label();

            if (!(&memberRole == &std.op.Member.Role.constValue || &memberRole == &std.op.Member.Role.constVarValue || &memberRole == &std.op.Member.Role.input)) {
                continue;
            }
            std::string bgcolor;
            if (&currentMemberName == &memberName) {
                bgcolor = stateNode.m_matchStatus == SolverStateNode::MatchStatus::failed ? R"(bgcolor="orangered")" : R"(bgcolor="#a7c957")";
                auto subCommandsNum = stateNode.m_subCommands.size();
                ss << fmt::format(
                    R"(
                  <TR>
                      <TD COLSPAN="1" ROWSPAN="{}" bgcolor="lightgoldenrod2">{}</TD>
                      <TD COLSPAN="2" ROWSPAN="{}" {}>{}</TD>
                      <TD COLSPAN="1">{}</TD>
                      <TD COLSPAN="1">{}</TD>
                  </TR>
)",
                    subCommandsNum, i++,
                    subCommandsNum, bgcolor, memberNameStr,
                    stateNode.m_subCommands.empty() ? "" : stateNode.m_subCommands.front().printKey1(),
                    stateNode.m_subCommands.empty() ? "" : stateNode.m_subCommands.front().printKey2());
                bool first = true;
                for (auto subCommand : stateNode.m_subCommands) {
                    if (first) {
                        first = false;
                        continue;
                    }
                    ss << fmt::format(
                        R"(
                  <TR>
                          <TD COLSPAN="1">{}</TD>
                          <TD COLSPAN="1">{}</TD>
                  </TR>
)",
                        subCommand.printKey1(), subCommand.printKey2());
                }
            } else {
                ss << fmt::format(
                    R"(
                  <TR>
                      <TD COLSPAN="1" bgcolor="lightgoldenrod2">{}</TD>
                      <TD COLSPAN="2" {}>{}</TD>
                      <TD COLSPAN="2"></TD>
                  </TR>
)",
                    i++,
                    bgcolor, memberNameStr);
            }
        }

        ss << fmt::format(
            R"(
              </TABLE>
         >];
)");
    }

    edges.push_back({ nullptr, &startSolverNode() });
    while (!edges.empty()) {
        Edge edge = edges.front();
        edges.pop_front();
        states.insert(edge.from);
        states.insert(edge.to);

        ss << fmt::format("\"state{}\" -> \"state{}\"\n", fmt::ptr(edge.from), fmt::ptr(edge.to));
        for (auto& child : edge.to->m_children) {
            edges.push_back({ edge.to, child.get() });
        }
    }

    ss << "}";

    // offline graphviz: https://graphviz.org/download/
    //  online graphviz: https://dreampuf.github.io/GraphvizOnline/
    TRACE(toolFinderGraphviz, "\n{}", ss.str());
}

// ============================================================================
ToolFinder::SolverStateNode& ToolFinder::SolverState::startSolverNode()
{
    return *m_startSolverNode;
}

// ============================================================================
void ToolFinder::SolverState::run()
{
    TRACE(toolFinderExplore, "SolverState::run");

    auto& id  = m_description.w.id;
    auto& std = m_description.w.std;

    SolverStateNode* startSolverNodePtr = &startSolverNode();
    m_nextStates.push_back(startSolverNodePtr);

    while (!m_nextStates.empty()) {
        SolverStateNode* solverNodePtr = m_nextStates.front();
        m_nextStates.pop_front();

        if (solverNodePtr->m_matchStatus == SolverStateNode::MatchStatus::accepted) {
            SolverStateNode& lastSolverNode = *solverNodePtr;
            solverNodePtr = solverNodePtr->step();
            printAsDot();
            if (lastSolverNode.m_matchStatus == SolverStateNode::MatchStatus::finished) {
                TRACE(toolFinderExplore, "getSolvers solver found");
                m_resultFn(lastSolverNode);
                continue;
            }
            if (lastSolverNode.m_matchStatus == SolverStateNode::MatchStatus::failed) {
                TRACE(toolFinderExplore, "getSolvers deadend");
                continue;
            }
        }
        if (solverNodePtr->m_inputCommand == SolverStateNode::InputCommand::or_) {
            continue;
        }
        m_processFn(m_commandFns, solverNodePtr);
        addNextState(*solverNodePtr);
    }
}

// ============================================================================
void ToolFinder::SolverState::addNextState(SolverStateNode& solverStateNode)
{
    TRACE(toolFinderExplore, "getSolvers accepted: {}, {}", solverStateNode.m_inputCommand, solverStateNode.m_solverPointer.printKV());
    solverStateNode.m_matchStatus = SolverStateNode::MatchStatus::accepted;
    m_nextStates.push_back(&solverStateNode);
}

// ============================================================================
ToolFinder::SolverPointer::SolverPointer() :
    m_cellPtr(nullptr), m_memberNodePtr(nullptr), m_parent(nullptr)
{
}

// ============================================================================
ToolFinder::SolverPointer::SolverPointer(CellI& description)
{
    if (description.label() == "mapObj") {
        std::cout << "";
    }
    auto& id        = description.w.id;
    m_cellPtr       = &description;
    m_memberNodePtr = &(description).__type__()[id.memberIds][id.first];
}

// ============================================================================
ToolFinder::SolverPointer::SolverPointer(SolverPointer* parent, CellI* cellPtr, CellI* memberNodePtr) :
    m_parent(parent), m_cellPtr(cellPtr), m_memberNodePtr(memberNodePtr)
{

}

// ============================================================================
CellI& ToolFinder::SolverPointer::memberName()
{
    auto& id          = (*m_cellPtr).w.id;
    CellI& memberNode = *m_memberNodePtr;
    CellI& memberKV   = memberNode[id.value];
    CellI& member     = memberKV[id.value];

    return member[id.name];
}

// ============================================================================
CellI& ToolFinder::SolverPointer::memberValue()
{
    CellI& current = *m_cellPtr;
    return current[memberName()];
}

// ============================================================================
std::string ToolFinder::SolverPointer::printKV()
{
    return fmt::format("{}:{}", (*m_cellPtr).__type__().label(), memberName().label());
}

// ============================================================================
ToolFinder::SolverPointer ToolFinder::SolverPointer::step(SolverStateNode& solverStateNode, int& popCount)
{
    auto& id  = (*m_cellPtr).w.id;
    auto& std = (*m_cellPtr).w.std;
    auto& filterFn = solverStateNode.m_state.m_filterFn;

    if (solverStateNode.m_inputCommand == SolverStateNode::InputCommand::push) {
        CellI* cellPtr       = &memberValue();
        CellI* memberNodePtr = &(*cellPtr).__type__()[id.memberIds][id.first];
        return { this, cellPtr, memberNodePtr };
    }

    CellI* cellPtr        = m_cellPtr;
    CellI* memberNodePtr  = m_memberNodePtr;
    SolverPointer* parent = m_parent;

    while (true) {
        memberNodePtr = memberNodePtr->getNextOrNullptr();
        while (!memberNodePtr && parent) {
            cellPtr       = parent->m_cellPtr;
            memberNodePtr = parent->m_memberNodePtr;
            parent        = parent->m_parent;
            ++popCount;
            memberNodePtr = memberNodePtr->getNextOrNullptr();
        }

        if (!memberNodePtr) {
            return { parent, cellPtr, memberNodePtr };
        }

        CellI& memberKV   = (*memberNodePtr)[id.value];
        CellI& member     = memberKV[id.value];

        if (filterFn(*cellPtr, member)) {
            return { parent, cellPtr, memberNodePtr };
        }
    }

    return SolverPointer();
}

// ============================================================================
bool ToolFinder::SolverPointer::isLast()
{
    return !m_cellPtr || !m_memberNodePtr;
}

// ============================================================================
ToolFinder::SolverStateNode::SolverStateNode(SolverState& state, SolverPointer solverPointer, Node* nodePtr, SolverStateNode* parent) :
    m_state(state),
    m_inputCommand(InputCommand::and_),
    m_nodePtr(nodePtr),
    m_parent(parent),
    m_solverPointer(solverPointer),
    m_matchStatus(MatchStatus::created),
    m_capturedPath(nullptr),
    m_capturedValue(nullptr),
    m_capturedVar(nullptr)
{
}

void ToolFinder::SolverStateNode::checkKey(CellI& key)
{
    m_subCommands.emplace_back(SubCommand::Kind::checkKey, &key, nullptr);
}

void ToolFinder::SolverStateNode::checkAndCaptureValue(CellI& key, CellI& value)
{
    m_subCommands.emplace_back(SubCommand::Kind::checkAndCaptureValue, &key, &value);
}

void ToolFinder::SolverStateNode::checkKeyValue(CellI& key, CellI& value)
{
    m_subCommands.emplace_back(SubCommand::Kind::checkKeyValue, &key, &value);
}

void ToolFinder::SolverStateNode::or_()
{
    m_inputCommand = InputCommand::or_;
}

void ToolFinder::SolverStateNode::push()
{
    m_inputCommand = InputCommand::push;
}

bool ToolFinder::SolverStateNode::empty()
{
    return m_subCommands.empty();
}

bool ToolFinder::SolverStateNode::evaluate()
{
    bool ret = true;
    for (auto& command : m_subCommands) {
        ret &= command.evaluate(m_state.m_toolFinder, m_nodePtr);
        if (ret == false) {
            return ret;
        }
        if (command.m_kind == SubCommand::Kind::checkAndCaptureValue) {
            if (m_capturedValue || m_capturedVar) {
                if (m_capturedValue != command.m_key1) {
                    return false;
                }
                if (m_capturedVar != command.m_key2) {
                    return false;
                }
            } else {
                m_capturedValue = command.m_key1;
                m_capturedVar   = command.m_key2;
                m_capturedPath  = saveCurrentPath();
            }
        }
    }

    return ret;
}

CellI* ToolFinder::SolverStateNode::saveCurrentPath()
{
    auto& w   = m_state.m_toolFinder.w;
    List& ret = *new List(w, w.std.Cell, fmt::format("path for {}", m_capturedVar->label()));
    std::deque<SolverPointer*> stack;
    SolverPointer* solverPointer = &m_solverPointer;
    while (solverPointer) {
        stack.push_front(solverPointer);
        solverPointer = solverPointer->m_parent;
    }
    for (SolverPointer* solverPointerPtr : stack) {
        CellI& key = (*solverPointerPtr->m_memberNodePtr)[w.id.value][w.id.key];
        ret.add(w.ast.parameter(key));
    }

    return &ret;
}

bool ToolFinder::SolverStateNode::SubCommand::evaluate(ToolFinder& toolFinder, Node*& node)
{
    switch (m_kind) {
    case Kind::nop:
        return true;
    case Kind::checkKey:
        return toolFinder.checkConstValue(node, *m_key1);
    case Kind::checkAndCaptureValue: {
        bool ret = toolFinder.checkConstValue(node, *m_key1);
        return ret;
    }
    case Kind::checkKeyValue:
        return toolFinder.checkConstKeyValue(node, *m_key1, *m_key2);
    };

    return false;
}

std::string ToolFinder::SolverStateNode::SubCommand::printKey1()
{
    if (!m_key1) {
        return "";
    }
    return (*m_key1).label();
}

std::string ToolFinder::SolverStateNode::SubCommand::printKey2()
{
    if (!m_key2) {
        return "";
    }
    return (*m_key2).label();
}

ToolFinder::SolverStateNode& ToolFinder::SolverStateNode::addNext(SolverStateNode*& solverNodePtr)
{
    m_children.push_back(std::make_unique<SolverStateNode>(m_state, solverNodePtr->m_solverPointer, solverNodePtr->m_nodePtr, this));
    auto* ret            = m_children.back().get();
    ret->m_capturedPath  = solverNodePtr->m_capturedPath;
    ret->m_capturedValue = solverNodePtr->m_capturedValue;
    ret->m_capturedVar   = solverNodePtr->m_capturedVar;
    solverNodePtr        = ret;

    return *ret;
}

ToolFinder::SolverStateNode& ToolFinder::SolverStateNode::addChild(SolverStateNode& solverNode)
{
    m_children.push_back(std::make_unique<SolverStateNode>(m_state, solverNode.m_solverPointer, solverNode.m_nodePtr, this));
    auto* ret = m_children.back().get();
    ret->m_parent = this;

    return *ret;
}

ToolFinder::SolverStateNode* ToolFinder::SolverStateNode::step()
{
    auto& id    = m_state.m_toolFinder.id;
    auto& popFn = m_state.m_popFn;

    if (!evaluate()) {
        m_matchStatus = MatchStatus::failed;
        return nullptr;
    }

    if (m_inputCommand == InputCommand::or_) {
        m_matchStatus = SolverStateNode::MatchStatus::checked;
        SolverStateNode* firstChild = nullptr;
        for (auto& child : m_children) {
            child->m_nodePtr = m_nodePtr;
            child->m_capturedPath  = m_capturedPath;
            child->m_capturedValue = m_capturedValue;
            child->m_capturedVar   = m_capturedVar;
            m_state.addNextState(*child);
        }
        return this;
    }

    int popCount = 0;
    SolverPointer nextPointer = pointer().step(*this, popCount);
    if (nextPointer.isLast()) {
        m_matchStatus = MatchStatus::finished;
        return nullptr;
    }
    m_matchStatus = MatchStatus::checked;

    SolverStateNode* solverNodePtr = this;
    solverNodePtr->addNext(solverNodePtr).pointer(nextPointer);
    for (int i = 0; i < popCount; ++i) {
        popFn(*solverNodePtr);
    }

    return solverNodePtr;
}

ToolFinder::SolverPointer& ToolFinder::SolverStateNode::pointer()
{
    return m_solverPointer;
}

void ToolFinder::SolverStateNode::pointer(CellI& description)
{
    m_solverPointer = SolverPointer(description);
}

void ToolFinder::SolverStateNode::pointer(SolverPointer& solverPointer)
{
    m_solverPointer = solverPointer;
}

// ============================================================================
std::list<std::list<ToolFinder::BuilderChainNode>*> ToolFinder::getSolvers(CellI& description)
{
    TRACE(toolFinderExplore, "getSolvers {}", description.label());

    std::list<std::list<ToolFinder::BuilderChainNode>*> results;
    SolverState solverState(getRootNodeForDescriptionKind(DescriptionKind::solver), *this, description);

    solverState.m_processFn = [this](std::vector<std::function<void(SolverStateNode& solverNode, CellI& memberName, CellI& memberValue)>>& commandFns, SolverStateNode* solverNodePtr) {
        SolverPointer pointer = solverNodePtr->pointer();
        auto& solverNode      = *solverNodePtr;
        CellI* currentCellPtr = pointer.m_cellPtr;
        CellI& currentCell    = *currentCellPtr;
        CellI& memberKV       = (*pointer.m_memberNodePtr)[id.value];
        CellI& member         = memberKV[id.value];
        CellI& memberName     = member[id.name];
        CellI& memberRelation = member[id.relation];
        CellI& memberRole     = member[id.role];
        CellI& memberValue    = currentCell[memberName];

        if (&memberRelation == &std.op.Member.Relation.external) {
            if (&memberRole == &std.op.Member.Role.constValue) {
                commandFns[0](solverNode, memberName, memberValue);
            } else if (&memberRole == &std.op.Member.Role.constVarValue) {
                commandFns[0](solverNode, memberName, memberValue[id.value]);
            } else if (&memberRole == &std.op.Member.Role.input) {
                if (&memberValue.__type__() == &std.op.ConstVar) {
                    commandFns[1](solverNode, memberName, memberValue);
                } else if (&memberValue.__type__() == &std.op.UnknownVar) {
                    commandFns[2](solverNode, memberName, memberValue);
                } else {
                    if (memberValue.has(id.state) && (&memberValue[id.state] == &std.op.State.missingInput)) {
                        commandFns[3](solverNode, memberName, memberValue);
                    } else {
                        commandFns[4](solverNode, memberName, memberValue);
                    }
                }
            } else {
                panic("Unprocessed member!");
            }
        } else {
            commandFns[4](solverNode, memberName, memberValue);
        }
    };
    auto& commandFns = solverState.m_commandFns;
    // 0
    commandFns.push_back([](SolverStateNode& solverNode, CellI& memberName, CellI& memberValue) {
        solverNode.checkKeyValue(memberName, memberValue);
    });
    // 1
    commandFns.push_back([this](SolverStateNode& solverNode, CellI& memberName, CellI& memberValue) {
        solverNode.or_();

        auto& child1 = solverNode.addChild(solverNode);
        child1.checkKey(memberName);
        child1.checkKeyValue(id.op, id.value);
        child1.checkKey(memberValue[id.value]);

        auto& child2 = solverNode.addChild(solverNode);
        child2.checkKey(memberName);
        child2.checkKeyValue(id.op, id.push);
        child2.checkKeyValue(id.__type__, std.op.ConstVar);
        child2.checkKeyValue(id.op, id.pop);
    });
    // 2
    commandFns.push_back([this](SolverStateNode& solverNode, CellI& memberName, CellI& memberValue) {
        solverNode.checkKey(memberName);
        solverNode.checkKeyValue(id.op, id.push);
        solverNode.checkKeyValue(id.__type__, std.op.UnknownVar);
        solverNode.checkKeyValue(id.op, id.pop);
    });
    // 3
    commandFns.push_back([this](SolverStateNode& solverNode, CellI& memberName, CellI& memberValue) {
        // there are two option here:
        solverNode.or_();

        // 1. this is an uninitialized variable
        auto& child1 = solverNode.addChild(solverNode);
        child1.checkKey(memberName);
        child1.checkKeyValue(id.op, id.push);
        child1.checkKeyValue(id.__type__, std.op.UnknownVar);
        child1.checkKeyValue(id.op, id.pop);

        // 2. this is a function which depends on an uninitialized variable
        auto& child2 = solverNode.addChild(solverNode);
        child2.checkKey(memberName);
        child2.checkKeyValue(id.op, id.push);
        child2.push(); });
    // 4
    commandFns.push_back([this](SolverStateNode& solverNode, CellI& memberName, CellI& memberValue) {
        solverNode.checkKey(memberName);
        solverNode.checkKeyValue(id.op, id.push);
        solverNode.push();
    });
    solverState.m_filterFn = [this](CellI& cell, CellI& member) -> bool {
        CellI& memberRole = member[id.role];
        return &memberRole == &std.op.Member.Role.constValue || &memberRole == &std.op.Member.Role.constVarValue || &memberRole == &std.op.Member.Role.input;
    };
    solverState.m_popFn = [this](SolverStateNode& solverNode) {
        solverNode.checkKeyValue(id.op, id.pop);
    };
    solverState.m_resultFn = [&results](SolverStateNode& solverNode) {
        Node& node = *solverNode.m_nodePtr;
        results.push_back(&node.m_solver);
    };

    solverState.run();

    return results;
}

struct SolverStackNode
{
    CellI* effectPtr   = nullptr;
    CellI* slotItemPtr = nullptr;
};

// ============================================================================
void ToolFinder::addPermutation(Node* rootNode, CellI& description)
{
    std::deque<StackNode> stack;
    Node* currentNode    = rootNode;
    CellI* memberItemPtr = &description.__type__()[id.memberIds][id.first];
    CellI* currentPtr    = &description;

    while (memberItemPtr) {
        CellI& current        = *currentPtr;
        CellI& memberItem     = *memberItemPtr;
        CellI& memberKV       = memberItem[id.value];
        CellI& member         = memberKV[id.value];
        CellI& memberName     = member[id.name];
        CellI& memberRelation = member[id.relation];
        CellI& memberRole     = member[id.role];

        if (&memberRelation == &std.op.Member.Relation.external) {
            if (&memberRole == &std.op.Member.Role.constValue) {
                addKeyWithConstValue(currentNode, memberName, current[memberName]);
            } else if (&memberRole == &std.op.Member.Role.constValue) {
                addKeyWithConstValue(currentNode, memberName, current[memberName][id.value]);
            } else if (&memberRole == &std.op.Member.Role.input) {
                addValue(currentNode, memberName);
                CellI& memberValue = current[memberName];
                if ((&memberValue.__type__() == &std.op.ConstVar) || (&memberValue.__type__() == &std.op.UnknownVar)) {
                    addKeyWithConstValue(currentNode, memberName, memberValue);
                } else {
                    addKeyWithConstValue(currentNode, id.op, id.push);
                    stack.push_back({ currentPtr, memberItemPtr, nullptr });
                    currentPtr    = &memberValue;
                    memberItemPtr = &memberValue.__type__()[id.memberIds][id.first];
                    continue;
                }
            }
        } else {
            CellI& memberValue = current[memberName];
            addKeyWithConstValue(currentNode, id.op, id.push);
            stack.push_back({ currentPtr, memberItemPtr, nullptr });
            currentPtr    = &memberValue;
            memberItemPtr = &memberValue.__type__()[id.memberIds][id.first];
            continue;
        }

        memberItemPtr = memberItemPtr->getNextOrNullptr();
        while (!memberItemPtr && !stack.empty()) {
            currentPtr   = stack.back().effectPtr;
            memberItemPtr = stack.back().slotItemPtr;
            stack.pop_back();
            addKeyWithConstValue(currentNode, id.op, id.pop);
            memberItemPtr = memberItemPtr->getNextOrNullptr();
        }
    }
    currentNode->m_isLeaf = 1;
    currentNode->m_effect = &description;
}

// ============================================================================
bool ToolFinder::checkConstValue(Node*& node, CellI& value)
{
    auto keyFindIt = node->m_children.find(&value);
    if (keyFindIt == node->m_children.end()) {
        return false;
    }
    TRACE(toolFinderLookup, "MATCH: {}", value.label());
    node = keyFindIt->second;
    return true;
}

// ============================================================================
bool ToolFinder::checkConstKeyValue(Node*& node, CellI& key, CellI& value)
{
    TRACE(toolFinderExplore, "checkConstKeyValue {}: {}", key.label(), value.label());
    // __type__ is a special key as it can not be a key in a trie node so escaped with "op type"
    if (&key == &id.__type__) {
        auto opFindIt = node->m_children.find(&id.op);
        if (opFindIt != node->m_children.end()) {
            Node* opNode = opFindIt->second;
            for (auto& [opKey, nextNode] : opNode->m_children) {
                if (opKey == &id.type) {
                    node             = nextNode;
                    auto valueFindIt = node->m_children.find(&value);
                    if (valueFindIt != node->m_children.end()) {
                        TRACE(toolFinderLookup, "MATCH: op");
                        TRACE(toolFinderLookup, "MATCH: type");
                        TRACE(toolFinderLookup, "MATCH: {}", value.label());
                        node = valueFindIt->second;
                        return true;
                    }
                }
            }
        }
        return false;
    } else {
        auto keyFindIt = node->m_children.find(&key);
        if (keyFindIt == node->m_children.end()) {
            return false;
        } else {
            TRACE(toolFinderLookup, "MATCH: {}", key.label());
            node = keyFindIt->second;
        }
    }

    // ok, key was found, now check the value
    auto findIt = node->m_children.find(&value);
    if (findIt == node->m_children.end()) {
        return false;
    } else {
        TRACE(toolFinderLookup, "MATCH: {}", value.label());
        node = findIt->second;
        return true;
    }

    return false;
}

// ============================================================================
bool ToolFinder::hasPermutation(Node* rootNode, CellI& description)
{
    std::deque<StackNode> stack;
    Node* currentNode    = rootNode;
    CellI* memberItemPtr = &description.__type__()[id.memberIds][id.first];
    CellI* currentPtr    = &description;

    while (memberItemPtr) {
        CellI& current        = *currentPtr;
        CellI& memberItem     = *memberItemPtr;
        CellI& memberKV       = memberItem[id.value];
        CellI& member         = memberKV[id.value];
        CellI& memberName     = member[id.name];
        CellI& memberRelation = member[id.relation];
        CellI& memberRole     = member[id.role];

        if (&memberRelation == &std.op.Member.Relation.external) {
            if (&memberRole == &std.op.Member.Role.constValue) {
                CellI& memberValue = current[memberName];
                if (!checkConstKeyValue(currentNode, memberName, memberValue)) {
                    return false;
                }
            } else if (&memberRole == &std.op.Member.Role.constValue) {
                CellI& memberValue = current[memberName];
                if (!checkConstKeyValue(currentNode, memberName, memberValue[id.value])) {
                    return false;
                }
            } else if (&memberRole == &std.op.Member.Role.input) {
                if (!checkConstValue(currentNode, memberName)) {
                    return false;
                }
                CellI& memberValue = current[memberName];
                if ((&memberValue.__type__() == &std.op.ConstVar) || (&memberValue.__type__() == &std.op.UnknownVar)) {
                    if (!checkConstKeyValue(currentNode, memberName, memberValue)) {
                        return false;
                    }
                } else {
                    if (!checkConstKeyValue(currentNode, id.op, id.push)) {
                        return false;
                    }
                    stack.push_back({ currentPtr, memberItemPtr, nullptr });
                    currentPtr    = &memberValue;
                    memberItemPtr = &memberValue.__type__()[id.memberIds][id.first];
                    continue;
                }
            }
        } else {
            CellI& memberValue = current[memberName];
            if (!checkConstKeyValue(currentNode, id.op, id.push)) {
                return false;
            }
            stack.push_back({ currentPtr, memberItemPtr, nullptr });
            currentPtr    = &memberValue;
            memberItemPtr = &memberValue.__type__()[id.memberIds][id.first];
            continue;
        }

        memberItemPtr = memberItemPtr->getNextOrNullptr();
        while (!memberItemPtr && !stack.empty()) {
            currentPtr    = stack.back().effectPtr;
            memberItemPtr = stack.back().slotItemPtr;
            stack.pop_back();
            if (!checkConstKeyValue(currentNode, id.op, id.pop)) {
                return false;
            }
            memberItemPtr = memberItemPtr->getNextOrNullptr();
        }
    }
    if (currentNode && currentNode->m_isLeaf) {
        return true;
    }
    return false;
}

// ============================================================================
bool ToolFinder::isConstantFoldingPossible(CellI& description)
{
    std::deque<CellI*> cellsToInspect;
    cellsToInspect.push_back(&description);

    while (!cellsToInspect.empty()) {
        CellI& currentCell = *cellsToInspect.back();
        cellsToInspect.pop_back();

        bool allInputMemberIsConst = true;
        for (auto& memberKV : currentCell.__type__()[id.members]) {
            auto& member = memberKV[id.value];
            if (&member[id.role] != &std.op.Member.Role.input) {
                continue;
            }
            CellI& memberName = member[id.name];
            CellI& inputValue = currentCell[memberName];
            if (&inputValue.__type__() != &std.op.ConstVar) {
                allInputMemberIsConst = false;
            }
            if ((&inputValue.__type__() != &std.op.ConstVar) && (&inputValue.__type__() != &std.op.UnknownVar)) {
                cellsToInspect.push_back(&inputValue);
                break;
            }
        }
        if (allInputMemberIsConst) {
            return true;
        }
    }

    return false;
}

struct FoldingData
{
    CellI& m_cell;
    CellI* m_parent;
    CellI* m_parentKey;
};

// ============================================================================
CellI& ToolFinder::doConstantFolding(CellI& description)
{
    std::deque<FoldingData> cellsToInspect;
    cellsToInspect.push_back({ description, nullptr, nullptr });
    CellI* ret = &description;

    while (!cellsToInspect.empty()) {
        CellI& currentCell = cellsToInspect.back().m_cell;
        CellI* parent      = cellsToInspect.back().m_parent;
        CellI* parentKey   = cellsToInspect.back().m_parentKey;
        cellsToInspect.pop_back();

        bool allInputMemberIsConst = true;
        for (auto& memberKV : currentCell.__type__()[id.members]) {
            auto& member = memberKV[id.value];
            if (&member[id.role] != &std.op.Member.Role.input) {
                continue;
            }
            CellI& memberName = member[id.name];
            CellI& inputValue = currentCell[memberName];
            if (&inputValue.__type__() != &std.op.ConstVar) {
                allInputMemberIsConst = false;
            }
            if ((&inputValue.__type__() != &std.op.ConstVar) && (&inputValue.__type__() != &std.op.UnknownVar)) {
                cellsToInspect.push_back({ inputValue, &currentCell, &memberName });
                break;
            }
        }
        if (allInputMemberIsConst) {
            currentCell();
            CellI& simplifiedTool = currentCell[id.value];
            if (parent) {
                (*parent).set(*parentKey, w.op.const_(simplifiedTool));
            } else {
                return simplifiedTool;
            }
        }
    }

    return *ret;
}

// ============================================================================
class RecombineInfo
{
public:
    RecombineInfo(CellI& outCell) :
        w(outCell.w),
        m_parent(nullptr),
        m_outCellPtr(&outCell),
        m_outKeyPtr(nullptr),
        m_referenceOutKeyPtr(nullptr),
        m_cellToRecombine(nullptr),
        m_firstBuilderNodePtr(nullptr),
        m_builderNodePtr(nullptr),
        m_firstMappingNodePtr(nullptr),
        m_mappingNodePtr(nullptr)
    {
    }

    RecombineInfo(RecombineInfo& parent, CellI& outKey, CellI& cellToRecombine) :
        w(parent.w),
        m_parent(&parent),
        m_outCellPtr(nullptr),
        m_outKeyPtr(nullptr),
        m_referenceOutKeyPtr(&outKey),
        m_cellToRecombine(&cellToRecombine),
        m_firstBuilderNodePtr(&cellToRecombine.__type__()[w.id.selfBuilders][w.id.builder][w.id.first]),
        m_builderNodePtr(m_firstBuilderNodePtr),
        m_firstMappingNodePtr(&cellToRecombine.__type__()[w.id.selfBuilders][w.id.memberMapping][w.id.first]),
        m_mappingNodePtr(m_firstMappingNodePtr)
    {
    }

    void setParent()
    {
        m_parent->m_children.push_back(this);
    }

    bool prepareBuild()
    {
        if (!m_parent) {
            return false;
        }
        m_outCellPtr = m_parent->outCell();
        m_outKeyPtr  = m_parent->mappedKey(*m_referenceOutKeyPtr);

        return true;
    }

    CellI* outCell()
    {
        if (!m_parent) {
            return m_outCellPtr;
        }
        return &(*m_outCellPtr)[*m_outKeyPtr];
    }

    CellI* mappedKey(CellI& key)
    {
        if (!m_parent || m_mappingNodePtr == m_firstMappingNodePtr) {
            return &key;
        } else {
            auto& mapping = static_cast<Map&>((*m_mappingNodePtr)[w.id.value]);
            return &mapping.getValue(key);
        }
    }

    bool step() // true if wrap around, so go from last to first
    {
        if (!m_parent) {
            return true;
        }
        if (CellI* nextOrNull = m_builderNodePtr->getNextOrNullptr()) {
            m_builderNodePtr = nextOrNull;
            m_mappingNodePtr = m_mappingNodePtr->getNextOrNullptr();
            return false;
        } else {
            m_builderNodePtr = m_firstBuilderNodePtr;
            m_mappingNodePtr = m_firstMappingNodePtr;
            return true;
        }
    }

    bool isLast()
    {
        return m_parent == nullptr;
    }

    World& w;
    RecombineInfo* m_parent;
    std::list<RecombineInfo*> m_children;
    CellI* m_outCellPtr;
    CellI* m_outKeyPtr;
    CellI* m_referenceOutKeyPtr;
    CellI* m_cellToRecombine;
    CellI* m_firstBuilderNodePtr;
    CellI* m_builderNodePtr;
    CellI* m_firstMappingNodePtr;
    CellI* m_mappingNodePtr;
};

// ============================================================================
std::list<ToolFinder::RecombineResult> ToolFinder::recombine(Node* rootNode, CellI& description)
{
    std::deque<RecombineInfo> recombineInfos;
    Object outCell(w, std.op.ConstVar);
    std::list<ToolFinder::RecombineResult> ret;
    recombineInfos.push_back({ outCell });
    recombineInfos.push_back({ recombineInfos.back(), id.value, description });
    recombineInfos.back().setParent();

    std::deque<CellI*> cellsToInspect;
    cellsToInspect.push_back(&description);

    while (!cellsToInspect.empty()) {
        CellI& currentCell = *cellsToInspect.back();
        cellsToInspect.pop_back();

        auto& parentInfo = recombineInfos.back();
        for (auto& memberKV : currentCell.__type__()[id.members]) {
            auto& member = memberKV[id.value];
            if (&member[id.role] != &std.op.Member.Role.input) {
                continue;
            }
            CellI& memberName = member[id.name];
            CellI& inputValue = currentCell[memberName];
            if ((&inputValue.__type__() != &std.op.ConstVar) && (&inputValue.__type__() != &std.op.UnknownVar)) {
                cellsToInspect.push_back(&inputValue);
                recombineInfos.push_back({ parentInfo, memberName, inputValue });
                recombineInfos.back().setParent();
            }
        }
    }
    bool hasMorePermutation = true;
    while (hasMorePermutation) {
        for (auto& recombineInfo : recombineInfos) {
            if (recombineInfo.prepareBuild()) {
                buildTool({ *recombineInfo.m_outCellPtr, w.ast.member(*recombineInfo.m_outKeyPtr), *recombineInfo.m_cellToRecombine, (*recombineInfo.m_builderNodePtr)[id.value] });
            }
        }
        CellI& tool = outCell[id.value];
        if (!hasPermutation(rootNode, tool)) {
            addPermutation(rootNode, tool);
            struct TreeNode
            {
                RecombineInfo& m_recombineInfo;
                Object& m_builderNode;
            };
            std::list<TreeNode> nodes;
            RecombineInfo& firstRecombineInfo = recombineInfos.at(1);
            Object& firstBuilderNode          = *new Object(w, std.op.BuilderNode);
            firstBuilderNode.set(id.originalKey, id.emptyObject);
            firstBuilderNode.set(id.transformedKey, id.emptyObject);
            firstBuilderNode.set(id.builder, (*firstRecombineInfo.m_builderNodePtr)[id.value]);
            nodes.push_back({ firstRecombineInfo, firstBuilderNode });
            bool first = true;

            while (!nodes.empty()) {
                RecombineInfo& currentInfo = nodes.front().m_recombineInfo;
                Object& builderNode        = nodes.front().m_builderNode;

                if (currentInfo.m_children.empty()) {
                    nodes.pop_front();
                    continue;
                }

                List& children = *new List(w, std.op.BuilderNode);
                builderNode.set(id.children, children);
                for (RecombineInfo* recombineInfoChild : currentInfo.m_children) {
                    Object& newBuilderNode = *new Object(w, std.op.BuilderNode);
                    newBuilderNode.set(id.originalKey, *recombineInfoChild->m_referenceOutKeyPtr);
                    newBuilderNode.set(id.transformedKey, *recombineInfoChild->m_outKeyPtr);
                    newBuilderNode.set(id.builder, (*recombineInfoChild->m_builderNodePtr)[id.value]);
                    children.add(newBuilderNode);
                    nodes.push_back({ *recombineInfoChild, newBuilderNode });
                }
                nodes.pop_front();
            }
            if (isConstantFoldingPossible(tool)) {
                ret.push_back({ true, &firstBuilderNode, tool });
            } else {
                ret.push_back({ false, &firstBuilderNode, tool });
            }
        }

        // stepping
        bool wrapAround = true;
        auto lastIt     = recombineInfos.begin() + recombineInfos.size() - 1;
        while (true) {
            auto& recombineInfo = *lastIt;
            if (wrapAround) {
                wrapAround = recombineInfo.step();
            }
            if (recombineInfo.isLast() || !wrapAround) {
                break;
            }
            --lastIt;
        }
        hasMorePermutation = !wrapAround;
    }

    return ret;
}

class SubEffect
{
public:
    CellI* cell;
    CellI* key;
    CellI* effect;
};

ToolFinder::BuildToolInfo::BuildToolInfo(CellI& outCell, CellI& outKey, CellI& matchedEffect, CellI& builder) :
    m_outCell(outCell), m_outKey(outKey), m_matchedEffect(matchedEffect), m_builder(builder)
{
}

// ============================================================================
void ToolFinder::buildTool(const BuildToolInfo& buildToolInfo)
{
    auto& ListOfCellStruct = w.getStruct(w.templateId("std::List", id.valueType, std.Cell));

    std::list<BuildToolInfo> toCreate;
    toCreate.push_back(buildToolInfo);

    auto getValuePtrFromValueCell = [this, &ListOfCellStruct](CellI& matchedEffect, CellI& valueCell) -> CellI* {
        if (&valueCell.__type__() == &std.op.ConstVar) {
            return &valueCell;
        } else if (&valueCell.__type__() == &ListOfCellStruct) {
            CellI* valuePtr = &matchedEffect;
            for (CellI& pathItem : valueCell) {
                CellI& currentValue = *valuePtr;
                if (&pathItem.__type__() == &std.ast.Member) {
                    valuePtr = &currentValue[pathItem[id.key]];
                } else if (&pathItem.__type__() == &std.ast.Parameter) {
                    CellI& key = pathItem[id.key];
                    valuePtr   = &currentValue[key];
                } else {
                    panic("Unknown builder path item type!");
                }
            }

            return valuePtr;
        } else {
            panic("Unknown builder path item type!");
        }
    };

    while (!toCreate.empty()) {
        BuildToolInfo& buildToolInfo = toCreate.front();
        CellI& matchedEffect         = buildToolInfo.m_matchedEffect;
        CellI& builder               = buildToolInfo.m_builder;
        CellI* retPtr                = &buildToolInfo.m_outCell;
        CellI* retKeyPtr             = &buildToolInfo.m_outKey;
        CellI* slotItemPtr           = &builder[id.first];
        bool first                   = true;
        CellI* primitiveToolPtr      = nullptr;
        std::list<SubEffect> subEffects;
        std::deque<StackNode> stack;
        while (slotItemPtr) {
            CellI& ret = *retPtr;
            CellI& key = (*slotItemPtr)[id.value];

            if (first) {
                if (&key.__type__() != &std.ast.Member || (&key[id.key] != &id.__type__)) {
                    panic("The first item in a builder must be a member with a value of __type__!");
                }
                first               = false;
                CellI& nextSlotItem = (*slotItemPtr)[id.next];
                CellI& valueCell    = nextSlotItem[id.value];
                Object* newObj      = nullptr;
                CellI* typePtr      = nullptr;
                if (&valueCell.__type__() == &std.ast.ConstVar) {
                    typePtr     = &valueCell[id.value];
                    newObj      = new Object(w, *typePtr, fmt::format("built from {}", builder.label()));
                    slotItemPtr = &nextSlotItem;
                } else if (&valueCell.__type__() == &std.ast.PrimitiveToolName) {
                    CellI& ast = valueCell[id.name];
                    typePtr    = &ast[w.id.primitiveTool];
                    newObj     = new Object(w, *typePtr, fmt::format("built from {}", builder.label()));
                    (*newObj).set(w.id.ast, ast);
                    primitiveToolPtr = &ast;
                } else {
                    panic("Builder type is not a constant value!");
                }
                newObj->set(id.state, std.op.State.ready); // TODO
                CellI& type = *typePtr;
                CellI& retKey = *retKeyPtr;
                if (&retKey.__type__() == &std.ast.Member) {
                    retPtr->set(retKey[id.key], *newObj);
                } else if (&retKey.__type__() == &std.ast.Parameter) {
                    CellI& paramKey = retKey[id.key];
                    CellI& slot     = *new Object(w, std.op.Parameter);
                    slot.set(w.id.key, paramKey);
                    slot.set(w.id.value, (*newObj));
                    (*newObj).set(w.id.stack, (*retPtr)[id.method][id.value]);
                    (*retPtr)[id.parameters].set(paramKey, slot);
                } else {
                    panic("Unknown builder item type");
                }
                retPtr = newObj;
                TRACE(toolFinderLookup, "BUILD: __type__:{}", type.label());

                slotItemPtr = &nextSlotItem;
            } else if (primitiveToolPtr) {
                if (&key == &id.pop) {
                    TRACE(toolFinderLookup, "BUILD: pop");
                    retPtr           = stack.back().effectPtr;
                    primitiveToolPtr = stack.back().slotItemPtr;
                    retKeyPtr        = stack.back().paramItemPtr;
                    stack.pop_back();
                    slotItemPtr      = slotItemPtr->getNextOrNullptr();
                    first            = false;
                    std::cout << "";
                    continue;
                }
                CellI& unwrappedKey  = key[id.key];
                CellI& nextSlotItem  = (*slotItemPtr)[id.next];
                slotItemPtr          = &nextSlotItem;
                CellI& valueCell     = nextSlotItem[id.value];
                if (&valueCell == &id.push) {
                    TRACE(toolFinderLookup, "BUILD: push to '{}'", unwrappedKey.label());
                    stack.push_back({ retPtr, primitiveToolPtr, retKeyPtr });
                    retKeyPtr        = &key;
                    primitiveToolPtr = nullptr;
                    slotItemPtr      = slotItemPtr->getNextOrNullptr();
                    first            = true;
                    std::cout << "";
                    continue;
                }
                CellI* valuePtr      = getValuePtrFromValueCell(matchedEffect, valueCell);
                if (!(&(*valuePtr).__type__() == &std.op.UnknownVar || &(*valuePtr).__type__() == &std.op.ConstVar)) {
                    subEffects.push_back({ retPtr, &w.ast.member(unwrappedKey), valuePtr });
                    TRACE(toolFinderLookup, "BUILD: '{}' is a sub effect", unwrappedKey.label());
                } else {
                    ret.set(unwrappedKey, *valuePtr);
                    TRACE(toolFinderLookup, "BUILD: '{}':{}", unwrappedKey.label(), (*valuePtr).label());
                }
            } else if (&key.__type__() == &std.ast.Member) {
                CellI& unwrappedKey = key[id.key];
                CellI& nextSlotItem = (*slotItemPtr)[id.next];
                CellI& valueCell    = nextSlotItem[id.value];
                CellI* valuePtr     = getValuePtrFromValueCell(matchedEffect, valueCell);

                if (!(&(*valuePtr).__type__() == &std.op.UnknownVar || &(*valuePtr).__type__() == &std.op.ConstVar)) {
                    subEffects.push_back({ retPtr, &key, valuePtr });
                    TRACE(toolFinderLookup, "BUILD: '{}' is a sub effect", unwrappedKey.label());
                } else {
                    ret.set(unwrappedKey, *valuePtr);
                    TRACE(toolFinderLookup, "BUILD: '{}':{}", unwrappedKey.label(), (*valuePtr).label());
                }
                slotItemPtr = &nextSlotItem;
            } else if (&key.__type__() == &std.ast.Parameter) {
                CellI& unwrappedKey = key[id.key];
                CellI& nextSlotItem = (*slotItemPtr)[id.next];
                CellI& valueCell    = nextSlotItem[id.value];
                CellI* valuePtr     = getValuePtrFromValueCell(matchedEffect, valueCell);

                if (ret.missing(id.parameters)) {
                    CellI& ParameterType = ret[id.method][id.value][id.parametersType];
                    ret.set(id.parameters, *new Object(w, ParameterType));
                    TRACE(toolFinderLookup, "BUILD: parameters");
                }
                CellI& parameters = ret[id.parameters];
                if (!(&(*valuePtr).__type__() == &std.op.UnknownVar || &(*valuePtr).__type__() == &std.op.ConstVar)) {
                    subEffects.push_back({ retPtr, &key, valuePtr });
                    TRACE(toolFinderLookup, "BUILD: param: '{}' is a sub effect", unwrappedKey.label());
                } else {
                    parameters.set(unwrappedKey, *valuePtr);
                    TRACE(toolFinderLookup, "BUILD: param: '{}':{}", unwrappedKey.label(), (*valuePtr).label());
                }
                slotItemPtr = &nextSlotItem;
            } else {
                panic("Unknown builder item!");
            }

            slotItemPtr = slotItemPtr->getNextOrNullptr();
        }
        for (auto& subEffect : subEffects) {
            CellI& cell             = *subEffect.cell;
            CellI& key              = *subEffect.key;
            CellI& effect           = *subEffect.effect;
            auto subToolBuildersPtr = findBuildersForDescription(effect, DescriptionKind::selfBuilder);

            if (!subToolBuildersPtr) {
                panic("Sub effect not found!");
            }
            CellI* subToolBuilder  = &(*subToolBuildersPtr)[id.first][id.value]; // the first self builder is the copy constructor
            toCreate.push_back({ cell, key, effect, *subToolBuilder });
        }
        toCreate.erase(toCreate.begin());
    }
}

// ============================================================================
ToolFinder::Node* ToolFinder::getRootNodeForDescriptionKind(DescriptionKind descriptionKind)
{
    switch (descriptionKind) {
    case DescriptionKind::consequence:
        return m_consequenceRootNode.get();
    case DescriptionKind::selfBuilder:
        return m_selfBuilderRootNode.get();
    case DescriptionKind::solver:
        return m_solverRootNode.get();
    }

    panic("Unknown description kind!");
}

// ============================================================================
bool ToolFinder::checkUnknownsInTool(CellI& effect)
{
    if (&effect.__type__() == &std.op.ConstVar) {
        return false;
    } else if (&effect.__type__() == &std.op.UnknownVar) {
        return true;
    } else if (&effect.__type__() == &std.op.Call) {
        if (effect.has("parameters")) {
            for (CellI& parameterKV : effect[id.parameters]) {
                CellI& parameter = parameterKV[id.value];
                if (checkUnknownsInTool(parameter[id.value])) {
                    return true;
                }
            }
        }
        return false;
    } else if (effect.__type__().has(id.primitiveTool)) {
        CellI& ast         = effect.__type__()[id.ast];
        bool isConstructor = ast.has("isConstructor");
        for (CellI& mappingKV : ast[id.memberMapping]) {
            if (isConstructor) {
                isConstructor = false;
                continue;
            }
            if (checkUnknownsInTool(effect[mappingKV[id.value]])) {
                return true;
            }
        }
        return false;
    }

    panic("Unknown AST to instantiate!");
}

// ============================================================================
static CellI* findMissingParameterKey(ToolFinder::ConversionToolBlueprint& blueprint)
{
    CellI& tool       = *blueprint.m_tool;
    World& w          = tool.w;
    CellI& filledKey  = *blueprint.m_slotId;
    for (CellI& parameter : tool[w.id.parameters].__type__()[w.id.members]) {
        CellI& key = parameter[w.id.key];
        if (&key != &filledKey) {
            return &key;
        }
    }

    return nullptr;
}

class ConversionLib : public Library
{
public:
    ConversionLib(World& w, Ast::Scope& parentScope, const std::string& conversionToolName, CellI& conversionToolAst, CellI& inputType, CellI& returnType);
    Ast::Scope& m_conversionScope;
};
class ConversionLibAst : public AstHelper
{
public:
    ConversionLibAst(World& w, Ast::Scope& scope, const std::string& conversionToolName, CellI& conversionToolAst, CellI& inputType, CellI& returnType);
};

ConversionLibAst::ConversionLibAst(World& w, Ast::Scope& parentScope, const std::string& conversionToolName, CellI& conversionToolAst, CellI& inputType, CellI& returnType) :
    AstHelper(w)
{
    parentScope.add<Function>(conversionToolName)
        .parameters(
            parameter("from", _(inputType)))
        .returnType(_(returnType))
        .instructions(
            return_(conversionToolAst));
}

ConversionLib::ConversionLib(World& w, Ast::Scope& parentScope, const std::string& conversionToolName, CellI& conversionToolAst, CellI& inputType, CellI& returnType) :
    Library(w, parentScope, "conversion"),
    m_conversionScope(parentScope.add<Ast::Scope>("conversion"))
{
    ConversionLibAst solverLibAst(w, m_conversionScope, conversionToolName, conversionToolAst, inputType, returnType);
}

// ============================================================================
void ToolFinder::createConversionToolFromBlueprint(CellI& from, CellI& to, ToolFinder::ConversionToolBlueprint& blueprint, List& results)
{
    CellI& blueprintTool = *blueprint.m_tool;
    CellI& blueprintKey  = *blueprint.m_slotId;

    DEBUG(toolFinderConversion, "check conversion with tool `{}` from: {}:{}, to: {}", blueprint.m_tool->label(), blueprintKey.label(), from.label(), to.label());

    if (static_cast<Map&>(blueprintTool[id.parameters].__type__()[id.members]).size() == 1) {
        // nothing to solve here, just test
        CellI* toolPtr = nullptr;
        if (blueprintTool.has(w.id.primitiveTool)) {
            CellI& tool = *new Object(w, blueprintTool);
            toolPtr     = &tool;
            tool.set(blueprintKey, w.op.const_(from));
            tool.set(id.state, std.op.State.ready);
        } else {
            CellI& tool = *new Object(w, std.op.Call);
            toolPtr     = &tool;
            tool.set(id.method, *blueprint.m_tool);
            Map& parameters = w.op.parameters(blueprintKey, w.op.const_(from));
            tool.set(id.parameters, parameters);
        }
        CellI& tool = *toolPtr;
        tool();
        if (&tool[id.value] != &to) {
            return;
        }
        std::string conversionToolName = fmt::format("conversionToolFor_{}", blueprintTool[id.name].label());
        Ast::Base* conversionToolAstPtr = nullptr;
        if (blueprintTool.has(w.id.primitiveTool)) {
            conversionToolAstPtr = &w.ast.call(w.ast.parameter(w.name("from")), w.ast.primitiveToolName(blueprintTool));
        } else {
            panic("TODO");
        }
        Ast::Base& conversionToolAst = *conversionToolAstPtr;

        Ast::Scope rootScope(w, "toolFinder");
        Compiler compiler(w);
        ConversionLib conversionLib(w, rootScope, conversionToolName, conversionToolAst, from.__type__(), to.__type__());
        conversionLib.include(w.arcLib());
        compiler.compile(conversionLib);
        auto& conversionTool        = conversionLib.getFunction(fmt::format("conversion::{}", conversionToolName));
        auto& astFunction           = conversionLib.m_conversionScope.getItem<Ast::Function>(conversionToolName);
        auto& compiledDescription   = compiler.compileAsDescription(conversionToolAst, astFunction);
        auto& serializedDescription = serializeEffect(compiledDescription);

        DEBUG(toolFinderConversion, " generating fn {}(from:{}) -> {} ...\n{}", blueprint.m_tool->label(), from.label(), to.label(), conversionTool.printAsValue());
        results.add(w.ast.kvPair(serializedDescription, conversionTool));
        return;
    }
    if (static_cast<Map&>(blueprintTool[id.parameters].__type__()[id.members]).size() != 2) {
        return;
    }

    CellI* missingSlotIdPtr = findMissingParameterKey(blueprint);
    if (!missingSlotIdPtr) {
        return; // TODO
    }

    CellI& firstParam = blueprintTool[id.parameters].__type__()[id.members][id.list][id.first][id.value][id.value][id.name];
    bool swappedParams = &firstParam != &blueprintKey;
    TRACE(toolFinderConversion, " swappedParams: {}", swappedParams);

    // this is the from in "tool(from, x) == to"
    Object unknownX(w, std.op.ConstVar, "unknownX");
    CellI& missingSlotId = *missingSlotIdPtr;
    auto& getValueFromX  = w.op.get(w.op.unknown_(unknownX), w.op.const_(id.value));
    getValueFromX.set(id.state, std.op.State.missingInput);
    CellI* toolPtr = nullptr;
    if (blueprintTool.has(w.id.primitiveTool)) {
        CellI& tool = *new Object(w, blueprintTool);
        toolPtr     = &tool;
        tool.set(blueprintKey, w.op.const_(from));
        tool.set(missingSlotId, getValueFromX);
    } else {
        CellI& tool = *new Object(w, std.op.Call);
        toolPtr     = &tool;
        tool.set(id.method, *blueprint.m_tool);
        Map& parameters = w.op.parameters(blueprintKey, w.op.const_(from), missingSlotId, getValueFromX);
        tool.set(id.parameters, parameters);
    }
    CellI& tool = *toolPtr;
    CellI& missingSlotEquation = w.op.equal(tool, w.op.const_(to));
    missingSlotEquation.label("tool(from, x) == to");
    DEBUG(toolFinderConversion, " missingSlotEquation: {}", missingSlotEquation.printAsValue());
    CellI* solvedMissingSlotEquationPtr = solve(missingSlotEquation);
    if (!solvedMissingSlotEquationPtr) {
        return;
    }

    CellI& solvedMissingSlotEquation = (*solvedMissingSlotEquationPtr)[id.first][id.value];
    List* missingSlotSolversPtr      = &findToolsByDescription(solvedMissingSlotEquation, DescriptionKind::consequence);
    if (!missingSlotSolversPtr) {
        return;
    }

    List& missingSlotSolvers = *missingSlotSolversPtr;

    for (CellI& missingSlotSolver : missingSlotSolvers) {
        //        std::cout << blueprint << '\n';
        // printAsValue(missingSlotSolver, "");
        missingSlotSolver();

        CellI& solvedX = unknownX[id.value];

        // TODO we need to validate this value somewhere. In case of Division it can not be in the rhs
        // HACK
        if ((&solvedX == &w._0_) && (&blueprintTool == &w.std.op.Divide)) {
            DEBUG(toolFinderConversion, " Division by zero, skipped");
            continue;
        }

        DEBUG(toolFinderConversion, " solved equation: {}  =>  {}  =>  {} = {}", solvedMissingSlotEquation.printAsValue(), missingSlotSolver.printAsValue(), unknownX.label(), solvedX.label());
        TRACE(toolFinderConversion, "unknownX.value = {}", solvedX.label());

        Ast::Scope rootScope(w, "toolFinder");
        Compiler compiler(w);

        std::string conversionToolName = fmt::format("conversionToolFor_{}", blueprintTool[id.name].label());
        Ast::Base* conversionToolAstPtr = nullptr;
        if (blueprintTool.has(w.id.primitiveTool)) {
            // TODO maybe we need a dedicated AST cell that accept parameter names also in compiled form
            // so we know, that the op.LHS or op.RHS is the solvedX, but we currently can not express this with AST nodes as call expect an obj
            if (swappedParams) {
                conversionToolAstPtr = &w.ast.call(w.ast._(solvedX), w.ast.primitiveToolName(blueprintTool))(id.other, w.ast.parameter(w.name("from")));
            } else {
                conversionToolAstPtr = &w.ast.call(w.ast.parameter(w.name("from")), w.ast.primitiveToolName(blueprintTool))(id.other, w.ast._(solvedX));
            }
        } else {
            panic("TODO");
        }
        Ast::Base& conversionToolAst = *conversionToolAstPtr;
        ConversionLib conversionLib(w, rootScope, conversionToolName, conversionToolAst, from.__type__(), to.__type__());
        conversionLib.include(w.arcLib());
        compiler.compile(conversionLib);
        auto& conversionTool        = conversionLib.getFunction(fmt::format("conversion::{}", conversionToolName));
        auto& astFunction           = conversionLib.m_conversionScope.getItem<Ast::Function>(conversionToolName);
        auto& compiledDescription   = compiler.compileAsDescription(conversionToolAst, astFunction);
        auto& serializedDescription = serializeEffect(compiledDescription);

        DEBUG(toolFinderConversion, " generating fn {}(X:{}, from:{}) -> {} ...\n{}", blueprint.m_tool->label(), solvedX.label(), from.label(), to.label(), conversionTool.printAsValue());
        results.add(w.ast.kvPair(serializedDescription, conversionTool));
    }
}

// ============================================================================
CellI& ToolFinder::findConversionTools(CellI& from, CellI& to)
{
    DEBUG(toolFinderConversion, "");
    DEBUG(toolFinderConversion, "findConversionTools from: {}, to: {}", from.printAsValue(), to.printAsValue());
    DEBUG(toolFinderConversion, "");

    List& results = *new List(w, std.List);
    findConversionToolsByType(from, to, results);
    findConversionToolsByContainer(from, to, results);

    std::cout << "";

    return results;
}

// ============================================================================
void ToolFinder::findConversionToolsByType(CellI& from, CellI& to, List& results)
{
    CellI& inputType  = from.__type__();
    CellI& outputType = to.__type__();

    ConversionToolKey conversionToolKey(inputType, outputType);

    auto tools = m_conversionTools.equal_range(conversionToolKey);
    if (tools.first != m_conversionTools.end()) {
        DEBUG(toolFinderConversion, "conversionToolKey:{}", conversionToolKey);
    }
    for (auto it = tools.first; it != tools.second; ++it) {
        ConversionToolBlueprint blueprint = it->second;
        //        std::cout << "  " << blueprint << '\n';
        // ConversionToolKey [from: Number, to: Number]: ConversionToolBlueprint [tool: Add, input: lhs]
        createConversionToolFromBlueprint(from, to, blueprint, results);
    }
}

// ============================================================================
void ToolFinder::findConversionToolsByContainer(CellI& from, CellI& to, List& results)
{
}

// ============================================================================
std::string ToolFinder::printConsequenceNodesAsGrapviz()
{
    return m_consequenceRootNode->printAsGrapviz(w);
}

// ============================================================================
void ToolFinder::exploreSlotManipulations()
{
    Object& x = *new Object(w, std.Number, "X");

    for (CellI& tool : m_tools) {
        TRACE(toolFinderExplore, "explore: {}", tool.label());

        if (tool.missing(id.returnType)) {
            continue;
        }
        CellI& returnType = tool[id.returnType];
        if (&returnType == &std.Boolean) {
            auto& parameters            = static_cast<Map&>(tool[id.parameters].__type__()[id.members]);
            bool allInputParamIsBoolean = true;
            for (auto& parameter : parameters) {
                CellI& parameterType = parameter[id.value][id.type];
                if (&parameterType != &std.Boolean) {
                    allInputParamIsBoolean = false;
                    break;
                }
            }
            if (!allInputParamIsBoolean) {
                continue;
            }
            if (parameters.size() == 1) {
                auto& booleanTool = *new Object(w, tool);
                booleanTool.set(id.input, w.op.unknown_(x));
                auto& opEqual = w.op.equal(booleanTool, w.op.const_(w.false_));
                opEqual.label(fmt::format("{}(X) == {}", tool.label(), booleanTool[id.input].label(), opEqual[id.rhs].label()));
                exploreSlotManipulationFor(opEqual);

                opEqual.set(id.rhs, w.op.const_(w.true_));
                opEqual.label(fmt::format("{}(X) == {}", tool.label(), booleanTool[id.input].label(), opEqual[id.rhs].label()));

                exploreSlotManipulationFor(opEqual);

                continue;
            }
            auto& booleanTool = *new Object(w, tool);
            booleanTool.set(id.lhs, w.op.const_(w.true_));
            booleanTool.set(id.rhs, w.op.unknown_(x));
            auto& opEqual = w.op.equal(booleanTool, w.op.const_(w.false_));
            opEqual.label(fmt::format("{}({}, X) == {}", tool.label(), booleanTool[id.lhs].label(), opEqual[id.rhs].label()));
            exploreSlotManipulationFor(opEqual);

            booleanTool.set(id.lhs, w.op.unknown_(x));
            booleanTool.set(id.rhs, w.op.const_(w.true_));
            opEqual.label(fmt::format("{}(X, {}) == {}", tool.label(), booleanTool[id.rhs].label(), opEqual[id.rhs].label()));
            exploreSlotManipulationFor(opEqual);

            opEqual.set(id.rhs, w.op.const_(w.true_));

            booleanTool.set(id.lhs, w.op.const_(w.false_));
            booleanTool.set(id.rhs, w.op.unknown_(x));
            exploreSlotManipulationFor(opEqual);

            booleanTool.set(id.lhs, w.op.unknown_(x));
            booleanTool.set(id.rhs, w.op.const_(w.false_));
            exploreSlotManipulationFor(opEqual);

            m_consequenceRootNode->printAsGrapviz(w);

            continue;
        }
        if (&returnType != &std.Number) {
            continue;
        }

        auto& numberTool = *new Object(w, tool);
        CellI& opConst2  = w.op.const_(2);
        opConst2.erase(id.value);
        opConst2.label("2");

        CellI& opConst4 = w.op.const_(4);
        opConst4.erase(id.value);
        opConst4.label("4");

        numberTool.set(id.lhs, opConst2);
        numberTool.set(id.rhs, w.op.unknown_(x));

        auto& opEqual = w.op.equal(numberTool, opConst4);
        opEqual.label(fmt::format("{}(x, y) == z", tool.label()));
        exploreSlotManipulationFor(opEqual);

        numberTool.set(id.lhs, w.op.unknown_(x));
        numberTool.set(id.rhs, opConst2);

        opEqual.label(fmt::format("{}(y, x) == z", tool.label()));
        exploreSlotManipulationFor(opEqual);
    }
}

struct BuilderWithTool
{
    CellI& m_builder;
    CellI& m_tool;
};

// ============================================================================
void ToolFinder::exploreSlotManipulationFor(CellI& description)
{
    DEBUG(toolFinderExplore, "");
    DEBUG(toolFinderExplore, "input: {}", description.printAsValue());

    Node* rootNode    = new Node();
    auto permutations = recombine(rootNode, description);
    for (auto& permutationResult : permutations) {
        CellI& permutation = permutationResult.m_recombinedTool;
        auto buildersPtr = findBuildersForDescription(permutation, DescriptionKind::consequence);
        if (!buildersPtr) {
            continue;
        }
        std::list<BuilderWithTool> tools1;
        for (auto& builder : *buildersPtr) {
            TRACE(toolFinderExplore, "build with {}", builder.label());
            Object retVal(w, std.ast.ConstVar);
            buildTool({ retVal, w.ast.member(id.value), permutation, builder });
            auto& tool = retVal[id.value];
            tools1.push_back({ builder, tool });
        }

        for (auto& builderWithTool1 : tools1) {
            auto& tool1 = builderWithTool1.m_tool;
            TRACE(toolFinderExplore, "  1. build: {}", tool1.printAsValue());
            if (&tool1.__type__() == &std.op.Equal && (&tool1[id.lhs].__type__() == &std.op.UnknownVar) && (&tool1[id.rhs].__type__() == &std.op.ConstVar)) {
                DEBUG(toolFinderExplore, "  1. result: {}", tool1.printAsValue());
                std::list<BuilderChainNode> builderChain;
                builderChain.push_back({ nullptr, permutationResult.m_builderForTool });
                builderChain.push_back({ &builderWithTool1.m_builder, nullptr });
                addSolver(description, builderChain);
                return;
            }
            auto tool1Permutations = recombine(rootNode, tool1);
            for (auto& tool1PermutationResult : tool1Permutations) {
                CellI& tool1Permutation = tool1PermutationResult.m_recombinedTool;
                TRACE(toolFinderExplore, "  1. permutation: {}", tool1Permutation.printAsValue());
                if (tool1PermutationResult.m_isConstantFoldingPossible && (&tool1Permutation[id.lhs].__type__() == &std.op.UnknownVar)) {
                    DEBUG(toolFinderExplore, "  1. result: {}", tool1Permutation.printAsValue());
                    std::list<BuilderChainNode> builderChain;
                    builderChain.push_back({ nullptr, permutationResult.m_builderForTool });
                    builderChain.push_back({ &builderWithTool1.m_builder, nullptr });
                    builderChain.push_back({ nullptr, tool1PermutationResult.m_builderForTool });
                    addSolver(description, builderChain);
                    return;
                }
                auto buildersPtr = findBuildersForDescription(tool1Permutation, DescriptionKind::consequence);
                if (!buildersPtr) {
                    continue;
                }
                std::list<BuilderWithTool> tools2;
                for (auto& builder : *buildersPtr) {
                    TRACE(toolFinder, "build with {}", builder.label());
                    Object retVal(w, std.ast.ConstVar);
                    buildTool({ retVal, w.ast.member(id.value), tool1Permutation, builder });
                    auto& tool = retVal[id.value];
                    tools2.push_back({ builder, tool });
                }
                for (auto& builderWithTool2 : tools2) {
                    auto& tool2 = builderWithTool2.m_tool;
                    TRACE(toolFinderExplore, "    2. result: {}", tool2.printAsValue());
                    auto tool2Permutations = recombine(rootNode, tool2);
                    for (auto& tool2PermutationResult : tool2Permutations) {
                        CellI& tool2Permutation = tool2PermutationResult.m_recombinedTool;
                        TRACE(toolFinderExplore, "    2. permutation: {}", tool2Permutation.printAsValue());
                        if (tool2PermutationResult.m_isConstantFoldingPossible && (&tool2Permutation[id.lhs].__type__() == &std.op.UnknownVar)) {
                            TRACE(toolFinderExplore, "  1. result: {}", tool1Permutation.printAsValue());
                            DEBUG(toolFinderExplore, "  2. result: {}", tool2Permutation.printAsValue());
                            std::list<BuilderChainNode> builderChain;
                            builderChain.push_back({ nullptr, permutationResult.m_builderForTool });
                            builderChain.push_back({ &builderWithTool1.m_builder, nullptr });
                            builderChain.push_back({ nullptr, tool1PermutationResult.m_builderForTool });
                            builderChain.push_back({ &builderWithTool2.m_builder, nullptr });
                            builderChain.push_back({ nullptr, tool2PermutationResult.m_builderForTool });
                            addSolver(description, builderChain);
                            return;
                        }
                    }
                }
            }
        }
    }
}

} // namespace cells
} // namespace infocell

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

namespace infocell {
namespace cells {

// ============================================================================
static void printAsValue(CellI& cell, const std::string& label = "")
{
    if (!label.empty()) {
        std::cout << label << ": ";
    }

    std::cout << cell.printAsValue() << std::endl;
}

std::string ToolFinder::Node::print()
{
    std::deque<std::string> path;
    Node* child = this;
    Node* parent = m_parent;
    while (parent) {
        CellI* value = nullptr;
        for (auto& pair : parent->m_children) {
            if (pair.second == child) {
                value = pair.first;
                break;
            }
        }
        path.push_front(value->label());
        child  = parent;
        parent = parent->m_parent;
    }
    std::stringstream ss;
    for (auto& item : path) {
        ss << item << " ";
    }

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
    m_root = std::make_unique<Node>();
}

// ============================================================================
bool ToolFinder::empty()
{
    return m_root->m_children.empty();
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
    std::deque<StackNode> stack;
    List& ret              = *new List(w, std.Cell);
    Node* currentNode      = m_root.get();
    CellI* slotItemPtr     = &effect.__type__()["slotKeyList"][id.first];
    CellI* paramItemPtr    = nullptr;
    CellI* currentPtr      = &effect;

    while (slotItemPtr) {
        CellI& slotItem               = *slotItemPtr;
        CellI& current                = *currentPtr;
        CellI& key                    = slotItem[id.value];
        ParamValueKind paramValueKind = ParamValueKind::NonParamValue;
        CellI* keyPtr                 = nullptr;
        CellI* valuePtr               = nullptr;

        if (&key == &id.__type__) {
            serializeKeyWithConstValue(ret, key, current.__type__());
        } else {
            if (&key == &id.method) {
                serializeKeyWithConstValue(ret, key, current[key]);
            } else if (&key == &id.parameters && current.has(key)) {
                if (!paramItemPtr) {
                    paramItemPtr = &current[id.parameters][id.first];
                }
                CellI& paramSlot = (*paramItemPtr)[id.value];
                keyPtr           = &paramSlot[id.key];
                valuePtr         = &paramSlot[id.value];
                serializeKeyWithParamValue(ret, *keyPtr, *valuePtr, paramValueKind);
            } else {
                keyPtr   = &key;
                valuePtr = &current[key];
                serializeKeyWithParamValue(ret, *keyPtr, *valuePtr, paramValueKind);
            }
            if (paramValueKind == ParamValueKind::Call) {
                stack.push_back({ currentPtr, slotItemPtr, paramItemPtr });
                currentPtr   = valuePtr;
                slotItemPtr  = &(*valuePtr).__type__()["slotKeyList"][id.first];
                paramItemPtr = nullptr;
                continue;
            }

            if (&key == &id.parameters && paramItemPtr) {
                CellI& paramItem = *paramItemPtr;
                paramItemPtr     = paramItem.getNextOrNullptr();
                if (paramItemPtr) {
                    continue;
                } else {
                    paramItemPtr = nullptr;
                }
            }
        }

        slotItemPtr = slotItem.getNextOrNullptr();
        while (!slotItemPtr && !stack.empty()) {
            currentPtr   = stack.back().effectPtr;
            slotItemPtr  = stack.back().slotItemPtr;
            paramItemPtr = stack.back().paramItemPtr;
            stack.pop_back();
            ret.add(id.op);
            ret.add(id.pop);
            slotItemPtr = slotItemPtr->getNextOrNullptr();
        }
    }

    return ret;
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
void ToolFinder::add(Object& tool)
{
    auto& description = tool[id.description];
    if (tool.has(id.returnType)) {
        // so this can be a conversion tool
        CellI& returnType = tool[id.returnType];
        if (tool.has(id.parameters)) {
            for (CellI& parameter : tool[id.parameters]) {
                CellI& inputType = parameter[id.type];
                ConversionToolKey key(inputType, returnType);
                ConversionToolBlueprint blueprint(tool, parameter[id.key]);
                m_conversionTools.insert({ key, blueprint });
            }
        }
    }
    m_tools.add(tool);

    TRACE(toolFinder, "{} =>", tool.label());
    // TODO: getting the hybrid List type during compilation is not possible, so using heuristic here
    // TODO: use enum here instead!
    if (description.has(w.id.first) && description.has(w.id.last) && description.has(w.id.size)) {
        for (CellI& effect : description) {
            add(effect, tool);
        }
    } else {
        add(description, tool);
    }
}

// ============================================================================
void ToolFinder::saveCurrentPath(CellI& key, CellI& memberKey, Map& memberIds, std::deque<StackNode>& stack)
{
    auto getSlotForKey = [this](CellI& key) -> CellI& {
        if (&key == &id.self) {
            return w.ast.member(key);
        } else {
            return w.ast.parameter(key);
        }
    };
    if (!memberIds.hasKey(memberKey)) {
        List& path = *new List(w, std.Cell, fmt::format("path for {}", memberKey.label()));
        for (auto& stackItem : stack) {
            path.add(getSlotForKey((*stackItem.slotItemPtr)[id.value]));
        }
        path.add(getSlotForKey(key));
        memberIds.add(memberKey, path);
    }
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
        addValue(node, value[id.value]);
        if (&value[id.value] == &id.op) {
            // handling the _(op) case, where op is a constant
            // it will be op op, so the escaping character here is op
            addValue(node, value[id.value]);
        }
        paramValueKind = ParamValueKind::ConstVar;
    } else if (&value.__type__() == &std.ast.Self) {
        addValue(node, id.op);
        addValue(node, id.variable);
        paramValueKind = ParamValueKind::Self;
    } else if (&value.__type__() == &std.ast.Return) {
        addValue(node, id.op);
        addValue(node, id.variable);
        paramValueKind = ParamValueKind::Return;
    } else if (&value.__type__() == &std.ast.Parameter) {
        addValue(node, id.op);
        addValue(node, id.variable);
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
void ToolFinder::add(CellI& effect, CellI& tool)
{
    std::deque<StackNode> stack;
    Node* currentNode      = m_root.get();
    CellI* slotItemPtr     = &effect.__type__()["slotKeyList"][id.first];
    CellI* paramItemPtr    = nullptr;
    CellI* currentPtr      = &effect;
    bool hasReturnInEffect = false;
    Map memberIds(w, std.Cell, std.Cell);

    while (slotItemPtr) {
        CellI& slotItem               = *slotItemPtr;
        CellI& current                = *currentPtr;
        CellI& key                    = slotItem[id.value];
        ParamValueKind paramValueKind = ParamValueKind::NonParamValue;
        CellI* keyPtr                 = nullptr;
        CellI* valuePtr               = nullptr;

        if (&key == &id.__type__) {
            addKeyWithConstValue(currentNode, key, current.__type__());
        } else {
            if (&key == &id.method) {
                addKeyWithConstValue(currentNode, key, current[key]);
            } else if (&key == &id.parameters && current.has(key)) {
                if (!paramItemPtr) {
                    paramItemPtr = &current[id.parameters][id.first];
                }
                CellI& paramSlot = (*paramItemPtr)[id.value];
                keyPtr           = &paramSlot[id.key];
                valuePtr         = &paramSlot[id.value];
                addKeyWithParamValue(currentNode, *keyPtr, *valuePtr, paramValueKind);
            } else {
                keyPtr   = &key;
                valuePtr = &current[key];
                addKeyWithParamValue(currentNode, *keyPtr, *valuePtr, paramValueKind);
            }
            if (paramValueKind == ParamValueKind::Return) {
                saveCurrentPath(*keyPtr, id.return_, memberIds, stack);
                hasReturnInEffect = true;
            } else if (paramValueKind == ParamValueKind::Self) {
                saveCurrentPath(*keyPtr, id.self, memberIds, stack);
            } else if (paramValueKind == ParamValueKind::Parameter) {
                saveCurrentPath(*keyPtr, (*valuePtr)[id.key], memberIds, stack);
            } else if (paramValueKind == ParamValueKind::Call) {
                stack.push_back({ currentPtr, slotItemPtr, paramItemPtr });
                currentPtr   = valuePtr;
                slotItemPtr  = &(*valuePtr).__type__()["slotKeyList"][id.first];
                paramItemPtr = nullptr;
                continue;
            }

            if (&key == &id.parameters && paramItemPtr) {
                CellI& paramItem = *paramItemPtr;
                paramItemPtr     = paramItem.getNextOrNullptr();
                if (paramItemPtr) {
                    continue;
                } else {
                    paramItemPtr = nullptr;
                }
            }
        }

        slotItemPtr = slotItem.getNextOrNullptr();
        while (!slotItemPtr && !stack.empty()) {
            currentPtr   = stack.back().effectPtr;
            slotItemPtr  = stack.back().slotItemPtr;
            paramItemPtr = stack.back().paramItemPtr;
            stack.pop_back();
            addValue(currentNode, id.op);
            addValue(currentNode, id.pop);
            slotItemPtr = slotItemPtr->getNextOrNullptr();
        }
    }

    currentNode->m_isLeaf = 1;
    if (currentNode->m_builders == nullptr) {
        currentNode->m_builders = new List(w, std.List, "builders");
    }
    currentNode->m_builders->add(createBuilder(tool, memberIds, hasReturnInEffect));
    currentNode->m_tool   = &tool;
    currentNode->m_effect = &effect;

    if (IS_LOG_ENABLED) {
        CellI& astAsList = serializeEffect(effect);
        std::stringstream ss;
        for (CellI& value: astAsList) {
            if (value.has(id.primitiveTool)) {
                ss << value[id.name].label() << " ";
            } else {
                ss << value.label() << " ";
            }
        }
        TRACE(toolFinder, "  {}", ss.str());
    }
}

// ============================================================================
List& ToolFinder::createBuilder(CellI& tool, Map& memberIds, bool hasReturnInEffect)
{
    List& builder = *new List(w, std.Cell, fmt::format("builder for {}", tool.label()));

    if (hasReturnInEffect) {
        builder.add(w.ast.member(id.__type__));
        builder.add(w.ast.primitiveToolName(std.op.Equal[id.ast]));
        builder.add(w.ast.member(id.lhs));
        builder.add(w.id.push);
    }
    builder.add(w.ast.member(id.__type__));
    if (tool[id.ast].has(w.id.primitiveTool)) {
        builder.add(w.ast.primitiveToolName(tool[id.ast]));
    } else {
        builder.add(w.ast._(std.op.Call));
        builder.add(w.ast.member(id.method));
        builder.add(w.ast._(tool));
    }

    for (CellI& parameter : tool[id.parameters]) {
        CellI& key = parameter[id.key];
        if (&key == &id.self) {
            builder.add(w.ast.member(key));
        } else {
            builder.add(w.ast.parameter(key));
        }
        builder.add(memberIds.getValue(key));
    }
    if (hasReturnInEffect) {
        builder.add(w.id.pop);
        builder.add(w.ast.member(id.other));
        builder.add(memberIds.getValue(id.return_));
    }

    return builder;
}

// ============================================================================
bool ToolFinder::checkValue(Node*& node, CellI& key, CellI& value, bool& needPush, MultiMatchState& multiMatchState, CellI*& multiMatch)
{
    // __type__ is a special key as it can not be a key in a trie node so escaped with "op type"
    if (&key == &id.__type__) {
        auto opFindIt = node->m_children.find(&id.op);
        if (opFindIt != node->m_children.end()) {
            Node* opNode = opFindIt->second;
            for (auto& [opKey, nextNode] : opNode->m_children) {
                if (opKey == &id.type) {
                    node = nextNode;
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
        auto opFindIt = node->m_children.find(&id.op);
        if (opFindIt == node->m_children.end()) {
            return false;
        } else {
            // ok, so value not found but we have an op here
            Node* opNode = opFindIt->second;
            TRACE(toolFinderLookup, "MATCH: op");

            bool matchFound   = false;
            CellI* firstMatch = nullptr;

            for (auto& [opKey, nextNode] : opNode->m_children) {
                if (multiMatchState == MultiMatchState::Restore) {
                    if (opKey != multiMatch) {
                        // we didn't find the last match yet, keep trying
                        continue;
                    } else {
                        // we found the last match
                        multiMatchState = MultiMatchState::Detect;
                        multiMatch      = &id.emptyObject;
                        continue;
                    }
                }
                if (opKey == &id.variable) {
                    if (matchFound) {
                        TRACE(toolFinderLookup, "MULTIMATCH: variable");
                    } else {
                        TRACE(toolFinderLookup, "MATCH: variable");
                        node = nextNode;
                        matchFound = true;
                        firstMatch = &id.variable;
                    }
                    if (multiMatchState == MultiMatchState::Skip) {
                        return true;
                    }
                }

                if (opKey == &id.push && (&value.__type__() != &std.ast.ConstVar)) {
                    if (!((&value.__type__() == &std.op.Call) || (value.__type__().has(id.primitiveTool)))) {
                        panic("Not supported type!");
                    }
                    if (matchFound) {
                        TRACE(toolFinderLookup, "MULTIMATCH: push");
                    } else {
                        TRACE(toolFinderLookup, "MATCH: push");
                        matchFound = true;
                        node       = nextNode;
                        needPush   = true;
                        firstMatch = &id.push;
                    }
                    if (multiMatchState == MultiMatchState::Skip) {
                        return true;
                    } else {
                        multiMatch = firstMatch;
                    }
                }
                if (opKey == &id.pop) {
                    if (matchFound) {
                        TRACE(toolFinderLookup, "MULTIMATCH: pop");
                    } else {
                        TRACE(toolFinderLookup, "MATCH: pop");
                        matchFound = true;
                        node       = nextNode;
                        firstMatch = &id.pop;
                    }
                    if (multiMatchState == MultiMatchState::Skip) {
                        return true;
                    } else {
                        multiMatch = firstMatch;
                    }
                }
            }
            if (matchFound) {
                return true;
            }
        }
    } else {
        TRACE(toolFinderLookup, "MATCH: {}", value.label());
        node = findIt->second;
        return true;
    }

    return false;
}

// ============================================================================
List& ToolFinder::findToolsByEffect(CellI& effect)
{
    List& ret = *new List(w, std.ast.Base);
    if (checkUnknownsInTool(effect)) {
        TRACE(toolFinderLookup, "Unknown in the effect");
    } else {
        TRACE(toolFinderLookup, "Only constants in the effect");
    }
    List* buildersPtr = findBuildersForEffect(effect);
    if (!buildersPtr) {
        return ret;
    }
    for (auto& builder : *buildersPtr) {
        TRACE(toolFinder, "build with {}", builder.label());
        Object retVal(w, std.ast.ConstVar);
        buildTool({ retVal, w.ast.member(id.value), effect, builder });
        auto& tool = retVal[id.value];
        DEBUG(toolFinderLookup, "result: {}", tool.printAsValue());
        if (checkUnknownsInTool(tool)) {
            tool.set(id.state, std.op.State.missingInput);
            TRACE(toolFinderLookup, "Unknown in the result");
            ret.add(tool);
        } else {
            tool.set(id.state, std.op.State.start);
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
List* ToolFinder::findBuildersForEffect(CellI& inputEffect)
{
    //    DEBUG(toolFinderLookup, "input: {}", inputEffectAst.printAsValue());
    List& ret           = *new List(w, std.Cell);
    Node* node          = m_root.get();
    CellI* effectPtr    = &inputEffect;
    CellI* slotItemPtr  = &inputEffect.__type__()["slotKeyList"][id.first];
    CellI* paramItemPtr = nullptr;

    std::deque<StackNode> stack;
    std::deque<StackNode> stackForMultiMatch;

    CellI* multiMatch               = &w.id.emptyObject;
    Node* multiNode                 = node;
    MultiMatchState multiMatchState = MultiMatchState::Detect;

    while (true) {
        while (slotItemPtr || paramItemPtr) {
            CellI& effect   = *effectPtr;
            CellI* keyPtr   = &(*slotItemPtr)[id.value];
            CellI* valuePtr = nullptr;
            if (keyPtr == &id.parameters && effect.has(id.parameters)) {
                if (!paramItemPtr) {
                    paramItemPtr = &effect[id.parameters][id.first];
                }
                CellI& paramSlot = (*paramItemPtr)[id.value];
                keyPtr           = &paramSlot[id.key];
                valuePtr         = &paramSlot[id.value];
            } else {
                keyPtr = &(*slotItemPtr)[id.value];
                if (effect.missing(*keyPtr)) {
                    return nullptr;
                }
                valuePtr = &effect[*keyPtr];
            }

            CellI& key   = *keyPtr;
            CellI& value = *valuePtr;

            bool needPush = false;
            Node* oldNode = node;
            if (checkValue(node, key, value, needPush, multiMatchState, multiMatch) == false) {
                TRACE(toolFinderLookup, "MATCH failed");
                node = nullptr;
                break;
            }
            if (multiMatchState == MultiMatchState::Detect && (multiMatch != &w.id.emptyObject)) {
                TRACE(toolFinderLookup, "MATCH continuity saved");
                stackForMultiMatch = stack;
                stackForMultiMatch.push_back({ effectPtr, slotItemPtr, paramItemPtr });
                multiMatchState = MultiMatchState::Skip;
                multiNode       = oldNode;
            }
            if (needPush) {
                stack.push_back({ effectPtr, slotItemPtr, paramItemPtr });
                effectPtr    = &effect[key];
                slotItemPtr  = &(*effectPtr).__type__()["slotKeyList"][id.first];
                paramItemPtr = nullptr;
                continue;
            }

            // step parameters first if possible
            if (&(*slotItemPtr)[id.value] == &id.parameters && paramItemPtr) {
                CellI& paramItem = *paramItemPtr;
                paramItemPtr     = paramItem.getNextOrNullptr();
                if (paramItemPtr) {
                    continue;
                }
            }

            // step slots then if possible
            slotItemPtr = slotItemPtr->getNextOrNullptr();
            if (slotItemPtr) {
                continue;
            }

            // pop stack if possible
            while (!slotItemPtr && !stack.empty()) {
                // check that stack pop is in the matcher
                auto opFindIt = node->m_children.find(&id.op);
                if (opFindIt == node->m_children.end()) {
                    return nullptr;
                }
                TRACE(toolFinderLookup, "MATCH: op");
                Node* opNode   = opFindIt->second;
                auto popFindIt = opNode->m_children.find(&id.pop);
                if (popFindIt == opNode->m_children.end()) {
                    return nullptr;
                }
                TRACE(toolFinderLookup, "MATCH: pop");
                node = popFindIt->second;

                effectPtr    = stack.back().effectPtr;
                slotItemPtr  = stack.back().slotItemPtr;
                paramItemPtr = stack.back().paramItemPtr;
                stack.pop_back();

                // step after stack pop
                if (paramItemPtr == nullptr) {
                    slotItemPtr = slotItemPtr->getNextOrNullptr();
                } else {
                    paramItemPtr = paramItemPtr->getNextOrNullptr();
                }
            }
        }
        if (node && node->m_isLeaf) {
            for (CellI& builder : *node->m_builders) {
                ret.add(builder);
            }
        }
        if (stackForMultiMatch.empty()) {
            break;
        } else {
            TRACE(toolFinderLookup, "MATCH continue from multimatch state");
            std::swap(stack, stackForMultiMatch);
            effectPtr    = stack.back().effectPtr;
            slotItemPtr  = stack.back().slotItemPtr;
            paramItemPtr = stack.back().paramItemPtr;
            node         = multiNode;
            stack.pop_back();
            multiMatchState = MultiMatchState::Restore;
        }
    }

    if (ret.empty()) {
        delete &ret;
        return nullptr;
    }


    return &ret;
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
        if (&valueCell.__type__() == &std.ast.ConstVar) {
            return &valueCell;
        } else if (&valueCell.__type__() == &ListOfCellStruct) {
            CellI* valuePtr = &matchedEffect;
            for (CellI& pathItem : valueCell) {
                CellI& currentValue = *valuePtr;
                if (&pathItem.__type__() == &std.ast.Member) {
                    valuePtr = &currentValue[pathItem[id.key]];
                } else if (&pathItem.__type__() == &std.ast.Parameter) {
                    CellI& key = pathItem[id.key];
                    if (currentValue.__type__().has(id.primitiveTool)) {
                        valuePtr = &currentValue[pathItem[id.key]];
                    } else {
                        Map& parameters = static_cast<Map&>(currentValue[id.parameters]);
                        valuePtr        = &parameters.getValue(key)[id.value];
                    }
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
                newObj->set(id.state, std.op.State.start); // TODO
                CellI& type = *typePtr;
                CellI& retKey = *retKeyPtr;
                if (&retKey.__type__() == &std.ast.Member) {
                    retPtr->set(retKey[id.key], *newObj);
                } else if (&retKey.__type__() == &std.ast.Parameter) {
                    CellI& paramKey = retKey[id.key];
                    CellI& slot     = *new Object(w, std.ast.Slot);
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
                Map& membersMapping  = static_cast<Map&>((*primitiveToolPtr)[id.memberMapping]);
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
                CellI& translatedKey = membersMapping.getValue(unwrappedKey);
                CellI* valuePtr      = getValuePtrFromValueCell(matchedEffect, valueCell);
                if (!(&(*valuePtr).__type__() == &std.op.UnknownVar || &(*valuePtr).__type__() == &std.op.ConstVar)) {
                    subEffects.push_back({ retPtr, &w.ast.member(translatedKey), valuePtr });
                    TRACE(toolFinderLookup, "BUILD: '{}' is a sub effect", unwrappedKey.label());
                } else {
                    ret.set(translatedKey, *valuePtr);
                    TRACE(toolFinderLookup, "BUILD: '{}':{}", translatedKey.label(), (*valuePtr).label());
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
                    ret.set(id.parameters, *new Map(w, std.Cell, std.ast.Slot));
                    TRACE(toolFinderLookup, "BUILD: parameters");
                }
                auto& parameters = static_cast<Map&>(ret[id.parameters]);
                if (!(&(*valuePtr).__type__() == &std.op.UnknownVar || &(*valuePtr).__type__() == &std.op.ConstVar)) {
                    subEffects.push_back({ retPtr, &key, valuePtr });
                    TRACE(toolFinderLookup, "BUILD: param: '{}' is a sub effect", unwrappedKey.label());
                } else {
                    CellI& slot = *new Object(w, std.ast.Slot);
                    slot.set(w.id.key, unwrappedKey);
                    slot.set(w.id.value, *valuePtr);
                    parameters.add(unwrappedKey, slot);
                    TRACE(toolFinderLookup, "BUILD: param: '{}':{}", unwrappedKey.label(), (*valuePtr).label());
                }
                slotItemPtr = &nextSlotItem;
            } else {
                panic("Unknown builder item!");
            }

            slotItemPtr = slotItemPtr->getNextOrNullptr();
        }
        for (auto& subEffect : subEffects) {
            CellI& cell              = *subEffect.cell;
            CellI& key               = *subEffect.key;
            CellI& effect            = *subEffect.effect;
            List* subToolBuildersPtr = findBuildersForEffect(effect);

            if (!subToolBuildersPtr) {
                panic("Sub effect not found!");
            }
            CellI* subToolBuilder  = &(*subToolBuildersPtr)[id.first][id.value]; // TODO
            toCreate.push_back({ cell, key, effect, *subToolBuilder });
        }
        toCreate.erase(toCreate.begin());
    }
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
            for (CellI& parameter : effect[id.parameters]) {
                if (checkUnknownsInTool(parameter[w.id.value])) {
                    return true;
                }
            }
        }
        return false;
    } else if (effect.__type__().has(id.primitiveTool)) {
        CellI& ast         = effect.__type__()[id.ast];
        bool isConstructor = ast.has("isConstructor");
        for (CellI& key : ast[id.memberMapping]) {
            if (isConstructor) {
                isConstructor = false;
                continue;
            }
            if (checkUnknownsInTool(effect[key])) {
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
    for (CellI& parameter : tool[w.id.parameters]) {
        CellI& key = parameter[w.id.key];
        if (&key != &filledKey) {
            return &key;
        }
    }

    return nullptr;
}

class SolverLib : public Library
{
public:
    SolverLib(World& w, Ast::Scope& parentScope, CellI& solverAst);
};
class SolverLibAst : public AstHelper
{
public:
    SolverLibAst(World& w, Ast::Scope& scope, CellI& solverAst);
};

SolverLibAst::SolverLibAst(World& w, Ast::Scope& parentScope, CellI& solverAst) :
    AstHelper(w)
{
    parentScope.add<Function>("solverFunction")
        .instructions(solverAst);
}

SolverLib::SolverLib(World& w, Ast::Scope& parentScope, CellI& solverAst) :
    Library(w, parentScope)
{
    SolverLibAst solverLibAst(w, parentScope.add<Ast::Scope>("solver"), solverAst);
}

class ConversionLib : public Library
{
public:
    ConversionLib(World& w, Ast::Scope& parentScope, const std::string& conversionToolName, CellI& conversionToolAst, CellI& inputType, CellI& returnType);
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
    Library(w, parentScope)
{
    ConversionLibAst solverLibAst(w, parentScope.add<Ast::Scope>("conversion"), conversionToolName, conversionToolAst, inputType, returnType);
}

// ============================================================================
void ToolFinder::createConversionToolFromBlueprint(CellI& from, CellI& to, ToolFinder::ConversionToolBlueprint& blueprint, List& results)
{
    CellI* toolPtr = nullptr;
    CellI& blueprintTool = *blueprint.m_tool;
    CellI& blueprintKey  = *blueprint.m_slotId;

    CellI* missingSlotIdPtr = findMissingParameterKey(blueprint);
    if (!missingSlotIdPtr) {
        return; // TODO
    }

    // this is the from in "tool(from, x) == to"
    Object unknownX(w, std.op.ConstVar, "unknownX");
    CellI& missingSlotId = *missingSlotIdPtr;
    if (blueprintTool.has(w.id.primitiveTool)) {
        Map& membersMapping = static_cast<Map&>(blueprintTool[w.id.ast][w.id.memberMapping]);
        CellI& tool         = *new Object(w, blueprintTool);
        toolPtr             = &tool;
        tool.set(membersMapping.getValue(blueprintKey), w.op.const_(from));
        tool.set(membersMapping.getValue(missingSlotId), w.op.get(w.op.unknown_(unknownX), w.op.const_(id.value)));
    } else {
        CellI& tool = *new Object(w, std.op.Call);
        toolPtr     = &tool;
        tool.set(id.method, *blueprint.m_tool);
        Map& parameters = w.op.parameters(blueprintKey, w.op.const_(from), missingSlotId, w.op.get(w.op.unknown_(unknownX), w.op.const_(id.value)));
        tool.set(id.parameters, parameters);
    }
    CellI& tool = *toolPtr;
    CellI& missingSlotEquation = w.op.equal(tool, w.op.const_(to));
    missingSlotEquation.label("tool(from, x) == to");
    List& missingSlotSolvers = findToolsByEffect(missingSlotEquation);

    for (CellI& missingSlotSolver : missingSlotSolvers) {
        //        std::cout << blueprint << '\n';
        printAsValue(missingSlotSolver, "");
        continue; // TODO
        missingSlotSolver();

        CellI& solvedX = unknownX[id.value];
        //        std::cout << "unknownX.value = " << solvedX.label() << std::endl;

        Ast::Scope rootScope2(w, "toolFinder");
        Compiler compiler2(w);
        std::string conversionToolName = fmt::format("conversionToolFor{}", blueprint.m_tool->label());
        CellI& conversionToolAst       = *new Object(w, std.ast.Function);
        conversionToolAst.set(*blueprint.m_slotId, w.ast.parameter(w.name("from")));
        conversionToolAst.set(missingSlotId, w.ast._(solvedX));
        ConversionLib conversionLib(w, rootScope2, conversionToolName, conversionToolAst, from.__type__(), to.__type__());
        conversionLib.include(w.arcLib());
        compiler2.compile(conversionLib);
        auto& conversionToolFn = conversionLib.getFunction(fmt::format("conversion::{}", conversionToolName));

        printAsValue(conversionToolFn, "");
        results.add(conversionToolFn);
    }
}

// ============================================================================
CellI& ToolFinder::findConversionTools(CellI& from, CellI& to)
{
    printAsValue(from, "from");
    printAsValue(to, "to");

    List& results = *new List(w, std.List);
    findConversionToolsByValue(from, to, results);
    findConversionToolsByType(from, to, results);
    findConversionToolsByContainer(from, to, results);

    std::cout << "";

    return results;
}

// ============================================================================
void ToolFinder::exploreSlotManipulations()
{
    ConversionToolKey conversionToolKey(std.Number, std.Number);
    Object& x = *new Object(w, std.Number, "X");

    for (CellI& tool : m_tools) {
        TRACE(toolFinder, "explore: {}", tool.label());

        if (tool.missing(id.returnType)) {
            continue;
        }
        CellI& returnType = tool[id.returnType];
        if (&returnType != &std.Number) {
            continue;
        }

        auto& numberTool = *new Object(w, tool);
        numberTool.set(id.lhs, w.op.const_(2));
        numberTool.set(id.rhs, w.op.unknown_(x));

        auto& opEqual = w.op.equal(numberTool, w.op.const_(4));
        opEqual.label(fmt::format("{}(x, y) == z", tool.label()));
        DEBUG(toolFinderExplore, "equation: {}", opEqual.printAsValue());

        List& tools1 = findToolsByEffect(opEqual);
        for (auto& tool1 : tools1) {
            DEBUG(toolFinderExplore, "  1. result: {}", tool1.printAsValue());
            List& tools2 = findToolsByEffect(tool1);
            for (auto& tool2 : tools2) {
                DEBUG(toolFinderExplore, "    2. result: {}", tool2.printAsValue());
            }
        }
    }
}

// ============================================================================
void ToolFinder::findConversionToolsByValue(CellI& from, CellI& to, List& results)
{
    ConversionToolKey conversionToolKey(from, to);

    auto tools = m_conversionTools.equal_range(conversionToolKey);
    if (tools.first != m_conversionTools.end()) {
        std::cout << conversionToolKey << '\n';
    }
    for (auto it = tools.first; it != tools.second; ++it) {
        ConversionToolBlueprint blueprint = it->second;
        //        std::cout << "  " << blueprint << '\n';
        // ConversionToolKey [from: Number, to: Number]: ConversionToolBlueprint [tool: Add, input: lhs]
        createConversionToolFromBlueprint(from, to, blueprint, results);
    }
}

// ============================================================================
void ToolFinder::findConversionToolsByType(CellI& from, CellI& to, List& results)
{
    CellI& inputType  = from.__type__();
    CellI& outputType = to.__type__();

    ConversionToolKey conversionToolKey(inputType, outputType);

    auto tools = m_conversionTools.equal_range(conversionToolKey);
    if (tools.first != m_conversionTools.end()) {
        std::cout << conversionToolKey << '\n';
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

} // namespace cells
} // namespace infocell

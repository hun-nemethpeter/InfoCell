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

List& ToolFinder::callSlotKeyList()
{
    if (!m_callSlotKeyList) {
        m_callSlotKeyList = std::make_unique<List>(w, w.std.ast.Slot);
        m_callSlotKeyList->add(id.__type__);
        m_callSlotKeyList->add(id.method);
        m_callSlotKeyList->add(id.self);
        m_callSlotKeyList->add(id.parameters);
    }

    return *m_callSlotKeyList;
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
    m_tools(w, w.std.String)
{
    m_root = std::make_unique<Node>();
}

// ============================================================================
bool ToolFinder::empty()
{
    return m_root->m_children.empty();
}

// ============================================================================
CellI& ToolFinder::serializeEffect(CellI& effect)
{
    std::deque<StackNode> stack;

    List& ret           = *new List(w, w.std.Cell);
    CellI* slotItemPtr  = &callSlotKeyList()[id.first];
    CellI* paramItemPtr = nullptr;
    CellI* currentPtr   = &effect;

    while (slotItemPtr) {
        CellI& slotItem = *slotItemPtr;
        CellI& current  = *currentPtr;
        CellI& key      = slotItem[id.value];

        if (&key == &id.__type__) {
            ret.add(id.op);
            ret.add(id.type);
            ret.add(current.__type__());
        } else if (&key == &id.method) {
            ret.add(key);
            ret.add(current[key]);
        } else {
            CellI* keyPtr   = nullptr;
            CellI* valuePtr = nullptr;

            if (&key == &id.self) {
                keyPtr   = &key;
                valuePtr = &current[key];
            } else if (&key == &id.parameters && current.has(key)) {
                if (!paramItemPtr) {
                    paramItemPtr = &current[id.parameters][id.first];
                }
                CellI& paramSlot = (*paramItemPtr)[id.value];
                keyPtr           = &paramSlot[id.key];
                valuePtr         = &paramSlot[id.value];
            }
            if (keyPtr && valuePtr) {
                CellI& paramKey   = *keyPtr;
                CellI& paramValue = *valuePtr;

                ret.add(paramKey);
                if (&paramValue.__type__() == &w.std.op.ConstVar) {
                    ret.add(paramValue[id.value]);
                    if (&paramValue[id.value] == &id.op) {
                        // handling the _(op) case, where op is a constant
                        // it will be op op, so the escaping character here is op
                        ret.add(paramValue[id.value]);
                    }
                } else if (&paramValue.__type__() == &w.std.ast.Self || &paramValue.__type__() == &w.std.ast.Return || &paramValue.__type__() == &w.std.ast.Parameter || &paramValue.__type__() == &w.std.op.UnknownVar) {
                    ret.add(id.op);
                    if (&paramValue.__type__() == &w.std.ast.Self) {
                        ret.add(id.variable);
                    } else if (&paramValue.__type__() == &w.std.ast.Return) {
                        ret.add(id.variable);
                    } else if (&paramValue.__type__() == &w.std.ast.Parameter) {
                        ret.add(id.variable);
                    } else if (&paramValue.__type__() == &w.std.op.UnknownVar) {
                        ret.add(id.variable);
                    } else {
                        panic("Unknow param type!");
                    }
                } else if (&paramValue.__type__() == &w.std.op.Call) {
                    ret.add(id.op);
                    ret.add(id.push);
                    stack.push_back({ currentPtr, slotItemPtr, paramItemPtr });
                    currentPtr   = &paramValue;
                    slotItemPtr  = &callSlotKeyList()[id.first];
                    paramItemPtr = nullptr;
                    continue;
                } else {
                    std::cout << "";
                }
            }
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
void ToolFinder::addValue(Node*& node, CellI& value)
{
    Node*& childNode = node->m_children[&value];
    if (childNode == nullptr) {
        childNode = new Node();
        childNode->m_parent = node;
    }
    node = childNode;
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
        List& path = *new List(w, w.std.Cell, fmt::format("path for {}", memberKey.label()));
        for (auto& stackItem : stack) {
            path.add(getSlotForKey((*stackItem.slotItemPtr)[id.value]));
        }
        path.add(getSlotForKey(key));
        memberIds.add(memberKey, path);
    }
}

// ============================================================================
void ToolFinder::add(CellI& effect, CellI& tool)
{
    std::deque<StackNode> stack;

    Node* currentNode      = m_root.get();
    CellI* slotItemPtr     = &callSlotKeyList()[id.first];
    CellI* paramItemPtr    = nullptr;
    CellI* currentPtr      = &effect;
    bool hasReturnInEffect = false;
    Map memberIds(w, w.std.Cell, w.std.Cell);

    while (slotItemPtr) {
        CellI& slotItem = *slotItemPtr;
        CellI& current  = *currentPtr;
        CellI& key      = slotItem[id.value];

        if (&key == &id.__type__) {
            addValue(currentNode, id.op);
            addValue(currentNode, id.type);
            addValue(currentNode, current.__type__());
        } else if (&key == &id.method) {
            addValue(currentNode, key);
            addValue(currentNode, current[key]);
        } else {
            CellI* keyPtr   = nullptr;
            CellI* valuePtr = nullptr;

            if (&key == &id.self) {
                keyPtr   = &key;
                valuePtr = &current[key];
            } else if (&key == &id.parameters && current.has(key)) {
                if (!paramItemPtr) {
                    paramItemPtr = &current[id.parameters][id.first];
                }
                CellI& paramSlot = (*paramItemPtr)[id.value];
                keyPtr           = &paramSlot[id.key];
                valuePtr         = &paramSlot[id.value];
            }
            if (keyPtr && valuePtr) {
                CellI& paramKey   = *keyPtr;
                CellI& paramValue = *valuePtr;

                addValue(currentNode, paramKey);
                if (&paramValue.__type__() == &w.std.ast.ConstVar) {
                    addValue(currentNode, paramValue[id.value]);
                    if (&paramValue[id.value] == &id.op) {
                        // handling the _(op) case, where op is a constant
                        // it will be op op, so the escaping character here is op
                        addValue(currentNode, paramValue[id.value]);
                    }
                } else if (&paramValue.__type__() == &w.std.ast.Self ||
                           &paramValue.__type__() == &w.std.ast.Return ||
                           &paramValue.__type__() == &w.std.ast.Parameter) {
                    addValue(currentNode, id.op);
                    CellI* memberKeyPtr = nullptr;
                    if (&paramValue.__type__() == &w.std.ast.Self) {
                        addValue(currentNode, id.variable);
                        memberKeyPtr = &id.self;
                    } else if (&paramValue.__type__() == &w.std.ast.Return) {
                        addValue(currentNode, id.variable);
                        memberKeyPtr      = &id.return_;
                        hasReturnInEffect = true;
                    } else if (&paramValue.__type__() == &w.std.ast.Parameter) {
                        if (paramValue.has(id.value)) {
                            std::cout << "";
                        }
                        addValue(currentNode, id.variable);
                        memberKeyPtr = &paramValue[id.key];
                    } else {
                        panic("Unknow param type!");
                    }
                    saveCurrentPath(paramKey, *memberKeyPtr, memberIds, stack);
                } else if (&paramValue.__type__() == &w.std.op.Call) {
                    addValue(currentNode, id.op);
                    addValue(currentNode, id.push);
                    stack.push_back({ currentPtr, slotItemPtr, paramItemPtr });
                    currentPtr   = &paramValue;
                    slotItemPtr  = &callSlotKeyList()[id.first];
                    paramItemPtr = nullptr;
                    continue;
                }
            }
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
        currentNode->m_builders = new List(w, w.std.List, "builders");
    } else {
        std::cout << "";
    }
    currentNode->m_builders->add(createBuilder(tool, memberIds, hasReturnInEffect));
    currentNode->m_tool   = &tool;
    currentNode->m_effect = &effect;

    if (IS_LOG_ENABLED) {
        CellI& astAsList = serializeEffect(effect);
        std::stringstream ss;
        for (CellI& value: astAsList) {
            ss << value.label() << " ";
        }
        TRACE(toolFinder, "  {}", ss.str());
    }
}

// ============================================================================
List& ToolFinder::createBuilder(CellI& tool, Map& memberIds, bool hasReturnInEffect)
{
    List& builder = *new List(w, w.std.Cell, fmt::format("builder for {}", tool.label()));

    if (hasReturnInEffect) {
        builder.add(w.ast.member(id.__type__));
        builder.add(w.ast.primitiveToolName(w.std.op.Equal[id.ast]));
        builder.add(w.ast.member(id.lhs));
        builder.add(w.id.push);
    }
    builder.add(w.ast.member(id.__type__));
    if (tool[id.ast].has(w.id.primitiveTool)) {
        builder.add(w.ast.primitiveToolName(tool[id.ast]));
    } else {
        builder.add(w.ast._(w.std.op.Call));
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
bool ToolFinder::checkValue(Node*& node, CellI& key, CellI& value, bool& needPush)
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

            for (auto& [opKey, nextNode] : opNode->m_children) {
                if (opKey == &id.variable) {
                    TRACE(toolFinderLookup, "MATCH: variable");
                    node = nextNode;
                    return true;
                }
                if (opKey == &id.push && (&value.__type__() != &w.std.ast.ConstVar)) {
                    TRACE(toolFinderLookup, "MATCH: push");

                    if (&value.__type__() != &w.std.op.Call) {
                        panic("Not supported type!");
                    }
                    node     = nextNode;
                    needPush = true;
                    return true;
                }
                if (opKey == &id.pop) {
                    TRACE(toolFinderLookup, "MATCH: pop");
                    node = nextNode;
                    return true;
                }
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
    List& ret = *new List(w, w.std.ast.Base);
    if (checkUnknownsInTool(effect)) {
        std::cout << "Unknown in the effect" << std::endl;
    } else {
        std::cout << "Only constants in the effect" << std::endl;
    }
    List* buildersPtr = findBuildersForEffect(effect);
    if (!buildersPtr) {
        return ret;
    }
    for (auto& builder : *buildersPtr) {
        TRACE(toolFinder, "build with {}", builder.label());
        Object retVal(w, w.std.ast.ConstVar);
        buildTool({ retVal, w.ast.member(id.value), effect, builder });
        auto& tool = retVal[id.value];
        DEBUG(toolFinderLookup, "result: {}", tool.printAsValue());
        if (checkUnknownsInTool(tool)) {
            tool.set(id.state, w.std.op.State.missingInput);
            std::cout << "Unknown" << std::endl;
        } else {
            tool.set(id.state, w.std.op.State.start);
            std::cout << "Constant" << std::endl;
            //        tool();
        }
        ret.add(tool);
    }

    return ret;
}

// ============================================================================
List* ToolFinder::findBuildersForEffect(CellI& inputEffect)
{
    //    DEBUG(toolFinderLookup, "input: {}", inputEffectAst.printAsValue());
    Node* node          = m_root.get();
    CellI* effectPtr    = &inputEffect;
    CellI* slotItemPtr  = &callSlotKeyList()[id.first];
    CellI* paramItemPtr = nullptr;

    std::deque<StackNode> stack;

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
            keyPtr   = &(*slotItemPtr)[id.value];
            if (effect.missing(*keyPtr)) {
                return nullptr;
            }
            valuePtr = &effect[*keyPtr];
        }

        CellI& key   = *keyPtr;
        CellI& value = *valuePtr;

        bool needPush = false;
        if (checkValue(node, key, value, needPush) == false) {
            return nullptr;
        }
        if (needPush) {
            stack.push_back({ effectPtr, slotItemPtr, paramItemPtr });
            effectPtr    = &effect[key];
            slotItemPtr  = &callSlotKeyList()[id.first];
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
        return node->m_builders;
    }

    return nullptr;
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
    auto& ListOfCellStruct = w.getStruct(w.templateId("std::List", id.valueType, w.std.Cell));

    std::list<BuildToolInfo> toCreate;
    toCreate.push_back(buildToolInfo);

    auto getValuePtrFromValueCell = [this, &ListOfCellStruct](CellI& matchedEffect, CellI& valueCell) -> CellI* {
        if (&valueCell.__type__() == &w.std.ast.ConstVar) {
            return &valueCell;
        } else if (&valueCell.__type__() == &ListOfCellStruct) {
            CellI* valuePtr = &matchedEffect;
            for (CellI& pathItem : valueCell) {
                CellI& currentValue = *valuePtr;
                if (&pathItem.__type__() == &w.std.ast.Member) {
                    valuePtr = &currentValue[pathItem[id.key]];
                } else if (&pathItem.__type__() == &w.std.ast.Parameter) {
                    Map& parameters = static_cast<Map&>(currentValue[id.parameters]);
                    CellI& key      = pathItem[id.key];
                    valuePtr        = &parameters.getValue(key)[id.value];
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
                if (&key.__type__() != &w.std.ast.Member || (&key[id.key] != &id.__type__)) {
                    panic("The first item in a builder must be a member with a value of __type__!");
                }
                first               = false;
                CellI& nextSlotItem = (*slotItemPtr)[id.next];
                CellI& valueCell    = nextSlotItem[id.value];
                Object* newObj      = nullptr;
                CellI* typePtr      = nullptr;
                if (&valueCell.__type__() == &w.std.ast.ConstVar) {
                    typePtr     = &valueCell[id.value];
                    newObj      = new Object(w, *typePtr, fmt::format("built from {}", builder.label()));
                    slotItemPtr = &nextSlotItem;
                } else if (&valueCell.__type__() == &w.std.ast.PrimitiveToolName) {
                    CellI& ast = valueCell[id.name];
                    typePtr    = &ast[w.id.primitiveTool];
                    newObj     = new Object(w, *typePtr, fmt::format("built from {}", builder.label()));
                    (*newObj).set(w.id.ast, ast);
                    primitiveToolPtr = &ast;
                } else {
                    panic("Builder type is not a constant value!");
                }
                newObj->set(id.state, w.std.op.State.start); // TODO
                CellI& type = *typePtr;
                CellI& retKey = *retKeyPtr;
                if (&retKey.__type__() == &w.std.ast.Member) {
                    retPtr->set(retKey[id.key], *newObj);
                } else if (&retKey.__type__() == &w.std.ast.Parameter) {
                    CellI& paramKey = retKey[id.key];
                    CellI& slot     = *new Object(w, w.std.ast.Slot);
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
                if (!(&(*valuePtr).__type__() == &w.std.op.UnknownVar || &(*valuePtr).__type__() == &w.std.op.ConstVar)) {
                    subEffects.push_back({ retPtr, &w.ast.member(translatedKey), valuePtr });
                    TRACE(toolFinderLookup, "BUILD: '{}' is a sub effect", unwrappedKey.label());
                } else {
                    ret.set(translatedKey, *valuePtr);
                    TRACE(toolFinderLookup, "BUILD: '{}':{}", translatedKey.label(), (*valuePtr).label());
                }
            } else if (&key.__type__() == &w.std.ast.Member) {
                CellI& unwrappedKey = key[id.key];
                CellI& nextSlotItem = (*slotItemPtr)[id.next];
                CellI& valueCell    = nextSlotItem[id.value];
                CellI* valuePtr     = getValuePtrFromValueCell(matchedEffect, valueCell);

                if (!(&(*valuePtr).__type__() == &w.std.op.UnknownVar || &(*valuePtr).__type__() == &w.std.op.ConstVar)) {
                    subEffects.push_back({ retPtr, &key, valuePtr });
                    TRACE(toolFinderLookup, "BUILD: '{}' is a sub effect", unwrappedKey.label());
                } else {
                    ret.set(unwrappedKey, *valuePtr);
                    TRACE(toolFinderLookup, "BUILD: '{}':{}", unwrappedKey.label(), (*valuePtr).label());
                }
                slotItemPtr = &nextSlotItem;
            } else if (&key.__type__() == &w.std.ast.Parameter) {
                CellI& unwrappedKey = key[id.key];
                CellI& nextSlotItem = (*slotItemPtr)[id.next];
                CellI& valueCell    = nextSlotItem[id.value];
                CellI* valuePtr     = getValuePtrFromValueCell(matchedEffect, valueCell);

                if (ret.missing(id.parameters)) {
                    ret.set(id.parameters, *new Map(w, w.std.Cell, w.std.ast.Slot));
                    TRACE(toolFinderLookup, "BUILD: parameters");
                }
                auto& parameters = static_cast<Map&>(ret[id.parameters]);
                if (!(&(*valuePtr).__type__() == &w.std.op.UnknownVar || &(*valuePtr).__type__() == &w.std.op.ConstVar)) {
                    subEffects.push_back({ retPtr, &key, valuePtr });
                    TRACE(toolFinderLookup, "BUILD: param: '{}' is a sub effect", unwrappedKey.label());
                } else {
                    CellI& slot = *new Object(w, w.std.ast.Slot);
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
    if (&effect.__type__() == &w.std.op.ConstVar) {
        return false;
    } else if (&effect.__type__() == &w.std.op.UnknownVar) {
        return true;
    } else if (&effect.__type__() == &w.std.op.Call) {
        if (checkUnknownsInTool(effect[id.self])) {
            return true;
        }
        if (effect.has("parameters")) {
            for (CellI& parameter : effect[id.parameters]) {
                if (checkUnknownsInTool(parameter[w.id.value])) {
                    return true;
                }
            }
        }
        return false;
    } else if (effect.has(id.ast) && effect[id.ast].has(id.primitiveTool)) {
        CellI& ast         = effect[id.ast];
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
    CellI& tool = *new Object(w, w.std.op.Call);
    CellI& ast  = (*blueprint.m_tool)[w.id.ast];
    if (ast.has(w.id.primitiveTool)) {
        tool.set(id.method, ast[w.id.primitiveTool]);
    } else {
        tool.set(id.method, *blueprint.m_tool);
    }
    CellI& key = *blueprint.m_slotId;
    if (&key == &w.id.self) {
        tool.set(w.id.self, w.op.const_(from));
    } else {
        tool.set(id.parameters, w.op.parameters(key, w.op.const_(from)));
    }

    Object unknownX(w, w.std.op.ConstVar, "unknownX");

    CellI* missingSlotIdPtr = findMissingParameterKey(blueprint);
    if (!missingSlotIdPtr) {
        return; // TODO
    }
    CellI& missingSlotId = *missingSlotIdPtr;

    // this is the from in "tool(from, x) == to"
    tool.set(missingSlotId, w.op.get(w.op.unknown_(unknownX), w.op.const_(id.value)));

    CellI& missingSlotEquation = w.op.equal(tool, w.op.const_(to));
    missingSlotEquation.label("tool(from, x) == to");

    List& missingSlotSolvers = findToolsByEffect(missingSlotEquation);

    return; // TODO
    for (CellI& missingSlotSolver : missingSlotSolvers) {
        //        std::cout << blueprint << '\n';
        missingSlotSolver();

        CellI& solvedX = unknownX[id.value];
        //        std::cout << "unknownX.value = " << solvedX.label() << std::endl;

        Ast::Scope rootScope2(w, "toolFinder");
        Compiler compiler2(w);
        std::string conversionToolName = fmt::format("conversionToolFor{}", blueprint.m_tool->label());
        CellI& conversionToolAst       = *new Object(w, w.std.ast.Function);
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

    List& results = *new List(w, w.std.List);
    findConversionToolsByValue(from, to, results);
    findConversionToolsByType(from, to, results);
    findConversionToolsByContainer(from, to, results);

    std::cout << "";

    return results;
}

// ============================================================================
void ToolFinder::exploreSlotManipulations()
{
    ConversionToolKey conversionToolKey(w.std.Number, w.std.Number);
    Object& x = *new Object(w, w.std.Number, "X");
    Object& y = *new Object(w, w.std.Number, "Y");
    Object& z = *new Object(w, w.std.Number, "Z");

    for (CellI& tool : m_tools) {
        TRACE(toolFinder, "explore: {}", tool.label());

        if (tool.missing(id.returnType)) {
            continue;
        }
        CellI& returnType = tool[id.returnType];
        if (&returnType != &w.std.Number) {
            continue;
        }

        auto& numberTool = w.op.call(w.op.const_(x), tool[w.id.ast][w.id.primitiveTool]);
        numberTool(id.other, w.op.const_(y));
//        static_cast<Object&>(numberTool).runAsCall();

        auto& opEqual = w.op.equal(numberTool, w.op.const_(z));
        opEqual.label(fmt::format("{}(x, y) == z", tool.label()));
        DEBUG(toolFinderLookup, "equation: {}", opEqual.printAsValue());

        CellI* outputEffect = nullptr;
        CellI* buildersPtr  = findBuildersForEffect(opEqual);
        if (!buildersPtr) {
            continue;
        }
        for (auto& builder : *buildersPtr) {
            Object retVal(w, w.std.ast.ConstVar);
            buildTool({ retVal, w.ast.member(id.value), opEqual, builder });
            CellI& result = retVal[id.value];
            DEBUG(toolFinderLookup, "result: {}", result.printAsValue());
            CellI* builders2ndLevelPtr = findBuildersForEffect(result);
            if (builders2ndLevelPtr) {
                std::cout << "";
            } else {
                std::cout << "";
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

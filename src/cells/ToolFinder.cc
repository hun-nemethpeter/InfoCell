#include "ToolFinder.h"

#include "Compiler.h"
#include "World.h"
#include "cells/printers/ValuePrinter.h"

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

    if (tool[w.id.name].label() == "Add") {
        ss << "m_lhs + m_rhs";
    } else if (tool[w.id.name].label() == "Subtract") {
        ss << "m_lhs - m_rhs";
    } else if (tool.has("primitiveTool")) {
        ss << tool[w.id.name].label() << "(";
        if (tool.has("members")) {
            forEach(tool[w.id.members][w.id.list], [this, &ss](CellI& slot, int i, bool& stop) {
                if (i > 0) {
                    ss << ", ";
                }
                ss << "m_" << slot[w.id.key].label();
            });
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
        m_callSlotKeyList->add(w.id.__type__);
        m_callSlotKeyList->add(w.id.method);
        m_callSlotKeyList->add(w.id.cell);
        m_callSlotKeyList->add(w.id.parameters);
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
    std::deque<StackNodeNew> stack;

    List& ret           = *new List(w, w.std.Cell);
    CellI* slotItemPtr  = &callSlotKeyList()[w.id.first];
    CellI* paramItemPtr = nullptr;
    CellI* currentPtr   = &effect;

    while (slotItemPtr) {
        CellI& slotItem = *slotItemPtr;
        CellI& current  = *currentPtr;
        CellI& key      = slotItem[w.id.value];

        if (&key == &w.id.__type__) {
            ret.add(w.id.op);
            ret.add(w.id.type);
            ret.add(current.__type__());
        } else if (&key == &w.id.method) {
            ret.add(key);
            ret.add(current[key]);
        } else {
            CellI* keyPtr   = nullptr;
            CellI* valuePtr = nullptr;

            if (&key == &w.id.cell) {
                keyPtr   = &key;
                valuePtr = &current[key];
            } else if (&key == &w.id.parameters && current.has(key)) {
                if (!paramItemPtr) {
                    paramItemPtr = &current[w.id.parameters][w.id.first];
                }
                CellI& paramSlot = (*paramItemPtr)[w.id.value];
                keyPtr           = &paramSlot[w.id.key];
                valuePtr         = &paramSlot[w.id.value];
            }
            if (keyPtr && valuePtr) {
                CellI& paramKey   = *keyPtr;
                CellI& paramValue = *valuePtr;

                ret.add(paramKey);
                if (&paramValue.__type__() == &w.std.ast.Cell) {
                    ret.add(paramValue[w.id.value]);
                    if (&paramValue[w.id.value] == &w.id.op) {
                        // handling the _(op) case, where op is a constant
                        // it will be op op, so the escaping character here is op
                        ret.add(paramValue[w.id.value]);
                    }
                } else if (&paramValue.__type__() == &w.std.ast.Self || &paramValue.__type__() == &w.std.ast.Return || &paramValue.__type__() == &w.std.ast.Parameter) {
                    ret.add(w.id.op);
                    if (&paramValue.__type__() == &w.std.ast.Self) {
                        ret.add(w.id.variable);
                    } else if (&paramValue.__type__() == &w.std.ast.Return) {
                        ret.add(w.id.variable);
                    } else if (&paramValue.__type__() == &w.std.ast.Parameter) {
                        ret.add(w.id.variable);
                    } else {
                        throw "Unknow param type!";
                    }
                } else if (&paramValue.__type__() == &w.std.op.Call) {
                    ret.add(w.id.op);
                    ret.add(w.id.push);
                    stack.push_back({ current, *slotItemPtr, *paramItemPtr });
                    currentPtr   = &paramValue;
                    slotItemPtr  = &callSlotKeyList()[w.id.first];
                    paramItemPtr = nullptr;
                    continue;
                }
            }
        }

        if (&key == &w.id.parameters && paramItemPtr) {
            CellI& paramItem = *paramItemPtr;
            paramItemPtr     = paramItem.has(w.id.next) ? &paramItem[w.id.next] : nullptr;
            if (paramItemPtr) {
                continue;
            } else {
                paramItemPtr = nullptr;
            }
        }
        slotItemPtr = slotItem.has(w.id.next) ? &slotItem[w.id.next] : nullptr;
        while (!slotItemPtr && !stack.empty()) {
            currentPtr   = &stack.back().ast;
            slotItemPtr  = &stack.back().slotItem;
            paramItemPtr = &stack.back().paramItem;
            stack.pop_back();
            ret.add(w.id.op);
            ret.add(w.id.pop);
            slotItemPtr = (*slotItemPtr).has(w.id.next) ? &(*slotItemPtr)[w.id.next] : nullptr;
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
    auto& effects = tool[w.id.description];
    if (tool.has(w.id.returnType)) {
        // so this can be a conversion tool
        CellI& returnType = tool[w.id.returnType];
        if (tool.has(w.id.parameters)) {
            forEach(tool[w.id.parameters][w.id.list], [this, &tool, &returnType](CellI& parameter, int i, bool& stop) {
                CellI& inputType = parameter[w.id.type];
                ConversionToolKey key(inputType, returnType);
                ConversionToolBlueprint blueprint(tool, parameter[w.id.key]);
                m_conversionTools.insert({ key, blueprint });
            });
        }
    }
    m_tools.add(tool);

    TRACE(toolFinder, "{} =>", tool.label());
    forEach(effects, [this, &tool](CellI& effect, int i, bool& stop) {
        add(effect, tool);
#if 0
        if (&effect.__type__() == &w.std.ast.Equal) {
            Object symmetricEffectAst(w, w.std.ast.Equal, "symmetric effect");
            symmetricEffectAst.set(w.id.rhs, effect[w.id.lhs]);
            symmetricEffectAst.set(w.id.lhs, effect[w.id.rhs]);
            add(symmetricEffectAst, tool, compiledToolType);
        }
#endif
    });
}

// ============================================================================
void ToolFinder::saveCurrentPath(CellI& key, CellI& memberKey, Map& memberIds, std::deque<StackNodeNew>& stack)
{
    if (!memberIds.hasKey(memberKey)) {
        List& path = *new List(w, w.std.Cell, fmt::format("path for {}", memberKey.label()));
        for (auto& stackItem : stack) {
            if (&stackItem.ast.__type__() == &w.std.ast.Return) {
                continue;
            }
            path.add(stackItem.slotItem[w.id.value]);
        }
        path.add(key);
        memberIds.add(memberKey, path);
    }
}

// ============================================================================
void ToolFinder::add(CellI& effect, CellI& tool)
{
    std::deque<StackNodeNew> stack;

    Node* currentNode   = m_root.get();
    CellI* slotItemPtr  = &callSlotKeyList()[w.id.first];
    CellI* paramItemPtr = nullptr;
    CellI* currentPtr   = &effect;
    Map memberIds(w, w.std.Cell, w.std.Cell);

    while (slotItemPtr) {
        CellI& slotItem = *slotItemPtr;
        CellI& current  = *currentPtr;
        CellI& key      = slotItem[w.id.value];

        if (&key == &w.id.__type__) {
            addValue(currentNode, w.id.op);
            addValue(currentNode, w.id.type);
            addValue(currentNode, current.__type__());
        } else if (&key == &w.id.method) {
            addValue(currentNode, key);
            addValue(currentNode, current[key]);
        } else {
            CellI* keyPtr   = nullptr;
            CellI* valuePtr = nullptr;

            if (&key == &w.id.cell) {
                keyPtr   = &key;
                valuePtr = &current[key];
            } else if (&key == &w.id.parameters && current.has(key)) {
                if (!paramItemPtr) {
                    paramItemPtr = &current[w.id.parameters][w.id.first];
                }
                CellI& paramSlot = (*paramItemPtr)[w.id.value];
                keyPtr           = &paramSlot[w.id.key];
                valuePtr         = &paramSlot[w.id.value];
            }
            if (keyPtr && valuePtr) {
                CellI& paramKey   = *keyPtr;
                CellI& paramValue = *valuePtr;

                addValue(currentNode, paramKey);
                if (&paramValue.__type__() == &w.std.ast.Cell) {
                    addValue(currentNode, paramValue[w.id.value]);
                    if (&paramValue[w.id.value] == &w.id.op) {
                        // handling the _(op) case, where op is a constant
                        // it will be op op, so the escaping character here is op
                        addValue(currentNode, paramValue[w.id.value]);
                    }
                } else if (&paramValue.__type__() == &w.std.ast.Self ||
                           &paramValue.__type__() == &w.std.ast.Return ||
                           &paramValue.__type__() == &w.std.ast.Parameter) {
                    addValue(currentNode, w.id.op);
                    CellI* memberKeyPtr = nullptr;
                    if (&paramValue.__type__() == &w.std.ast.Self) {
                        addValue(currentNode, w.id.variable);
                        memberKeyPtr = &w.id.self;
                    } else if (&paramValue.__type__() == &w.std.ast.Return) {
                        addValue(currentNode, w.id.variable);
                        memberKeyPtr = &w.id.return_;
                    } else if (&paramValue.__type__() == &w.std.ast.Parameter) {
                        addValue(currentNode, w.id.variable);
                        memberKeyPtr = &paramValue[w.id.key];
                    } else {
                        throw "Unknow param type!";
                    }
                    saveCurrentPath(paramKey, *memberKeyPtr, memberIds, stack);
                } else if (&paramValue.__type__() == &w.std.op.Call) {
                    addValue(currentNode, w.id.op);
                    addValue(currentNode, w.id.push);
                    stack.push_back({ current, *slotItemPtr, *paramItemPtr });
                    currentPtr   = &paramValue;
                    slotItemPtr  = &callSlotKeyList()[w.id.first];
                    paramItemPtr = nullptr;
                    continue;
                }
            }
        }

        if (&key == &w.id.parameters && paramItemPtr) {
            CellI& paramItem = *paramItemPtr;
            paramItemPtr     = paramItem.has(w.id.next) ? &paramItem[w.id.next] : nullptr;
            if (paramItemPtr) {
                continue;
            } else {
                paramItemPtr = nullptr;
            }
        }
        slotItemPtr = slotItem.has(w.id.next) ? &slotItem[w.id.next] : nullptr;
        while (!slotItemPtr && !stack.empty()) {
            currentPtr   = &stack.back().ast;
            slotItemPtr  = &stack.back().slotItem;
            paramItemPtr = &stack.back().paramItem;
            stack.pop_back();
            addValue(currentNode, w.id.op);
            addValue(currentNode, w.id.pop);
            slotItemPtr = (*slotItemPtr).has(w.id.next) ? &(*slotItemPtr)[w.id.next] : nullptr;
        }
    }

    currentNode->m_isLeaf = 1;
    if (currentNode->m_data != nullptr) {
        std::cout << "";
    }
    currentNode->m_data   = createBuilder(tool, memberIds);
    currentNode->m_tool   = &tool;
    currentNode->m_effect = &effect;

    if (IS_LOG_ENABLED) {
        CellI& astAsList = serializeEffect(effect);
        std::stringstream ss;
        forEach(astAsList, [&ss](CellI& value, int, bool& stop) {
            ss << value.label() << " ";
        });
        TRACE(toolFinder, "  {}", ss.str());
    }
}

// ============================================================================
CellI* ToolFinder::createBuilder(CellI& tool, Map& memberIds)
{
    List& builder = *new List(w, w.std.Cell, fmt::format("builder for {}", tool.label()));

    builder.add(w._(w.id.__type__));
    builder.add(w._(w.std.op.Call));

    builder.add(w._(w.id.method));
    builder.add(w._(tool));

    forEach(tool[w.id.parameters][w.id.list], [this, &memberIds, &builder](CellI& slot, int, bool&) {
        CellI& key = slot[w.id.key];
        builder.add(w.ast.cell(key));
        builder.add(memberIds.getValue(key));
    });

    return &builder;
}

// ============================================================================
void ToolFinder::handleStep(CellI*& effectAstPtr, CellI*& slotItemPtr, Node*& node, std::deque<StackNode>& stack)
{
    slotItemPtr = (*slotItemPtr).has(w.id.next) ? &(*slotItemPtr)[w.id.next] : nullptr;
    while (!slotItemPtr) {
        if (stack.empty()) {
            return;
        }

        auto opFindIt = node->m_children.find(&w.id.op);
        if (opFindIt == node->m_children.end()) {
            return;
        }
        TRACE(toolFinderLookup, "MATCH: op");
        Node* opNode   = opFindIt->second;
        auto popFindIt = opNode->m_children.find(&w.id.pop);
        if (popFindIt == opNode->m_children.end()) {
            return;
        }
        node = popFindIt->second;

        slotItemPtr  = &stack.back().slotItem;
        effectAstPtr = &stack.back().ast;
        stack.pop_back();
        slotItemPtr = (*slotItemPtr).has(w.id.next) ? &(*slotItemPtr)[w.id.next] : nullptr;
        TRACE(toolFinderLookup, "MATCH: pop");
    }
}

// ============================================================================
bool ToolFinder::checkValue(FindContext& findContext, CellI& key, CellI& value)
{
    Node*& node                  = findContext.trieNode;
    CellI*& slotItemPtr          = findContext.slotItemPtr;
    SlotKind& slotKind           = findContext.slotKind;
    CellI*& effectAstPtr         = findContext.effectAstPtr;
    std::deque<StackNode>& stack = findContext.stack;

    auto keyFindIt = node->m_children.find(&key);
    if (keyFindIt == node->m_children.end()) {
        node = nullptr;
        return false;
    } else {
        TRACE(toolFinderLookup, "MATCH: {}", key.label());
        node = keyFindIt->second;
    }

    auto findIt = node->m_children.find(&value);
    if (findIt == node->m_children.end()) {
        auto opFindIt = node->m_children.find(&w.id.op);
        if (opFindIt == node->m_children.end()) {
            node = nullptr;
            return false;
        } else {
            // ok, so value not found but we have an op here
            Node* opNode = opFindIt->second;
            TRACE(toolFinderLookup, "MATCH: op");

            for (auto& [opKey, nextNode] : opNode->m_children) {
                if (opKey == &w.id.type) {
                    TRACE(toolFinderLookup, "MATCH: type");
                    node = nextNode;
                    handleStep(effectAstPtr, slotItemPtr, node, stack);
                    return true;
                }
                if (opKey == &w.id.variable) {
                    TRACE(toolFinderLookup, "MATCH: variable");
                    node = nextNode;
                    handleStep(effectAstPtr, slotItemPtr, node, stack);
                    return true;
                }
                if (opKey == &w.id.return_) {
                    TRACE(toolFinderLookup, "MATCH: return_");
                    if (findContext.toolKind == ToolKind::Expression) {
                        // TODO What to do if there are two "op return" in the effect description?
                        throw "Not implemented! Handling more then one op return is missing";
                    }
                    if (stack.empty()) {
                        // we matched with a return but we are not inside an equal expression
                        return false;
                    }
                    CellI& prevAst = stack.back().ast;
                    if (&prevAst.__type__() != &w.std.Cell.Equal) {
                        return false;
                    }
                    CellI& prevSlotKey  = stack.back().slotItem[w.id.value][w.id.key];
                    CellI& otherSlotKey = (&prevSlotKey == &w.id.lhs) ? w.id.rhs : w.id.lhs;
                    CellI& returnValue  = prevAst[otherSlotKey];

                    node = nextNode;
                    handleStep(effectAstPtr, slotItemPtr, node, stack);
                    findContext.toolKind          = ToolKind::Expression;
                    findContext.expressionToolPtr = &(*effectAstPtr)[key];
                    DEBUG(toolFinderLookup, "unify return with {}", (*effectAstPtr)[key].printAsValue());
                    return true;
                }
                if (opKey == &w.id.push && (&value.__type__() != &w.std.ast.Cell)) {
                    TRACE(toolFinderLookup, "MATCH: push");
                    stack.push_back({ .ast = *effectAstPtr, .slotItem = *slotItemPtr });
                    effectAstPtr = &(*effectAstPtr)[key];
                    slotItemPtr  = &value.slotList()[w.id.first];
                    node         = nextNode;
                    slotKind     = SlotKind::TypeSlot;
                    return true;
                }
                if (opKey == &w.id.pop) {
                    TRACE(toolFinderLookup, "MATCH: pop");
                    if (stack.empty()) {
                        return false;
                    }
                    slotItemPtr  = &stack.back().slotItem;
                    effectAstPtr = &stack.back().ast;
                    stack.pop_back();
                    slotItemPtr = (*slotItemPtr).has(w.id.next) ? &(*slotItemPtr)[w.id.next] : nullptr;
                    node        = nextNode;
                    if (!slotItemPtr) {
                        handleStep(effectAstPtr, slotItemPtr, node, stack);
                    }
                    return true;
                }
            }
        }
    } else {
        TRACE(toolFinderLookup, "MATCH: {}", value.label());
        node = findIt->second;
    }

    // the first slot is the w.id.__type__ but it is not in the slot list
    if (slotKind == SlotKind::TypeSlot) {
        slotKind = SlotKind::NormalSlot;
        return true;
    }
    handleStep(effectAstPtr, slotItemPtr, node, stack);

    return true;
}

// ============================================================================
List& ToolFinder::findToolsByEffect(CellI& effectAst)
{
    List& ret              = *new List(w, w.std.ast.Base);
    CellI* outputEffectAst = nullptr;
    CellI* builder         = findBuilderForEffectAstOld(effectAst, outputEffectAst);
    if (!builder) {
        return ret;
    }
    Object retVal(w, w.std.ast.Cell);
    buildTool(retVal, w.id.value, *outputEffectAst, *builder);
    DEBUG(toolFinderLookup, "result: {}", retVal[w.id.value].printAsValue());

    ret.add(retVal[w.id.value]);

    return ret;
}

// ============================================================================
CellI* ToolFinder::findBuilderForEffectAstOld(CellI& inputEffectAst, CellI*& outputEffectAst)
{
//    DEBUG(toolFinderLookup, "input: {}", inputEffectAst.printAsValue());
    CellI& slotList         = inputEffectAst.slotList();
    FindContext findContext = {
        .trieNode     = m_root.get(),
        .slotList     = &slotList,
        .slotItemPtr  = slotList.has(w.id.first) ? &slotList[w.id.first] : nullptr,
        .slotKind     = SlotKind::TypeSlot,
        .effectAstPtr = &inputEffectAst,
    };

    do {
        findContext.toolKind = ToolKind::Statement;
        while (findContext.slotItemPtr) {
            if (findContext.slotKind == SlotKind::TypeSlot) {
                if (!checkValue(findContext, w.id.__type__, (*findContext.effectAstPtr).__type__())) {
                    return nullptr;
                }
            }

            CellI& key   = (*findContext.slotItemPtr)[w.id.value][w.id.key];
            CellI& value = (*findContext.effectAstPtr)[key];

            if ((*findContext.effectAstPtr).has(key) && !checkValue(findContext, key, value)) {
                return nullptr;
            }
        }
        if (findContext.toolKind == ToolKind::Expression) {
            if (!(findContext.trieNode && findContext.trieNode->m_isLeaf)) {
                return nullptr;
            }
            CellI& newEffectAst = *new Object(w, w.std.Cell.Equal); // TODO FIX memory leak

            newEffectAst.set(w.id.lhs, *findContext.expressionToolPtr);
            buildTool(newEffectAst, w.id.rhs, (*findContext.effectAstPtr), *findContext.trieNode->m_data);
            DEBUG(toolFinderLookup, "pattern match for {} with tool {}", findContext.trieNode->m_effect->printAsValue(), printTool(*findContext.trieNode->m_tool));
            DEBUG(toolFinderLookup, "created tool: {}", newEffectAst.printAsValue());

            CellI& newSlotList = newEffectAst.slotList();

            findContext.trieNode     = m_root.get();
            findContext.slotList     = &newSlotList;
            findContext.slotItemPtr  = newSlotList.has(w.id.first) ? &newSlotList[w.id.first] : nullptr;
            findContext.slotKind     = SlotKind::TypeSlot;
            findContext.effectAstPtr = &newEffectAst;
        }
    } while (findContext.toolKind == ToolKind::Expression);

    if (findContext.trieNode && findContext.trieNode->m_isLeaf) {
        outputEffectAst = findContext.effectAstPtr;
        CellI* result   = findContext.trieNode->m_data;
        return result;
    }

    return nullptr;
}

// ============================================================================
CellI* ToolFinder::findBuildersForEffect(CellI& inputEffectAst)
{
    DEBUG(toolFinderLookup, "input: {}", inputEffectAst.printAsValue());
    CellI& slotList         = inputEffectAst.slotList();
    FindContext findContext = {
        .trieNode     = m_root.get(),
        .slotList     = &slotList,
        .slotItemPtr  = slotList.has(w.id.first) ? &slotList[w.id.first] : nullptr,
        .slotKind     = SlotKind::TypeSlot,
        .effectAstPtr = &inputEffectAst,
    };

        findContext.toolKind = ToolKind::Statement;
    while (findContext.slotItemPtr) {
        if (findContext.slotKind == SlotKind::TypeSlot) {
            if (!checkValue(findContext, w.id.__type__, (*findContext.effectAstPtr).__type__())) {
                return nullptr;
            }
        }

        CellI& key   = (*findContext.slotItemPtr)[w.id.value][w.id.key];
        CellI& value = (*findContext.effectAstPtr)[key];

        if ((*findContext.effectAstPtr).has(key) && !checkValue(findContext, key, value)) {
            return nullptr;
        }
    }

    if (findContext.trieNode && findContext.trieNode->m_isLeaf) {
        CellI* result   = findContext.trieNode->m_data;
        return result;
    }

    return nullptr;
}

// ============================================================================
void ToolFinder::buildTool(CellI& outCell, CellI& outKey, CellI& inputAst, CellI& builder)
{
    auto& ListOfCellStruct = w.getStruct(w.templateId("std::List", w.id.valueType, w.std.Cell));

    World& w                = this->w;
    List& toCreate          = *new List(w, w.std.Cell);
    Index& toCreateItemRoot = *new Index(w);
    toCreateItemRoot.set(w.id.ast, inputAst);
    toCreateItemRoot.set(w.id.description, builder);
    toCreateItemRoot.set(w.id.cell, outCell);
    toCreateItemRoot.set(w.id.key, outKey);

    toCreate.add(toCreateItemRoot);
    CellI* toCreateItemPtr = &toCreate[w.id.first];
    while (toCreateItemPtr) {
        CellI& toCreateItem = (*toCreateItemPtr)[w.id.value];
        CellI& ast          = toCreateItem[w.id.ast];
        CellI& toolDesc     = toCreateItem[w.id.description];
        CellI* ret          = &toCreateItem[w.id.cell];
        CellI& retKey       = toCreateItem[w.id.key];

        CellI* slotItemPtr = &toolDesc[w.id.first];
        bool first         = true;
        List& subTools     = *new List(w, w.std.Cell);
        while (slotItemPtr) {
            CellI& key = (*slotItemPtr)[w.id.value];

            if (first) {
                if (&key.__type__() != &w.std.ast.Cell && (&key[w.id.value] != &w.id.__type__)) {
                    throw "Tool description without type!";
                }
                first               = false;
                CellI& nextSlotItem = (*slotItemPtr)[w.id.next];
                CellI& valueCell    = nextSlotItem[w.id.value];
                if (&valueCell.__type__() != &w.std.ast.Cell) {
                    throw "Tool description type is not constant value!";
                }
                CellI& type   = valueCell[w.id.value];
                CellI* newObj = new Object(w, type, fmt::format("built from {}", toolDesc.label()));
                ret->set(retKey, *newObj);
                ret = newObj;

                slotItemPtr = &nextSlotItem;
            } else if (&key.__type__() == &w.std.ast.Cell) {
                CellI& unwrappedKey = key[w.id.value];
                CellI& nextSlotItem = (*slotItemPtr)[w.id.next];
                CellI& valueCell    = nextSlotItem[w.id.value];
                CellI* valuePtr     = nullptr;
                if (&valueCell.__type__() == &w.std.ast.Cell) {
                    valuePtr = &ast[unwrappedKey];
                    ret->set(unwrappedKey, *valuePtr);
                } else if (&valueCell.__type__() == &ListOfCellStruct) {
                    valuePtr = &ast;
                    forEach(valueCell, [&valuePtr, &w](CellI& pathItem, int, bool& stop) {
                        CellI& currentValue = *valuePtr;
                        valuePtr            = &currentValue[pathItem];
                    });
                    ret->set(unwrappedKey, *valuePtr);
                } else {
                    throw "Tool description value is not a constant value or List!";
                }
                if (&(*valuePtr).__type__() != &w.std.ast.Cell) {
                    subTools.add(w.ast.slot(*ret, unwrappedKey));
                }
                slotItemPtr = &nextSlotItem;
            } else {
                throw "Tool description key is not constant value!";
            }

            slotItemPtr = (*slotItemPtr).has(w.id.next) ? &(*slotItemPtr)[w.id.next] : nullptr;
        }
        CellI* subpToolItemPtr = &subTools[w.id.first];
        while (subpToolItemPtr) {
            CellI& slot       = (*subpToolItemPtr)[w.id.value];
            CellI& key        = slot[w.id.key];
            CellI& value      = slot[w.id.type];
            CellI& subToolAst = key[value];

            CellI* toolAst        = nullptr;
            CellI* subToolBuilder = findBuilderForEffectAstOld(subToolAst, toolAst);

            if (!subToolBuilder) {
                throw "Sub tool not found!";
            }
            Index& toCreateItemSub = *new Index(w);
            toCreateItemSub.set(w.id.ast, subToolAst);
            toCreateItemSub.set(w.id.description, *subToolBuilder);
            toCreateItemSub.set(w.id.cell, (*ret));
            toCreateItemSub.set(w.id.key, value);
            toCreate.add(toCreateItemSub);

            CellI* toDelete = subpToolItemPtr;
            subpToolItemPtr = (*subpToolItemPtr).has(w.id.next) ? &(*subpToolItemPtr)[w.id.next] : nullptr;
            subTools.remove((List::Node*)toDelete);
        }
        CellI* toDelete = toCreateItemPtr;
        toCreateItemPtr = (*toCreateItemPtr).has(w.id.next) ? &(*toCreateItemPtr)[w.id.next] : nullptr;
        toCreate.remove((List::Node*)toDelete);
    }
}

// ============================================================================
static CellI* findMissingSlotId(CellI& tool, CellI& filledSlot)
{
    World& w          = tool.w;
    CellI& slotList   = tool.slotList();
    CellI* retPtr     = nullptr;
    forEach(slotList, [&w, &tool, &filledSlot, &retPtr](CellI& slot, int i, bool& stop) {
        CellI& slotKey = slot[w.id.key];
        if (&slotKey != &filledSlot) {
            retPtr = &slotKey;
            stop   = true;
            return;
        }
    });

    return retPtr;
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
    CellI& tool = *new Object(w, w.std.ast.Function);
    tool.set(*blueprint.m_slotId, w.ast.cell(from));

    Object unknownX(w, w.std.op.ConstVar, "unknownX");

    CellI* missingSlotIdPtr = findMissingSlotId(tool, *blueprint.m_slotId);
    if (!missingSlotIdPtr) {
        return; // TODO
    }
    CellI& missingSlotId = *missingSlotIdPtr;

    // this is the from in "tool(from, x) == to"
    tool.set(missingSlotId, w.ast.get(w._(unknownX), w._(w.id.value)));

    CellI& missingSlotEquation = w.ast.equal(tool, w._(to));
    missingSlotEquation.label("tool(from, x) == to");

    List& missingSlotSolvers = findToolsByEffect(missingSlotEquation);

    forEach(missingSlotSolvers, [&](CellI& missingSlotSolver, int i, bool&) {
        //        std::cout << blueprint << '\n';

        Ast::Scope rootScope(w, "toolFinder");
        Compiler compiler(w);
        SolverLib solverLib(w, rootScope, missingSlotSolver);
        solverLib.include(w.arcLib());
        compiler.compile(solverLib);
        auto& solverFn = solverLib.getFunction("solver::solverFunction");
        printAsValue(solverFn, "solverFn");
        solverFn.createSelfStack();
        solverFn();

        CellI& solvedX = unknownX[w.id.value];
        //        std::cout << "unknownX.value = " << solvedX.label() << std::endl;

        Ast::Scope rootScope2(w, "toolFinder");
        Compiler compiler2(w);
        std::string conversionToolName = fmt::format("conversionToolFor{}", blueprint.m_tool->label());
        CellI& conversionToolAst       = *new Object(w, w.std.ast.Function);
        conversionToolAst.set(*blueprint.m_slotId, w.ast.parameter(w.name("from")));
        conversionToolAst.set(missingSlotId, w._(solvedX));
        ConversionLib conversionLib(w, rootScope2, conversionToolName, conversionToolAst, from.__type__(), to.__type__());
        conversionLib.include(w.arcLib());
        compiler2.compile(conversionLib);
        auto& conversionToolFn = conversionLib.getFunction(fmt::format("conversion::{}", conversionToolName));

        printAsValue(conversionToolFn, "");
        results.add(conversionToolFn);
    });
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

    forEach(m_tools, [this](CellI& toolInfo, int i, bool&) {
        CellI& tool             = toolInfo["tool"];
        CellI& compiledToolType = toolInfo["compiledToolType"];
        TRACE(toolFinder, "explore: {}", tool.label());

        if (tool.missing(w.id.returnType)) {
            return;
        }
        CellI& returnType = tool[w.id.returnType][w.id.value];
        if (&returnType != &w.std.Number) {
            return;
        }

        CellI& astTool = *new Object(w, compiledToolType);
        astTool.set(w.id.lhs, w._(1));
        astTool.set(w.id.rhs, w._(2));

        CellI& astEqual = w.ast.equal(astTool, w._(3));
        astEqual.label(fmt::format("{}(x, y) == z", tool.label()));

        CellI* outputEffect = nullptr;
        CellI* builder      = findBuildersForEffect(astEqual);
        if (!builder) {
            return;
        }
        Object retVal(w, w.std.ast.Cell);
        buildTool(retVal, w.id.value, astEqual, *builder);
        DEBUG(toolFinderLookup, "result: {}", retVal[w.id.value].printAsValue());

    });
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

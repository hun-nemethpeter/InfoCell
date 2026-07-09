#include "ToolFinder.h"

#include "Compiler.h"
#include "World.h"
#include "cells/printers/ValuePrinter.h"

#define SPDLOG_ACTIVE_LEVEL SPDLOG_LEVEL_TRACE
#include "util/Log.h"

#include <sstream>

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

// ============================================================================
ToolFinder::Node::~Node()
{
    for (auto& pair : m_children) {
        delete pair.second;
    }
}

// ============================================================================
ToolFinder::ToolFinder(World& w) :
    w(w)
{
    m_root = std::make_unique<Node>();
}

// ============================================================================
bool ToolFinder::empty()
{
    return m_root->m_children.empty();
}

// ============================================================================
CellI& ToolFinder::serializeEffectAst(CellI& effectAst)
{
    CellI& slotList    = effectAst.slotList();
    CellI* slotItemPtr = slotList.has(w.id.first) ? &slotList[w.id.first] : nullptr;
    List& ret          = *new List(w, w.std.Cell);
    ret.label(effectAst.label());
    struct Context
    {
        CellI& ast;
        CellI& slotItem;
    };
    std::stack<Context> stack;
    bool first        = true;
    CellI* currentPtr = &effectAst;
    while (slotItemPtr) {
        CellI& slotItem = *slotItemPtr;
        CellI& slot     = slotItem[w.id.value];
        CellI& key      = slot[w.id.key];
        CellI& current  = *currentPtr;

        if (first) {
            first = false;
            ret.add(w.id.__type__);
            ret.add(current.__type__());
        }

        if (current.has(key)) {
            CellI& value = current[key];
            ret.add(key);
            if (&key == &w.id.__type__) {
                ret.add(value);
                if (&key == &w.id.op) {
                    ret.add(value);
                }
            } else if (&value.__type__() == &w.std.ast.Cell) {
                ret.add(value[w.id.value]);
                if (&value[w.id.value] == &w.id.op) {
                    ret.add(value[w.id.value]);
                }
#if 0
            } else if (&value.__type__() == &w.std.ast.Slot) {
                ret.add(value[w.id.type]);
#endif
            } else if (&value.__type__() == &w.std.ast.Member || &value.__type__() == &w.std.ast.Return) {
                ret.add(w.id.op);
                if (&value.__type__() == &w.std.ast.Member) {
                    ret.add(w.id.variable);
                } else {
                    ret.add(w.id.return_);
                }
            } else if ((&key != &w.id.__type__) && value.__type__()[w.id.memberOf][w.id.index].has(w.std.ast.Base)) {
                ret.add(w.id.op);
                ret.add(w.id.push);
                stack.push({ current, *slotItemPtr });
                first       = true;
                currentPtr  = &value;
                slotItemPtr = &value.slotList()[w.id.first];
                continue;
            }
        }

        slotItemPtr = slotItem.has(w.id.next) ? &slotItem[w.id.next] : nullptr;
        while (!slotItemPtr && !stack.empty()) {
            slotItemPtr = &stack.top().slotItem;
            currentPtr  = &stack.top().ast;
            stack.pop();
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

ToolFinder::ConversionToolBlueprint::ConversionToolBlueprint(CellI& tool, CellI& compiledToolType, CellI& slotId) :
    m_tool(&tool),
    m_compiledToolType(&compiledToolType),
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
void ToolFinder::add(CellI& tool, CellI& compiledToolType)
{
    auto& effects = tool[w.id.description][w.id.asts];
    if (tool.has(w.id.returnType)) {
        // so this can be a conversion tool
        CellI& returnType     = tool[w.id.returnType][w.id.value];
        CellI& toolInputSlots = compiledToolType[w.id.slots][w.id.list];
        forEach(toolInputSlots, [this, &tool, &compiledToolType, &returnType](CellI& slot, int i, bool& stop) {
            CellI& inputType = slot[w.id.type];
            ConversionToolKey key(inputType, returnType);
            ConversionToolBlueprint blueprint(tool, compiledToolType, slot[w.id.key]);
            m_conversionTools.insert({ key, blueprint });
        });
    }

    if (IS_LOG_ENABLED) {
        std::stringstream ss;

        ss << "" << tool.label() << "(";

        CellI& toolInputSlots = compiledToolType[w.id.slots][w.id.list];
        forEach(toolInputSlots, [this, &ss, &tool](CellI& slot, int i, bool& stop) {
            if (i > 0) {
                ss << ", ";
            }
            CellI& inputType = slot[w.id.type];
            ss << slot[w.id.key].label() << ": " << inputType.label();
        });
        ss << ")";
        if (tool.has(w.id.returnType)) {
            CellI& returnType = tool[w.id.returnType][w.id.value];
            ss << ": " << returnType.label();
        }
        TRACE(toolFinder, "{} =>", ss.str());
    }
    forEach(effects, [this, &tool, &compiledToolType](CellI& effect, int i, bool& stop) {
        add(effect, tool, compiledToolType);
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
void ToolFinder::saveCurrentPath(CellI& key, CellI& memberKey, Map& memberIds, std::deque<StackNode>& stack)
{
    if (!memberIds.hasKey(memberKey)) {
        List& path = *new List(w, w.std.Cell, fmt::format("path for {}", memberKey.label()));
        for (auto& stackItem : stack) {
            if (&stackItem.ast.__type__() == &w.std.ast.Return) {
                continue;
            }
            path.add(stackItem.slotItem[w.id.value][w.id.key]);
        }
        path.add(key);
        memberIds.add(memberKey, path);
    }
}

// ============================================================================
void ToolFinder::add(CellI& effect, CellI& tool, CellI& compiledToolType)
{
    std::deque<StackNode> stack;

    Node* currentNode  = m_root.get();
    CellI& slotList    = effect.slotList();
    CellI* slotItemPtr = slotList.has(w.id.first) ? &slotList[w.id.first] : nullptr;
    bool first         = true;
    CellI* currentPtr  = &effect;
    Map memberIds(w, w.std.Cell, w.std.Cell);

    while (slotItemPtr) {
        CellI& slotItem = *slotItemPtr;
        CellI& slot     = slotItem[w.id.value];
        CellI& key      = slot[w.id.key];
        CellI& current  = *currentPtr;

        if (first) {
            first = false;
            addValue(currentNode, w.id.__type__);
            addValue(currentNode, current.__type__());
        }

        if (current.has(key)) {
            CellI& value = current[key];
            addValue(currentNode, key);
            if (&value.__type__() == &w.std.ast.Cell) {
                addValue(currentNode, value[w.id.value]);
                if (&value[w.id.value] == &w.id.op) {
                    addValue(currentNode, value[w.id.value]);
                }
#if 0
            } else if (&value.__type__() == &w.std.ast.Slot) {
                addValue(currentNode, value[w.id.type]);
                memberIds.add(key, value);
//                ConversionToolKey conversionToolKey(value, returnType);
//                ConversionToolBlueprint blueprint(tool, compiledToolType, key);
//                m_conversionTools.insert({ conversionToolKey, blueprint });
#endif
            } else if (&value.__type__() == &w.std.ast.Member || &value.__type__() == &w.std.ast.Return) {
                addValue(currentNode, w.id.op);
                CellI* memberKeyPtr = nullptr;
                if (&value.__type__() == &w.std.ast.Member) {
                    addValue(currentNode, w.id.variable);
                    memberKeyPtr = &value[w.id.key];
                } else {
                    addValue(currentNode, w.id.return_);
                    // TODO w.id.return_ can not be a member name
                    memberKeyPtr = &w.id.return_;
                }
                saveCurrentPath(key, *memberKeyPtr, memberIds, stack);
            } else if ((&key != &w.id.__type__) && value.__type__()[w.id.memberOf][w.id.index].has(w.std.ast.Base)) {
                addValue(currentNode, w.id.op);
                addValue(currentNode, w.id.push);
                stack.push_back({ current, *slotItemPtr });
                first       = true;
                currentPtr  = &value;
                slotItemPtr = &value.slotList()[w.id.first];
                continue;
            }
        }

        slotItemPtr = slotItem.has(w.id.next) ? &slotItem[w.id.next] : nullptr;
        while (!slotItemPtr && !stack.empty()) {
            currentPtr  = &stack.back().ast;
            slotItemPtr = &stack.back().slotItem;
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
    currentNode->m_data   = processToolAst(tool, memberIds, compiledToolType);
    currentNode->m_tool   = &tool;
    currentNode->m_effect = &effect;

    if (IS_LOG_ENABLED) {
        CellI& astAsList = serializeEffectAst(effect);
        std::stringstream ss;
        forEach(astAsList, [&ss](CellI& value, int, bool& stop) {
            ss << value.label() << " ";
        });
        TRACE(toolFinder, "  {}", ss.str());
    }
}

// ============================================================================
CellI* ToolFinder::processToolAst(CellI& toolAst, Map& memberIds, CellI& compiledToolType)
{
    CellI& membersList = toolAst[w.id.members][w.id.list];
    CellI* slotItemPtr = membersList.has(w.id.first) ? &membersList[w.id.first] : nullptr;
    bool first         = true;
    List& builder      = *new List(w, w.std.Cell, fmt::format("builder for {}", toolAst.label()));

    while (slotItemPtr) {
        CellI& slotItem = *slotItemPtr;
        CellI& slot     = slotItem[w.id.value];
        CellI& key      = slot[w.id.key];

        if (first) {
            first = false;
            builder.add(w.ast.cell(w.id.__type__));
            builder.add(w.ast.cell(compiledToolType));
            continue;
        }
        builder.add(w.ast.cell(key));
        builder.add(memberIds.getValue(key));

        slotItemPtr = slotItem.has(w.id.next) ? &slotItem[w.id.next] : nullptr;
    }

    return &builder;
}

// ============================================================================
void ToolFinder::handleStep(CellI*& effectAstPtr, CellI*& slotItemPtr, Node*& node, std::stack<StackNode>& stack)
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

        slotItemPtr  = &stack.top().slotItem;
        effectAstPtr = &stack.top().ast;
        stack.pop();
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
    std::stack<StackNode>& stack = findContext.stack;

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
                    node = nextNode;
                    handleStep(effectAstPtr, slotItemPtr, node, stack);
                    findContext.toolKind          = ToolKind::Expression;
                    findContext.expressionToolPtr = &(*effectAstPtr)[key];
                    DEBUG(toolFinderLookup, "unify return with {}", (*effectAstPtr)[key].printAsValue());
                    return true;
                }
                if (opKey == &w.id.push && (&value.__type__() != &w.std.ast.Cell)) {
                    TRACE(toolFinderLookup, "MATCH: push");
                    stack.push({ .ast = *effectAstPtr, .slotItem = *slotItemPtr });
                    effectAstPtr = &(*effectAstPtr)[key];
                    slotItemPtr  = &value.slotList()[w.id.first];
                    node         = nextNode;
                    slotKind     = SlotKind::StructSlot;
                    return true;
                }
                if (opKey == &w.id.pop) {
                    TRACE(toolFinderLookup, "MATCH: pop");
                    if (stack.empty()) {
                        return false;
                    }
                    slotItemPtr  = &stack.top().slotItem;
                    effectAstPtr = &stack.top().ast;
                    stack.pop();
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
    if (slotKind == SlotKind::StructSlot) {
        slotKind = SlotKind::NormalSlot;
        return true;
    }
    handleStep(effectAstPtr, slotItemPtr, node, stack);

    return true;
}

// ============================================================================
List& ToolFinder::findToolsByEffectAst(CellI& effectAst)
{
    List& ret      = *new List(w, w.std.ast.Base);
    CellI* toolAst = nullptr;
    CellI* tool    = findToolByEffectAstImpl(effectAst, toolAst);
    if (!tool) {
        return ret;
    }
    Object retVal(w, w.std.ast.Cell);
    createTool(retVal, w.id.value, *toolAst, *tool);
    DEBUG(toolFinderLookup, "result: {}", retVal[w.id.value].printAsValue());

    ret.add(retVal[w.id.value]);

    return ret;
}

// ============================================================================
CellI* ToolFinder::findToolByEffectAstImpl(CellI& inputEffectAst, CellI*& outputEffectAst)
{
    DEBUG(toolFinderLookup, "input: {}", inputEffectAst.printAsValue());
    CellI& slotList         = inputEffectAst.slotList();
    FindContext findContext = {
        .trieNode     = m_root.get(),
        .slotList     = &slotList,
        .slotItemPtr  = slotList.has(w.id.first) ? &slotList[w.id.first] : nullptr,
        .slotKind     = SlotKind::StructSlot,
        .effectAstPtr = &inputEffectAst,
    };

    do {
        findContext.toolKind = ToolKind::Statement;
        while (findContext.slotItemPtr) {
            if (findContext.slotKind == SlotKind::StructSlot) {
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
            CellI& newEffectAst = *new Object(w, w.std.ast.Equal); // TODO FIX memory leak

            newEffectAst.set(w.id.lhs, *findContext.expressionToolPtr);
            createTool(newEffectAst, w.id.rhs, (*findContext.effectAstPtr), *findContext.trieNode->m_data);
            DEBUG(toolFinderLookup, "pattern match for {} with tool {}", findContext.trieNode->m_effect->printAsValue(), printTool(*findContext.trieNode->m_tool));
            DEBUG(toolFinderLookup, "created tool: {}", newEffectAst.printAsValue());

            CellI& newSlotList = newEffectAst.slotList();

            findContext.trieNode     = m_root.get();
            findContext.slotList     = &newSlotList;
            findContext.slotItemPtr  = newSlotList.has(w.id.first) ? &newSlotList[w.id.first] : nullptr;
            findContext.slotKind     = SlotKind::StructSlot;
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
void ToolFinder::createTool(CellI& outCell, CellI& outKey, CellI& inputAst, CellI& inputToolDesc)
{
    auto& ListOfCellStruct = w.getStruct(w.templateId("std::List", w.id.valueType, w.std.Cell));

    World& w                = this->w;
    List& toCreate          = *new List(w, w.std.Cell);
    Index& toCreateItemRoot = *new Index(w);
    toCreateItemRoot.set(w.id.ast, inputAst);
    toCreateItemRoot.set(w.id.description, inputToolDesc);
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

            CellI* toolAst     = nullptr;
            CellI* subToolDesc = findToolByEffectAstImpl(subToolAst, toolAst);

            if (!subToolDesc) {
                throw "Sub tool not found!";
            }
            Index& toCreateItemSub = *new Index(w);
            toCreateItemSub.set(w.id.ast, subToolAst);
            toCreateItemSub.set(w.id.description, *subToolDesc);
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
    CellI& tool = *new Object(w, *blueprint.m_compiledToolType);
    tool.set(*blueprint.m_slotId, w.ast.cell(from));

    Object unknownX(w, w.std.op.ConstVar, "unknownX");

    CellI* missingSlotIdPtr = findMissingSlotId(tool, *blueprint.m_slotId);
    if (!missingSlotIdPtr) {
        return; // TODO
    }
    CellI& missingSlotId = *missingSlotIdPtr;

    // this is the from in "tool(from, x) == to"
    tool.set(missingSlotId, w.ast.get(w._(unknownX), w._(w.id.value)));

    CellI& missingSlotEquation = *new Object(w, w.std.ast.Equal, "tool(from, x) == to");
    missingSlotEquation.set(w.id.lhs, tool);
    missingSlotEquation.set(w.id.rhs, w.ast.cell(to));

    List& missingSlotSolvers = findToolsByEffectAst(missingSlotEquation);

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
        CellI& conversionToolAst       = *new Object(w, *blueprint.m_compiledToolType);
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

void ToolFinder::findConversionToolsByContainer(CellI& from, CellI& to, List& results)
{

}

} // namespace cells
} // namespace infocell

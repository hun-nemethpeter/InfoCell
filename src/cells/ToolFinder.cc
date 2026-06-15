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
    CellI& slotList    = effectAst.struct_()[w.ids.slots][w.ids.list];
    CellI* slotItemPtr = slotList.has(w.ids.first) ? &slotList[w.ids.first] : nullptr;
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
        CellI& slot     = slotItem[w.ids.value];
        CellI& key     = slot[w.ids.key];
        CellI& current  = *currentPtr;

        if (first) {
            first = false;
            ret.add(w.ids.struct_);
            ret.add(current.struct_());
        }

        if (current.has(key)) {
            CellI& value = current[key];
            ret.add(key);
            if (&key == &w.ids.struct_) {
                ret.add(value);
                if (&key == &w.ids.op) {
                    ret.add(value);
                }
            } else if (&value.struct_() == &w.std.ast.Cell) {
                ret.add(value[w.ids.value]);
                if (&value[w.ids.value] == &w.ids.op) {
                    ret.add(value[w.ids.value]);
                }
            } else if (&value.struct_() == &w.std.ast.Member || &value.struct_() == &w.std.ast.Return) {
                ret.add(w.ids.op);
                if (&value.struct_() == &w.std.ast.Member) {
                    ret.add(w.ids.variable);
                } else {
                    ret.add(w.ids.return_);
                }
            } else if ((&key != &w.ids.struct_) && value.struct_()[w.ids.memberOf][w.ids.index].has(w.std.ast.Base)) {
                ret.add(w.ids.op);
                ret.add(w.ids.push);
                stack.push({ current, *slotItemPtr });
                first       = true;
                currentPtr  = &value;
                slotItemPtr = &value.struct_()[w.ids.slots][w.ids.list][w.ids.first];
                continue;
            }
        }

        slotItemPtr = slotItem.has(w.ids.next) ? &slotItem[w.ids.next] : nullptr;
        if (!slotItemPtr && !stack.empty()) {
            slotItemPtr = &stack.top().slotItem;
            currentPtr  = &stack.top().ast;
            stack.pop();
            ret.add(w.ids.op);
            ret.add(w.ids.pop);
            slotItemPtr = (*slotItemPtr).has(w.ids.next) ? &(*slotItemPtr)[w.ids.next] : nullptr;
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
    auto& effects = tool[w.ids.description][w.ids.asts];
    if (tool.has(w.ids.returnType)) {
        // so this can be a conversion tool
        CellI& returnType = tool[w.ids.returnType][w.ids.value];
        CellI& toolInputSlots = compiledToolType[w.ids.slots][w.ids.list];
        Visitor::visitList(toolInputSlots, [this, &tool, &compiledToolType, &returnType](CellI& slot, int i, bool& stop) {
            CellI& inputType = slot[w.ids.type];
            ConversionToolKey key(inputType, returnType);
            ConversionToolBlueprint blueprint(tool, compiledToolType, slot[w.ids.key]);
            m_conversionTools.insert({ key, blueprint });
        });
    }

    if (IS_LOG_ENABLED) {
        std::stringstream ss;

        ss << "" << tool.label() << "(";

        CellI& toolInputSlots = compiledToolType[w.ids.slots][w.ids.list];
        Visitor::visitList(toolInputSlots, [this, &ss, &tool](CellI& slot, int i, bool& stop) {
            if (i > 0) {
                ss << ", ";
            }
            CellI& inputType = slot[w.ids.type];
            ss << slot[w.ids.key].label() << ": " << inputType.label();
        });
        ss << ")";
        if (tool.has(w.ids.returnType)) {
            CellI& returnType = tool[w.ids.returnType][w.ids.value];
            ss << ": " << returnType.label();
        }
        TRACE(toolFinder, "{} =>", ss.str());

    }
    Visitor::visitList(effects, [this, &tool, &compiledToolType](CellI& effect, int i, bool& stop) {
        add(effect, tool, compiledToolType);
    });
}

// ============================================================================
void ToolFinder::add(CellI& effect, CellI& tool, CellI& compiledToolType)
{
    struct Context
    {
        CellI& ast;
        CellI& slotItem;
    };
    std::deque<Context> stack;

    Node* currentNode  = m_root.get();
    CellI& slotList    = effect.struct_()[w.ids.slots][w.ids.list];
    CellI* slotItemPtr = slotList.has(w.ids.first) ? &slotList[w.ids.first] : nullptr;
    bool first         = true;
    CellI* currentPtr  = &effect;
    Map memberIds(w, w.std.Cell, w.std.Cell);

    while (slotItemPtr) {
        CellI& slotItem = *slotItemPtr;
        CellI& slot     = slotItem[w.ids.value];
        CellI& key     = slot[w.ids.key];
        CellI& current  = *currentPtr;

        if (first) {
            first = false;
            addValue(currentNode, w.ids.struct_);
            addValue(currentNode, current.struct_());
        }

        if (current.has(key)) {
            CellI& value = current[key];
            addValue(currentNode, key);
            if (&value.struct_() == &w.std.ast.Cell) {
                addValue(currentNode, value[w.ids.value]);
                if (&value[w.ids.value] == &w.ids.op) {
                    addValue(currentNode, value[w.ids.value]);
                }
            } else if (&value.struct_() == &w.std.ast.Member || &value.struct_() == &w.std.ast.Return) {
                addValue(currentNode, w.ids.op);
                CellI* memberKeyPtr = nullptr;
                if (&value.struct_() == &w.std.ast.Member) {
                    addValue(currentNode, w.ids.variable);
                    memberKeyPtr = &value[w.ids.key];
                } else {
                    addValue(currentNode, w.ids.return_);
                    // TODO w.ids.return_ can not be a member name
                    memberKeyPtr = &w.ids.return_;
                }
                CellI& memberKey = *memberKeyPtr;
                if (!memberIds.hasKey(memberKey)) {
                    List& path = *new List(w, w.std.Cell, fmt::format("path for {}", memberKey.label()));
                    for (auto& stackItem : stack) {
                        if (&stackItem.ast.struct_() == &w.std.ast.Return) {
                            continue;
                        }
                        auto& key = stackItem.slotItem[w.ids.value][w.ids.key];
                        path.add(key);
                    }
                    path.add(slotItem[w.ids.value][w.ids.key]);
                    memberIds.add(memberKey, path);
                }
            } else if ((&key != &w.ids.struct_) && value.struct_()[w.ids.memberOf][w.ids.index].has(w.std.ast.Base)) {
                addValue(currentNode, w.ids.op);
                addValue(currentNode, w.ids.push);
                stack.push_back({ current, *slotItemPtr });
                first       = true;
                currentPtr  = &value;
                slotItemPtr = &value.struct_()[w.ids.slots][w.ids.list][w.ids.first];
                continue;
            }
        }

        slotItemPtr = slotItem.has(w.ids.next) ? &slotItem[w.ids.next] : nullptr;
        if (!slotItemPtr && !stack.empty()) {
            currentPtr  = &stack.back().ast;
            slotItemPtr = &stack.back().slotItem;
            stack.pop_back();
            addValue(currentNode, w.ids.op);
            addValue(currentNode, w.ids.pop);
            slotItemPtr = (*slotItemPtr).has(w.ids.next) ? &(*slotItemPtr)[w.ids.next] : nullptr;
        }
    }

    currentNode->m_isLeaf = 1;
    currentNode->m_data   = processToolAst(tool, memberIds, compiledToolType);

    if (IS_LOG_ENABLED) {
        CellI& astAsList = serializeEffectAst(effect);
        std::stringstream ss;
        Visitor::visitList(astAsList, [&ss](CellI& value, int, bool& stop) {
            ss << value.label() << " ";
        });
        TRACE(toolFinder, "  {}", ss.str());
    }
}

// ============================================================================
CellI* ToolFinder::processToolAst(CellI& toolAst, Map& memberIds, CellI& compiledToolType)
{
    CellI& membersList = toolAst[w.ids.members][w.ids.list];
    CellI* slotItemPtr = membersList.has(w.ids.first) ? &membersList[w.ids.first] : nullptr;
    bool first         = true;
    List& builder      = *new List(w, w.std.Cell, fmt::format("builder for {}", toolAst.label()));

    while (slotItemPtr) {
        CellI& slotItem = *slotItemPtr;
        CellI& slot     = slotItem[w.ids.value];
        CellI& key     = slot[w.ids.key];

        if (first) {
            first = false;
            builder.add(w.ast.cell(w.ids.struct_));
            builder.add(w.ast.cell(compiledToolType));
            continue;
        }
        builder.add(w.ast.cell(key));
        builder.add(memberIds.getValue(key));

        slotItemPtr = slotItem.has(w.ids.next) ? &slotItem[w.ids.next] : nullptr;
    }

    return &builder;
}

// ============================================================================
void ToolFinder::handleStep(CellI*& effectAstPtr, CellI*& slotItemPtr, Node*& node, std::stack<StackNode>& stack)
{
    slotItemPtr = (*slotItemPtr).has(w.ids.next) ? &(*slotItemPtr)[w.ids.next] : nullptr;
    while (!slotItemPtr) {
        if (stack.empty()) {
            return;
        }

        auto opFindIt = node->m_children.find(&w.ids.op);
        if (opFindIt == node->m_children.end()) {
            return;
        }
        Node* opNode   = opFindIt->second;
        auto popFindIt = opNode->m_children.find(&w.ids.pop);
        if (popFindIt == opNode->m_children.end()) {
            return;
        }
        node = popFindIt->second;

        slotItemPtr  = &stack.top().slotItem;
        effectAstPtr = &stack.top().effectAst;
        stack.pop();
        slotItemPtr = (*slotItemPtr).has(w.ids.next) ? &(*slotItemPtr)[w.ids.next] : nullptr;
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
        node = keyFindIt->second;
    }

    auto findIt = node->m_children.find(&value);
    if (findIt == node->m_children.end()) {
        auto opFindIt = node->m_children.find(&w.ids.op);
        if (opFindIt == node->m_children.end()) {
            node = nullptr;
            return false;
        } else {
            // ok, so value not found but we have an op here
            Node* opNode = opFindIt->second;

            for (auto& [opKey, nextNode] : opNode->m_children) {
                if (opKey == &w.ids.variable) {
                    node = nextNode;
                    handleStep(effectAstPtr, slotItemPtr, node, stack);
                    return true;
                }
                if (opKey == &w.ids.return_) {
                    if (findContext.toolKind == ToolKind::Expression) {
                        // TODO What to do if there are two "op return" in the effect description?
                        throw "Not implemented! Handling more then one op return is missing";
                    }
                    node = nextNode;
                    handleStep(effectAstPtr, slotItemPtr, node, stack);
                    findContext.toolKind          = ToolKind::Expression;
                    findContext.expressionToolPtr = &(*effectAstPtr)[key];
                    return true;
                }
                if (opKey == &w.ids.push) {
                    stack.push({ .effectAst = *effectAstPtr, .slotItem = *slotItemPtr });
                    effectAstPtr = &(*effectAstPtr)[key];
                    slotItemPtr  = &value.struct_()[w.ids.slots][w.ids.list][w.ids.first];
                    node         = nextNode;
                    slotKind     = SlotKind::StructSlot;
                    return true;
                }
                if (opKey == &w.ids.pop) {
                    if (stack.empty()) {
                        return false;
                    }
                    slotItemPtr  = &stack.top().slotItem;
                    effectAstPtr = &stack.top().effectAst;
                    stack.pop();
                    slotItemPtr = (*slotItemPtr).has(w.ids.next) ? &(*slotItemPtr)[w.ids.next] : nullptr;
                    node        = nextNode;
                    if (!slotItemPtr) {
                        handleStep(effectAstPtr, slotItemPtr, node, stack);
                    }
                    return true;
                }
            }
        }
    } else {
        node = findIt->second;
    }
    // the first slot is the w.ids.struct_ but it is not in the slot list
    if (slotKind == SlotKind::StructSlot) {
        slotKind = SlotKind::NormalSlot;
        return true;
    }
    handleStep(effectAstPtr, slotItemPtr, node, stack);

    return true;
}

// ============================================================================
CellI* ToolFinder::findToolByEffectAst(CellI& effectAst)
{
    CellI* toolAst = nullptr;
    CellI* tool    = findToolByEffectAstImpl(effectAst, toolAst);
    if (!tool) {
        return nullptr;
    }
    Object retVal(w, w.std.ast.Cell);
    createTool(retVal, w.ids.value, *toolAst, *tool);
    return &retVal[w.ids.value];
}

// ============================================================================
CellI* ToolFinder::findToolByEffectAstImpl(CellI& inputEffectAst, CellI*& outputEffectAst)
{
    CellI& slotList         = inputEffectAst.struct_()[w.ids.slots][w.ids.list];
    FindContext findContext = {
        .trieNode     = m_root.get(),
        .slotList     = &slotList,
        .slotItemPtr  = slotList.has(w.ids.first) ? &slotList[w.ids.first] : nullptr,
        .slotKind     = SlotKind::StructSlot,
        .effectAstPtr = &inputEffectAst,
    };

    do {
        findContext.toolKind = ToolKind::Statement;
        while (findContext.slotItemPtr)
        {
            CellI& key = (*findContext.slotItemPtr)[w.ids.value][w.ids.key];
            if (findContext.slotKind == SlotKind::StructSlot && !checkValue(findContext, w.ids.struct_, (*findContext.effectAstPtr).struct_())) {
                return nullptr;
            }

            if ((*findContext.effectAstPtr).has(key) && !checkValue(findContext, key, (*findContext.effectAstPtr)[key])) {
                return nullptr;
            }
        }
        if (findContext.toolKind == ToolKind::Expression) {
            if (!(findContext.trieNode && findContext.trieNode->m_isLeaf)) {
                return nullptr;
            }
            CellI& newEffectAst = *new Object(w, w.std.ast.Equal); // TODO FIX memory leak

            newEffectAst.set(w.ids.lhs, *findContext.expressionToolPtr);
            createTool(newEffectAst, w.ids.rhs, inputEffectAst, *findContext.trieNode->m_data);

            CellI& newSlotList = newEffectAst.struct_()[w.ids.slots][w.ids.list];

            findContext.trieNode     = m_root.get();
            findContext.slotList     = &newSlotList;
            findContext.slotItemPtr  = newSlotList.has(w.ids.first) ? &newSlotList[w.ids.first] : nullptr;
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
    auto& ListOfCellStruct = w.getStruct(w.templateId("std::List", w.ids.valueType, w.std.Cell));

    World& w        = this->w;
    List& toCreate          = *new List(w, w.std.Cell);
    Index& toCreateItemRoot = *new Index(w);
    toCreateItemRoot.set(w.ids.ast, inputAst);
    toCreateItemRoot.set(w.ids.description, inputToolDesc);
    toCreateItemRoot.set(w.ids.cell, outCell);
    toCreateItemRoot.set(w.ids.key, outKey);

    toCreate.add(toCreateItemRoot);
    CellI* toCreateItemPtr = &toCreate[w.ids.first];
    while (toCreateItemPtr) {
        CellI& toCreateItem = (*toCreateItemPtr)[w.ids.value];
        CellI& ast          = toCreateItem[w.ids.ast];
        CellI& toolDesc     = toCreateItem[w.ids.description];
        CellI* ret          = &toCreateItem[w.ids.cell];
        CellI& retKey       = toCreateItem[w.ids.key];

        CellI* slotItemPtr = &toolDesc[w.ids.first];
        bool first         = true;
        List& subTools     = *new List(w, w.std.Cell);
        while (slotItemPtr) {
            CellI& key = (*slotItemPtr)[w.ids.value];

            if (first) {
                if (&key.struct_() != &w.std.ast.Cell && (&key[w.ids.value] != &w.ids.struct_)) {
                    throw "Tool description without type!";
                }
                first               = false;
                CellI& nextSlotItem = (*slotItemPtr)[w.ids.next];
                CellI& valueCell    = nextSlotItem[w.ids.value];
                if (&valueCell.struct_() != &w.std.ast.Cell) {
                    throw "Tool description type is not constant value!";
                }
                CellI& type   = valueCell[w.ids.value];
                CellI* newObj = new Object(w, type, fmt::format("built from {}", toolDesc.label()));
                ret->set(retKey, *newObj);
                ret = newObj;

                slotItemPtr = &nextSlotItem;
            } else if (&key.struct_() == &w.std.ast.Cell) {
                CellI& unwrappedKey         = key[w.ids.value];
                CellI& nextSlotItem = (*slotItemPtr)[w.ids.next];
                CellI& valueCell    = nextSlotItem[w.ids.value];
                CellI* valuePtr     = nullptr;
                if (&valueCell.struct_() == &w.std.ast.Cell) {
                    valuePtr = &ast[unwrappedKey];
                    ret->set(unwrappedKey, *valuePtr);
                } else if (&valueCell.struct_() == &ListOfCellStruct) {
                    valuePtr = &ast;
                    Visitor::visitList(valueCell, [&valuePtr, &w](CellI& pathItem, int, bool& stop) {
                        CellI& currentValue = *valuePtr;
                        valuePtr            = &currentValue[pathItem];
                    });
                    ret->set(unwrappedKey, *valuePtr);
                } else {
                    throw "Tool description value is not a constant value or List!";
                }
                if (&(*valuePtr).struct_() != &w.std.ast.Cell) {
                    CellI& retVal = *new Object(w, w.std.ast.Return); // TODO FIX memory leak
                    retVal.set(w.ids.value, *valuePtr);
                    subTools.add(w.ast.slot(*ret, unwrappedKey));
                }
                slotItemPtr = &nextSlotItem;
            } else {
                throw "Tool description key is not constant value!";
            }

            slotItemPtr = (*slotItemPtr).has(w.ids.next) ? &(*slotItemPtr)[w.ids.next] : nullptr;
        }
        CellI* subpToolItemPtr = &subTools[w.ids.first];
        CellI& retVal          = *new Object(w, w.std.ast.Return); // TODO FIX memory leak
        while (subpToolItemPtr) {
            CellI& slot       = (*subpToolItemPtr)[w.ids.value];
            CellI& key        = slot[w.ids.key];
            CellI& value      = slot[w.ids.type];
            CellI& subToolAst = key[value];

            retVal.set(w.ids.value, subToolAst);
            CellI* toolAst     = nullptr;
            CellI* subToolDesc = findToolByEffectAstImpl(retVal, toolAst);

            if (!subToolDesc) {
                throw "Sub tool not found!";
            }
            Index& toCreateItemSub = *new Index(w);
            toCreateItemSub.set(w.ids.ast, subToolAst);
            toCreateItemSub.set(w.ids.description, *subToolDesc);
            toCreateItemSub.set(w.ids.cell, (*ret));
            toCreateItemSub.set(w.ids.key, value);
            toCreate.add(toCreateItemSub);

            CellI* toDelete = subpToolItemPtr;
            subpToolItemPtr = (*subpToolItemPtr).has(w.ids.next) ? &(*subpToolItemPtr)[w.ids.next] : nullptr;
            subTools.remove((List::Item*)toDelete);
        }
        CellI* toDelete = toCreateItemPtr;
        toCreateItemPtr = (*toCreateItemPtr).has(w.ids.next) ? &(*toCreateItemPtr)[w.ids.next] : nullptr;
        toCreate.remove((List::Item*)toDelete);
    }
}

// ============================================================================
void ToolFinder::print()
{
    if (empty())
        return;
    printCb(m_root.get());
}

// ============================================================================
void ToolFinder::printCb(Node* node)
{
    printf("%s -> ", node->m_data->label().c_str());

    for (auto& it : node->m_children) {
        printCb(it.second);
    }
}

static void fillMissingSlotsWithUnknown(CellI& tool, CellI& filledSlot, CellI& unknownX, bool forConversion = false)
{
    World& w = tool.w;
    CellI& slotList  = tool.struct_()[w.ids.slots][w.ids.list];
    Visitor::visitList(slotList, [&w, &tool, &filledSlot, &unknownX, forConversion](CellI& slot, int i, bool& stop) {
        CellI& slotType = slot[w.ids.type];
        CellI& slotKey = slot[w.ids.key];
        if (&slotKey != &filledSlot) {
            if (forConversion) {
                tool.set(slotKey, w._(unknownX));
            } else {
                tool.set(slotKey, w.ast.get(w._(unknownX), w._(w.ids.value)));
            }
        }
    });
}

class SolverLib : public Library
{
public:
    SolverLib(World& w, Ast::Scope& parentScope, CellI& solverAst, const std::string& conversionToolName, CellI& conversionToolAst, CellI& conversionToolReturnType);
};
class SolverLibAst : public AstHelper
{
public:
    SolverLibAst(World& w, Ast::Scope& scope, CellI& solverAst, const std::string& conversionToolName, CellI& conversionToolAst, CellI& conversionToolReturnType);
};

SolverLibAst::SolverLibAst(World& w, Ast::Scope& parentScope, CellI& solverAst, const std::string& conversionToolName, CellI& conversionToolAst, CellI& conversionToolReturnType) :
    AstHelper(w)
{
    auto& solverFunction = parentScope.add<Function>("solverFunction");
    solverFunction.instructions(solverAst);
    auto& conversionTool = parentScope.add<Function>(conversionToolName);
    conversionTool.returnType(_(conversionToolReturnType));
    conversionTool.instructions(
        return_(conversionToolAst));
}

SolverLib::SolverLib(World& w, Ast::Scope& parentScope, CellI& solverAst, const std::string& conversionToolName, CellI& conversionToolAst, CellI& conversionToolReturnType) :
    Library(w, parentScope)
{
    SolverLibAst solverLibAst(w, parentScope.add<Ast::Scope>("solver"), solverAst, conversionToolName, conversionToolAst, conversionToolReturnType);
}


static void PrintAsValue(CellI& cell, const std::string& label)
{
    CellValuePrinter valuePrinter;
    cell.accept(valuePrinter);

    if (!label.empty()) {
        std::cout << label << ": ";
    }

    std::cout << valuePrinter.print() << std::endl;
}

CellI& ToolFinder::createConversionToolFromBlueprint(CellI& from, CellI& to, ToolFinder::ConversionToolBlueprint& blueprint)
{
    World& w  = blueprint.m_tool->w;
    CellI& tool = *new Object(w, *blueprint.m_compiledToolType);
    tool.set(*blueprint.m_slotId, w.ast.cell(from));

    Object unknownX(w, w.std.op.ConstVar, "unknownX");
    fillMissingSlotsWithUnknown(tool, *blueprint.m_slotId, unknownX);

    CellI& missingSlotEquation = *new Object(w, w.std.ast.Equal, "tool(from, x) == to");
    missingSlotEquation.set(w.ids.lhs, tool);
    missingSlotEquation.set(w.ids.rhs, w.ast.cell(to));

    if (CellI* missingSlotSolver = findToolByEffectAst(missingSlotEquation)) {
        CellI& conversionToolAst = *new Object(w, *blueprint.m_compiledToolType);
        conversionToolAst.set(*blueprint.m_slotId, w.ast.var(fmt::format("input_{}", blueprint.m_slotId->label())));
        fillMissingSlotsWithUnknown(conversionToolAst, *blueprint.m_slotId, unknownX);

        Ast::Scope rootScope(w, "toolFinder");
        Compiler compiler(w);
        std::string conversionToolName = fmt::format("conversionToolFor{}", blueprint.m_tool->label());
        SolverLib solverLib(w, rootScope, *missingSlotSolver, conversionToolName, conversionToolAst, to.struct_());
        solverLib.include(w.arcLib());
        compiler.compile(solverLib);
        auto& solverFn = solverLib.getFunction("solver::solverFunction");
        auto& conversionToolFn = solverLib.getFunction(fmt::format("solver::{}", conversionToolName));
//        PrintAsValue(solverFn, "solverFn");
        solverFn.createSelfStack();
        solverFn();
//        std::cout << blueprint << '\n';
//        std::cout << "unknownX.value = " << unknownX[w.ids.value].label() << std::endl;

        Ast::Scope rootScope2(w, "toolFinder");
        Compiler compiler2(w);
        CellI& conversionToolAst2 = *new Object(w, *blueprint.m_compiledToolType);
        conversionToolAst2.set(*blueprint.m_slotId, w.ast.var(fmt::format("input_{}", blueprint.m_slotId->label())));
        fillMissingSlotsWithUnknown(conversionToolAst2, *blueprint.m_slotId, unknownX.get(w.ids.value), true);
        SolverLib solverLib2(w, rootScope2, *missingSlotSolver, conversionToolName, conversionToolAst2, to.struct_());
        solverLib2.include(w.arcLib());
        compiler2.compile(solverLib2);
        auto& conversionToolFn2 = solverLib2.getFunction(fmt::format("solver::{}", conversionToolName));
        //        PrintAsValue(solverFn, "solverFn");
        //        std::cout << blueprint << '\n';

        PrintAsValue(conversionToolFn2, "");
        std::cout << "" << std::endl;
    }

    return w.ids.emptyObject; // TODO
}

// ============================================================================
CellI& ToolFinder::findConversionTools(CellI& from, CellI& to)
{
    CellI& inputType = from.struct_();
    CellI& outputType = to.struct_();

    ConversionToolKey conversionToolKey(inputType, outputType);

    auto tools = m_conversionTools.equal_range(conversionToolKey);
    if (tools.first != m_conversionTools.end()) {
        std::cout << conversionToolKey << '\n';
    }
    for (auto it = tools.first; it != tools.second; ++it) {
        ConversionToolBlueprint blueprint = it->second;
//        std::cout << "  " << blueprint << '\n';
        // ConversionToolKey [from: Number, to: Number]: ConversionToolBlueprint [tool: Add, input: lhs]
        CellI& conversionTool = createConversionToolFromBlueprint(from, to, blueprint);
    }
    std::cout << "";
#if 0
Add(lhs: Number, rhs: Number): Number
And(lhs: Boolean, rhs: Boolean): Boolean
Divide(lhs: Number, rhs: Number): Number
Equal(lhs: ast::Base, rhs: ast::Base): Boolean
Get(cell: ast::Base, key: ast::Base): ast::Base
GreaterThan(lhs: Number, rhs: Number): Boolean
GreaterThanOrEqual(lhs: Number, rhs: Number): Boolean
Has(cell: ast::Base, key: ast::Base): Boolean
LessThan(lhs: Number, rhs: Number): Boolean
LessThanOrEqual(lhs: Number, rhs: Number): Boolean
Missing(cell: ast::Base, key: ast::Base): Boolean
Multiply(lhs: Number, rhs: Number): Number
Not(input: Boolean): Boolean
NotEqual(lhs: ast::Base, rhs: ast::Base): Boolean
NotSame(lhs: ast::Base, rhs: ast::Base): Boolean
Or(lhs: Boolean, rhs: Boolean): Boolean
Same(lhs: ast::Base, rhs: ast::Base): Boolean
Subtract(lhs: Number, rhs: Number): Number

2 -> 4
Number -> Number

Add(lhs: Number, rhs: Number): Number           // commutative so only Add(x, 2) == 4 is considered
Divide(lhs: Number, rhs: Number): Number        // Divide(x, 2) == 4 and Divide(2, x) == 4
Get(cell: ast::Base, key: ast::Base): ast::Base // Get(x, 2) == 4 and Get(2, x) == 4 are considered, but can not be solved with Set as that function is not a conversion tool
Multiply(lhs: Number, rhs: Number): Number      // commutative so only Multiply(x, 2) == 4 is considered
Subtract(lhs: Number, rhs: Number): Number      // Subtract(x, 2) == 4 and Divide(2, x) == 4


Solving equations:

Add(unknown1, 2) == 4

solving with
sub(lhs, rhs) solves [add(return(), rhs) == lhs] so [unknown1 == sub(4, 2)] so Add(Subtract(4, 2), lhs)



#endif
    return from.w.ids.emptyObject; // TODO
}

} // namespace cells
} // namespace infocell

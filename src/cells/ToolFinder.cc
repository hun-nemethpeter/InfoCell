#include "ToolFinder.h"

#include "Brain.h"

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
ToolFinder::ToolFinder(brain::Brain& kb) :
    kb(kb)
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
    CellI& slotList    = effectAst.struct_()[kb.ids.slots][kb.ids.list];
    CellI* slotItemPtr = slotList.has(kb.ids.first) ? &slotList[kb.ids.first] : nullptr;
    List& ret          = *new List(kb, kb.std.Cell);
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
        CellI& slot     = slotItem[kb.ids.value];
        CellI& key     = slot[kb.ids.key];
        CellI& current  = *currentPtr;

        if (first) {
            first = false;
            ret.add(kb.ids.struct_);
            ret.add(current.struct_());
        }

        if (current.has(key)) {
            CellI& value = current[key];
            ret.add(key);
            if (&key == &kb.ids.struct_) {
                ret.add(value);
                if (&key == &kb.ids.op) {
                    ret.add(value);
                }
            } else if (&value.struct_() == &kb.std.ast.Cell) {
                ret.add(value[kb.ids.value]);
                if (&value[kb.ids.value] == &kb.ids.op) {
                    ret.add(value[kb.ids.value]);
                }
            } else if (&value.struct_() == &kb.std.ast.Member || &value.struct_() == &kb.std.ast.Return) {
                ret.add(kb.ids.op);
                if (&value.struct_() == &kb.std.ast.Member) {
                    ret.add(kb.ids.variable);
                } else {
                    ret.add(kb.ids.return_);
                }
            } else if ((&key != &kb.ids.struct_) && value.struct_()[kb.ids.memberOf][kb.ids.index].has(kb.std.ast.Base)) {
                ret.add(kb.ids.op);
                ret.add(kb.ids.push);
                stack.push({ current, *slotItemPtr });
                first       = true;
                currentPtr  = &value;
                slotItemPtr = &value.struct_()[kb.ids.slots][kb.ids.list][kb.ids.first];
                continue;
            }
        }

        slotItemPtr = slotItem.has(kb.ids.next) ? &slotItem[kb.ids.next] : nullptr;
        if (!slotItemPtr && !stack.empty()) {
            slotItemPtr = &stack.top().slotItem;
            currentPtr  = &stack.top().ast;
            stack.pop();
            ret.add(kb.ids.op);
            ret.add(kb.ids.pop);
            slotItemPtr = (*slotItemPtr).has(kb.ids.next) ? &(*slotItemPtr)[kb.ids.next] : nullptr;
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
void ToolFinder::add(CellI& tool, CellI& compiledToolType)
{
    auto& effects = tool[kb.ids.description][kb.ids.asts];
    if (IS_LOG_ENABLED) {
        std::stringstream ss;

        ss << "" << tool.label() << "(";

        CellI& toolInputSlots = compiledToolType[kb.ids.slots][kb.ids.list];
        Visitor::visitList(toolInputSlots, [this, &ss, &tool](CellI& slot, int i, bool& stop) {
            if (i > 0) {
                ss << ", ";
            }
            ss << slot[kb.ids.key].label() << ": " << slot[kb.ids.type].label();
        });
        ss << ")";
        if (tool.has(kb.ids.returnType)) {
            ss << ": " << tool[kb.ids.returnType][kb.ids.value].label();
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
    CellI& slotList    = effect.struct_()[kb.ids.slots][kb.ids.list];
    CellI* slotItemPtr = slotList.has(kb.ids.first) ? &slotList[kb.ids.first] : nullptr;
    bool first         = true;
    CellI* currentPtr  = &effect;
    Map memberIds(kb, kb.std.Cell, kb.std.Cell);

    while (slotItemPtr) {
        CellI& slotItem = *slotItemPtr;
        CellI& slot     = slotItem[kb.ids.value];
        CellI& key     = slot[kb.ids.key];
        CellI& current  = *currentPtr;

        if (first) {
            first = false;
            addValue(currentNode, kb.ids.struct_);
            addValue(currentNode, current.struct_());
        }

        if (current.has(key)) {
            CellI& value = current[key];
            addValue(currentNode, key);
            if (&value.struct_() == &kb.std.ast.Cell) {
                addValue(currentNode, value[kb.ids.value]);
                if (&value[kb.ids.value] == &kb.ids.op) {
                    addValue(currentNode, value[kb.ids.value]);
                }
            } else if (&value.struct_() == &kb.std.ast.Member || &value.struct_() == &kb.std.ast.Return) {
                addValue(currentNode, kb.ids.op);
                CellI* memberKeyPtr = nullptr;
                if (&value.struct_() == &kb.std.ast.Member) {
                    addValue(currentNode, kb.ids.variable);
                    memberKeyPtr = &value[kb.ids.key];
                } else {
                    addValue(currentNode, kb.ids.return_);
                    // TODO kb.ids.return_ can not be a member name
                    memberKeyPtr = &kb.ids.return_;
                }
                CellI& memberKey = *memberKeyPtr;
                if (!memberIds.hasKey(memberKey)) {
                    List& path = *new List(kb, kb.std.Cell, fmt::format("path for {}", memberKey.label()));
                    for (auto& stackItem : stack) {
                        if (&stackItem.ast.struct_() == &kb.std.ast.Return) {
                            continue;
                        }
                        auto& key = stackItem.slotItem[kb.ids.value][kb.ids.key];
                        path.add(key);
                    }
                    path.add(slotItem[kb.ids.value][kb.ids.key]);
                    memberIds.add(memberKey, path);
                }
            } else if ((&key != &kb.ids.struct_) && value.struct_()[kb.ids.memberOf][kb.ids.index].has(kb.std.ast.Base)) {
                addValue(currentNode, kb.ids.op);
                addValue(currentNode, kb.ids.push);
                stack.push_back({ current, *slotItemPtr });
                first       = true;
                currentPtr  = &value;
                slotItemPtr = &value.struct_()[kb.ids.slots][kb.ids.list][kb.ids.first];
                continue;
            }
        }

        slotItemPtr = slotItem.has(kb.ids.next) ? &slotItem[kb.ids.next] : nullptr;
        if (!slotItemPtr && !stack.empty()) {
            currentPtr  = &stack.back().ast;
            slotItemPtr = &stack.back().slotItem;
            stack.pop_back();
            addValue(currentNode, kb.ids.op);
            addValue(currentNode, kb.ids.pop);
            slotItemPtr = (*slotItemPtr).has(kb.ids.next) ? &(*slotItemPtr)[kb.ids.next] : nullptr;
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
    CellI& membersList = toolAst[kb.ids.members][kb.ids.list];
    CellI* slotItemPtr = membersList.has(kb.ids.first) ? &membersList[kb.ids.first] : nullptr;
    bool first         = true;
    List& builder      = *new List(kb, kb.std.Cell, fmt::format("builder for {}", toolAst.label()));

    while (slotItemPtr) {
        CellI& slotItem = *slotItemPtr;
        CellI& slot     = slotItem[kb.ids.value];
        CellI& key     = slot[kb.ids.key];

        if (first) {
            first = false;
            builder.add(kb.ast.cell(kb.ids.struct_));
            builder.add(kb.ast.cell(compiledToolType));
            continue;
        }
        builder.add(kb.ast.cell(key));
        builder.add(memberIds.getValue(key));

        slotItemPtr = slotItem.has(kb.ids.next) ? &slotItem[kb.ids.next] : nullptr;
    }

    return &builder;
}

// ============================================================================
void ToolFinder::handleStep(CellI*& effectAstPtr, CellI*& slotItemPtr, Node*& node, std::stack<StackNode>& stack)
{
    slotItemPtr = (*slotItemPtr).has(kb.ids.next) ? &(*slotItemPtr)[kb.ids.next] : nullptr;
    while (!slotItemPtr) {
        if (stack.empty()) {
            return;
        }

        auto opFindIt = node->m_children.find(&kb.ids.op);
        if (opFindIt == node->m_children.end()) {
            return;
        }
        Node* opNode   = opFindIt->second;
        auto popFindIt = opNode->m_children.find(&kb.ids.pop);
        if (popFindIt == opNode->m_children.end()) {
            return;
        }
        node = popFindIt->second;

        slotItemPtr  = &stack.top().slotItem;
        effectAstPtr = &stack.top().effectAst;
        stack.pop();
        slotItemPtr = (*slotItemPtr).has(kb.ids.next) ? &(*slotItemPtr)[kb.ids.next] : nullptr;
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
        auto opFindIt = node->m_children.find(&kb.ids.op);
        if (opFindIt == node->m_children.end()) {
            node = nullptr;
            return false;
        } else {
            // ok, so value not found but we have an op here
            Node* opNode = opFindIt->second;

            for (auto& [opKey, nextNode] : opNode->m_children) {
                if (opKey == &kb.ids.variable) {
                    node = nextNode;
                    handleStep(effectAstPtr, slotItemPtr, node, stack);
                    return true;
                }
                if (opKey == &kb.ids.return_) {
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
                if (opKey == &kb.ids.push) {
                    stack.push({ .effectAst = *effectAstPtr, .slotItem = *slotItemPtr });
                    effectAstPtr = &(*effectAstPtr)[key];
                    slotItemPtr  = &value.struct_()[kb.ids.slots][kb.ids.list][kb.ids.first];
                    node         = nextNode;
                    slotKind     = SlotKind::StructSlot;
                    return true;
                }
                if (opKey == &kb.ids.pop) {
                    if (stack.empty()) {
                        return false;
                    }
                    slotItemPtr  = &stack.top().slotItem;
                    effectAstPtr = &stack.top().effectAst;
                    stack.pop();
                    slotItemPtr = (*slotItemPtr).has(kb.ids.next) ? &(*slotItemPtr)[kb.ids.next] : nullptr;
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
    // the first slot is the kb.ids.struct_ but it is not in the slot list
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
    Object retVal(kb, kb.std.ast.Cell);
    createTool(retVal, kb.ids.value, *toolAst, *tool);
    return &retVal[kb.ids.value];
}

// ============================================================================
CellI* ToolFinder::findToolByEffectAstImpl(CellI& inputEffectAst, CellI*& outputEffectAst)
{
    CellI& slotList         = inputEffectAst.struct_()[kb.ids.slots][kb.ids.list];
    FindContext findContext = {
        .trieNode     = m_root.get(),
        .slotList     = &slotList,
        .slotItemPtr  = slotList.has(kb.ids.first) ? &slotList[kb.ids.first] : nullptr,
        .slotKind     = SlotKind::StructSlot,
        .effectAstPtr = &inputEffectAst,
    };

    do {
        findContext.toolKind = ToolKind::Statement;
        while (findContext.slotItemPtr)
        {
            CellI& key = (*findContext.slotItemPtr)[kb.ids.value][kb.ids.key];
            if (findContext.slotKind == SlotKind::StructSlot && !checkValue(findContext, kb.ids.struct_, (*findContext.effectAstPtr).struct_())) {
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
            CellI& newEffectAst = *new Object(kb, kb.std.ast.Equal); // TODO FIX memory leak

            newEffectAst.set(kb.ids.lhs, *findContext.expressionToolPtr);
            createTool(newEffectAst, kb.ids.rhs, inputEffectAst, *findContext.trieNode->m_data);

            CellI& newSlotList = newEffectAst.struct_()[kb.ids.slots][kb.ids.list];

            findContext.trieNode     = m_root.get();
            findContext.slotList     = &newSlotList;
            findContext.slotItemPtr  = newSlotList.has(kb.ids.first) ? &newSlotList[kb.ids.first] : nullptr;
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
    auto& ListOfCellStruct = kb.getStruct(kb.templateId("std::List", kb.ids.valueType, kb.std.Cell));

    brain::Brain& kb        = this->kb;
    List& toCreate          = *new List(kb, kb.std.Cell);
    Index& toCreateItemRoot = *new Index(kb);
    toCreateItemRoot.set(kb.ids.ast, inputAst);
    toCreateItemRoot.set(kb.ids.description, inputToolDesc);
    toCreateItemRoot.set(kb.ids.cell, outCell);
    toCreateItemRoot.set(kb.ids.key, outKey);

    toCreate.add(toCreateItemRoot);
    CellI* toCreateItemPtr = &toCreate[kb.ids.first];
    while (toCreateItemPtr) {
        CellI& toCreateItem = (*toCreateItemPtr)[kb.ids.value];
        CellI& ast          = toCreateItem[kb.ids.ast];
        CellI& toolDesc     = toCreateItem[kb.ids.description];
        CellI* ret          = &toCreateItem[kb.ids.cell];
        CellI& retKey       = toCreateItem[kb.ids.key];

        CellI* slotItemPtr = &toolDesc[kb.ids.first];
        bool first         = true;
        List& subTools     = *new List(kb, kb.std.Cell);
        while (slotItemPtr) {
            CellI& key = (*slotItemPtr)[kb.ids.value];

            if (first) {
                if (&key.struct_() != &kb.std.ast.Cell && (&key[kb.ids.value] != &kb.ids.struct_)) {
                    throw "Tool description without type!";
                }
                first               = false;
                CellI& nextSlotItem = (*slotItemPtr)[kb.ids.next];
                CellI& valueCell    = nextSlotItem[kb.ids.value];
                if (&valueCell.struct_() != &kb.std.ast.Cell) {
                    throw "Tool description type is not constant value!";
                }
                CellI& type   = valueCell[kb.ids.value];
                CellI* newObj = new Object(kb, type, fmt::format("built from {}", toolDesc.label()));
                ret->set(retKey, *newObj);
                ret = newObj;

                slotItemPtr = &nextSlotItem;
            } else if (&key.struct_() == &kb.std.ast.Cell) {
                CellI& unwrappedKey         = key[kb.ids.value];
                CellI& nextSlotItem = (*slotItemPtr)[kb.ids.next];
                CellI& valueCell    = nextSlotItem[kb.ids.value];
                CellI* valuePtr     = nullptr;
                if (&valueCell.struct_() == &kb.std.ast.Cell) {
                    valuePtr = &ast[unwrappedKey];
                    ret->set(unwrappedKey, *valuePtr);
                } else if (&valueCell.struct_() == &ListOfCellStruct) {
                    valuePtr = &ast;
                    Visitor::visitList(valueCell, [&valuePtr, &kb](CellI& pathItem, int, bool& stop) {
                        CellI& currentValue = *valuePtr;
                        valuePtr            = &currentValue[pathItem];
                    });
                    ret->set(unwrappedKey, *valuePtr);
                } else {
                    throw "Tool description value is not a constant value or List!";
                }
                if (&(*valuePtr).struct_() != &kb.std.ast.Cell) {
                    CellI& retVal = *new Object(kb, kb.std.ast.Return); // TODO FIX memory leak
                    retVal.set(kb.ids.value, *valuePtr);
                    subTools.add(kb.ast.slot(*ret, unwrappedKey));
                }
                slotItemPtr = &nextSlotItem;
            } else {
                throw "Tool description key is not constant value!";
            }

            slotItemPtr = (*slotItemPtr).has(kb.ids.next) ? &(*slotItemPtr)[kb.ids.next] : nullptr;
        }
        CellI* subpToolItemPtr = &subTools[kb.ids.first];
        CellI& retVal          = *new Object(kb, kb.std.ast.Return); // TODO FIX memory leak
        while (subpToolItemPtr) {
            CellI& slot       = (*subpToolItemPtr)[kb.ids.value];
            CellI& key        = slot[kb.ids.key];
            CellI& value      = slot[kb.ids.type];
            CellI& subToolAst = key[value];

            retVal.set(kb.ids.value, subToolAst);
            CellI* toolAst     = nullptr;
            CellI* subToolDesc = findToolByEffectAstImpl(retVal, toolAst);

            if (!subToolDesc) {
                throw "Sub tool not found!";
            }
            Index& toCreateItemSub = *new Index(kb);
            toCreateItemSub.set(kb.ids.ast, subToolAst);
            toCreateItemSub.set(kb.ids.description, *subToolDesc);
            toCreateItemSub.set(kb.ids.cell, (*ret));
            toCreateItemSub.set(kb.ids.key, value);
            toCreate.add(toCreateItemSub);

            CellI* toDelete = subpToolItemPtr;
            subpToolItemPtr = (*subpToolItemPtr).has(kb.ids.next) ? &(*subpToolItemPtr)[kb.ids.next] : nullptr;
            subTools.remove((List::Item*)toDelete);
        }
        CellI* toDelete = toCreateItemPtr;
        toCreateItemPtr = (*toCreateItemPtr).has(kb.ids.next) ? &(*toCreateItemPtr)[kb.ids.next] : nullptr;
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

} // namespace cells
} // namespace infocell

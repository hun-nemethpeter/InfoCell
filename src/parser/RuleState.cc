#include <iostream>

#include <parser/Rule.h>
#include <parser/Parser.h>
#include <parser/HtmlHelper.h>

#include "RuleState.h"

namespace synth {

unsigned RuleState::s_id = 0;

// =====================================================================================================================
// =====================================================================================================================
// =====================================================================================================================
std::string RuleState::getName() const
{
    std::stringstream ret;
    if (getType() == RuleType::Term) {
        ret << m_rule->getName();
        return ret.str();
    }
    ret << m_rule->getName() << "_" << m_rule->getType() << m_id;

    return ret.str();
}

// =====================================================================================================================
RuleState::RuleState(const Rule* p_rule, Parser& p_parser, unsigned p_pos) :
    m_state(RuleStates::Init), m_rule(p_rule), m_parser(p_parser), m_pos(p_pos), m_length(0)
{
    m_id = s_id++;
#if 0
    if (m_id > 127)
        std::cout << "DDDD " << GetName() << " " << this << " construct" << std::endl;
#endif
}

// =====================================================================================================================
RuleState::~RuleState()
{
#if 0
    if (m_id > 127)
        std::cout << "DDDD " << GetName() << " " << this << " destruct" << std::endl;
#endif
}

// =====================================================================================================================
inline bool IsOptionalType(SlotType p_slotType)
{
    if (p_slotType == SlotType::Any || p_slotType == SlotType::Optional)
        return true;
    else
        return false;
}

// =====================================================================================================================
// =====================================================================================================================
// =====================================================================================================================
void SlotState::processTermEvent(RuleStates p_termState, unsigned p_pos)
{
    if (m_state == RuleStates::NotMatched || m_state == RuleStates::Matched)
        throw "error";
    m_state = p_termState;
}

// =====================================================================================================================
void SlotState::processEvent(SharedRuleState p_sender)
{
    m_ruleState = p_sender;
    switch (m_slotType) {
    case SlotType::One:
        return processEventOne(*p_sender);
    case SlotType::OneMore:
        return processEventOneMore(*p_sender);
    default:
        break;
    }
}

// =====================================================================================================================
void SlotState::processEventOne(RuleState& p_sender)
{
    if (m_state == RuleStates::NotMatched || m_state == RuleStates::Matched)
        throw "error";
    m_state = p_sender.getState();

    if (m_state == RuleStates::Init) {
        throw "error";
    }
}

// =====================================================================================================================
void SlotState::processEventOneMore(RuleState& p_sender)
{
    if (m_state == RuleStates::NotMatched)
        throw "error";
    m_state = p_sender.getState();
    if (m_state == RuleStates::Matched) {
        m_state = RuleStates::Growing;
    }
    if (m_state == RuleStates::Init) {
        throw "error";
    }
}

// =====================================================================================================================
void SlotState::reset()
{
    m_state    = RuleStates::Init;
    m_slotType = SlotType::Empty;
    m_ruleState.reset();
}

// =====================================================================================================================
void SlotState::reset(SharedRuleState p_ruleState, SlotType p_slotType)
{
    m_ruleState = p_ruleState;
    m_slotType  = p_slotType;
    m_state     = RuleStates::Init;
}

// =====================================================================================================================
// =====================================================================================================================
// =====================================================================================================================
void AndOrCommon::init()
{
    //    std::cout << GetName() << " created" << std::endl;
    if (m_rule->m_slots.empty())
        throw "Empty grammar in rule";
}

// =====================================================================================================================
std::string AndOrCommon::printResult() const
{
    //    std::cout << "PrintDebug" << GetName() << std::endl;

    std::stringstream ret;
    ret << getName() << "(";

    for (unsigned i = 0; i < m_slots.size(); ++i) {
        if (m_slots[i].getState() != RuleStates::NotMatched) {
            if (i)
                ret << ", ";
            ret << m_slots[i].m_ruleState->printValue();
        }
    }
    ret << ")";

    return ret.str();
}

// =====================================================================================================================
void AndOrCommon::processTermEvent(unsigned p_slotIndex, RuleStates p_termState, unsigned p_pos)
{
    if (p_termState == RuleStates::Matched) {
        //        std::cout << GetName() << ":" << p_slotIndex << " got term match" << std::endl;
    }
    SlotState& slot = m_slots[p_slotIndex];
    slot.processTermEvent(p_termState, p_pos);
    slotStateChanged(slot);
}

// =====================================================================================================================
void AndOrCommon::processEvent(unsigned p_slotIndex, SharedRuleState p_sender)
{
    if (m_parser.isDebugMode()) {
#if 0
        std::cout << GetName() << ":" << p_slotIndex << " got " << p_sender->GetState()
            << " from " << p_sender->GetName() << std::endl;
        if (GetState() == RuleStates::NotMatched)
            std::cout << "Not-matched obj got event!" << std::endl;;
#endif
    }

    // sanity check
    if (getState() == RuleStates::NotMatched)
        throw "Not-matched obj got event!";

    if (m_slots.empty())
        throw "Slots are missing!";

    SlotState& slot = m_slots[p_slotIndex];
    slot.processEvent(p_sender);
    slotStateChanged(slot);
}

// =====================================================================================================================
void AndOrCommon::createTypedSlot(SlotType p_slotType, unsigned p_slotIndex, SharedRuleState p_ruleState)
{
    SlotState& slotState = m_slots[p_slotIndex];
    SlotType slotType    = p_slotType;

    // in case of optional slot match case handled earlier in the cloned branch
    if (slotType == SlotType::Optional)
        slotType = SlotType::One;
    else if (slotType == SlotType::Any)
        slotType = SlotType::OneMore;

    switch (slotType) {
    case SlotType::One:
        slotState.reset(p_ruleState, SlotType::One);
        break;
    case SlotType::OneMore:
        slotState.reset(p_ruleState, SlotType::OneMore);
        break;
    default:
        abort();
        break;
    }
}

// =====================================================================================================================
void AndOrCommon::initSlot(unsigned p_slotIndex)
{
    //    std::cout << "InitSlot " << GetName() << ":" << p_slotIndex << std::endl;

    const Rule* slotRule      = m_rule->m_slots[p_slotIndex].m_rule;
    SharedRuleState ruleState = m_parser.getRuleState(slotRule, this, p_slotIndex);

    createTypedSlot(m_rule->getSlotType(p_slotIndex), p_slotIndex, ruleState);
    postInitTermSlot(p_slotIndex);
}

// =====================================================================================================================
void AndOrCommon::postInitTermSlot(unsigned p_slotIndex)
{
    const Rule* rule = m_rule->m_slots[p_slotIndex].m_rule;
    if (rule->getType() == RuleType::Term) {
        Parser::TermWaitEvent event;
        event.m_ruleState = m_selfRef;
        event.m_termRule  = rule;
        event.m_slotIndex = p_slotIndex;
        m_parser.subscribeTermEvent(event);
    }
}

// =====================================================================================================================
void AndOrCommon::notifyAllParentsFromStateChange(SharedRuleState p_ruleState)
{
    for (auto it : m_parentSlots) {
        auto parentNode = it.first;
        auto slotIndex  = it.second;
        m_parser.addRuleStateNotification([parentNode, slotIndex, p_ruleState]() { parentNode->processEvent(slotIndex, p_ruleState); });
    }
}

// =====================================================================================================================
void AndOrCommon::notifyAllParentsFromClone(SharedRuleState p_ruleState)
{
    //    std::cout << "DDDD m_parentSlots.size: " << m_parentSlots.size() << std::endl;
    for (auto it : m_parentSlots) {
        //        std::cout << "DDDD " << GetName() << " notif " << it->first->GetName() << std::endl;
        auto parentNode = it.first;
        m_parser.addRuleStateNotification([parentNode, p_ruleState]() { parentNode->cloneWith(p_ruleState); });
    }
}

// =====================================================================================================================
AndOrCommon::~AndOrCommon()
{
}

// =====================================================================================================================
// =====================================================================================================================
// =====================================================================================================================
void AndState::dotWriterVisitor(DotWriter& p_writer) const
{
    if (!p_writer.Print(this))
        return;
    if (!m_slots.empty() && !m_slots[m_slotIndex].isEmpty()) {
        m_slots[m_slotIndex].m_ruleState->dotWriterVisitor(p_writer);
    }
}

// =====================================================================================================================
std::string AndState::printAsDot() const
{
    std::stringstream ret;
    ret << getId() << " [label =<\n";
    ret << "<TABLE BORDER=\"1\" CELLBORDER=\"0\">\n";
    ret << "    <TR>\n";
    ret << "        <TD COLSPAN=\"4\" bgcolor=\"dodgerblue1\">" << QuoteAsHtml(getRule()->getName()) << "</TD>\n";
    ret << "    </TR>\n";
    ret << "    <TR>\n";
    ret << "        <TD bgcolor=\"wheat\">And</TD>\n";
    ret << "        <TD bgcolor=\"wheat\">Id: " << m_id << "</TD>\n";
    ret << "        <TD bgcolor=\"wheat\">" << getState() << "</TD>\n";
    ret << "        <TD bgcolor=\"wheat\">" << m_pos << "+" << m_length << "=" << getIndex() << "</TD>\n";
    ret << "    </TR>\n";

    for (unsigned i = 0; i < m_slots.size(); i++) {
        ret << "    <TR>\n";
        ret << "        <TD bgcolor=\"lightgoldenrod2\">" << i << "</TD>\n";
        ret << "        <TD COLSPAN=\"3\" ";
        if (i < m_slotIndex)
            ret << "bgcolor=\"palegreen\"";
        else if (m_slotIndex == i) {
            if (getState() != RuleStates::Matched)
                ret << "bgcolor=\"orangered\"";
            else
                ret << "bgcolor=\"palegreen\"";
            ret << " port=\"f" << i << "\"";
        }
        ret << ">" << QuoteAsHtml(getRule()->m_slots[i].m_rule->getName()) << "</TD>\n";
        ret << "    </TR>\n";
    }
    if (getState() == RuleStates::Matched) {
        ret << "    <TR>\n";
        ret << "        <TD bgcolor=\"wheat\">Res:</TD>\n";
        ret << "        <TD COLSPAN=\"3\" bgcolor=\"darkolivegreen1\">" << m_value << "</TD>\n";
        ret << "</TR>\n";
    }
    ret << "</TABLE>>];\n";

    const Rule* rule = getRule()->m_slots[m_slotIndex].m_rule;
    if (!m_slots.empty() && !m_slots[m_slotIndex].isEmpty() && m_state != RuleStates::Matched && rule->getType() != RuleType::Term)
        ret << getId() << ":f" << m_slotIndex << " -> " << m_slots[m_slotIndex].m_ruleState->getId() << ";\n";

    return ret.str();
}

// =====================================================================================================================
void AndState::init()
{
    AndOrCommon::init();
    initSlot();
}

// =====================================================================================================================
void AndState::initSlot()
{
    AndOrCommon::initSlot(m_slotIndex);
    if (IsOptionalType(getRule()->m_slots[m_slotIndex].m_type)) {
        SharedRuleState sharedClone;
        cloneMySelf(sharedClone)->handleEmptyMatchClone();
    }
}

// =====================================================================================================================
void AndState::handleEmptyMatchClone()
{
    handleValueSaving(TRuleValue());
    handleMatchedClone();
}

// =====================================================================================================================
void AndState::handleMatchedClone()
{
    handleStepping();
    notifyAllParentsFromClone(getSelfSPtr());
}

// =====================================================================================================================
void AndState::handleStepping()
{
    m_slots[m_slotIndex].reset();
    if (isLastRule()) {
        m_state = RuleStates::Matched;
    } else {
        m_slotIndex++;
        m_state = RuleStates::Accepted;
        initSlot();
    }
}

// =====================================================================================================================
void AndState::slotStateChanged(SlotState& p_slot)
{
    const Rule* rule = getRule()->m_slots[m_slotIndex].m_rule;
    m_state          = p_slot.getState();
    notifyAllParentsFromStateChange(getSelfSPtr());
    switch (m_state) {
    case RuleStates::Init:
        throw "Invalid slot state!";

    case RuleStates::NotMatched:
        m_slots.clear();
        return;

    case RuleStates::Accepted:
        m_length++;
        return;

    case RuleStates::Growing: {
        m_length++;
        // std::cout << "DDDD any " << GetName() << " " << rule->GetName() << std::endl;
        handleValueSaving(p_slot.m_ruleState->getValue());
        SharedRuleState sharedClone;
        cloneMySelf(sharedClone)->handleMatchedClone();
        m_state            = RuleStates::Accepted;
        p_slot.m_ruleState = m_parser.getRuleState(rule, this, m_slotIndex);
        postInitTermSlot(m_slotIndex);
    } break;

    case RuleStates::Matched: {
        m_length++;
        handleValueSaving(p_slot.m_ruleState->getValue());
        handleStepping();
    } break;
    }
}

// =====================================================================================================================
void AndState::updateParentSlot(AndOrCommon* p_oldParent, AndOrCommon* p_newParent, unsigned p_slotIndex)
{
    if (p_oldParent == p_newParent)
        m_parentSlots[p_oldParent] = p_slotIndex;
    else {
        m_parentSlots.erase(p_oldParent);
        m_parentSlots[p_newParent] = p_slotIndex;
    }
}

// =====================================================================================================================
AndState* AndState::cloneMySelf(SharedRuleState& p_sharedClone)
{
    AndState* clone = new AndState(*this);
    p_sharedClone.reset(clone);
    clone->setSelfRef(p_sharedClone);
    if (m_value)
        clone->m_value = m_value->clone();
    clone->m_id = s_id++;
    //    std::cout << GetName() << " cloned to " << clone->GetName() << std::endl;

    return clone;
}

// =====================================================================================================================
void AndState::cloneWith(SharedRuleState p_clonedState)
{
    //    std::cout << "DDDD GetIndex: " << GetIndex() << " : " << p_clonedState->GetIndex() << std::endl;

    SharedRuleState sharedClone;
    AndState* clone = cloneMySelf(sharedClone);

    if (p_clonedState->getIndex() > getIndex())
        clone->m_length++;

    if (p_clonedState->getState() == RuleStates::Matched) {
        clone->handleValueSaving(p_clonedState->getValue());
        clone->handleStepping();
    } else {
        static_cast<AndOrCommon*>(p_clonedState.get())->updateParentSlot(this, clone, m_slotIndex);
        clone->m_slots[m_slotIndex].m_ruleState = p_clonedState;
        //       std::cout << "DDDD " << GetName() << ":" << m_slotIndex << " -> " << p_clonedState->GetName() << std::endl;
    }
    clone->notifyAllParentsFromClone(sharedClone);
}

// =====================================================================================================================
void AndState::handleValueSaving(TRuleValue p_ruleValue)
{
    if (!m_value)
        m_value = getRule()->createRuleValue();
    m_value->addValue(p_ruleValue);
}

// =====================================================================================================================
// =====================================================================================================================
// =====================================================================================================================
void OrState::init()
{
    AndOrCommon::init();

    for (unsigned i = 0; i < m_slots.size(); ++i) {
        initSlot(i);
        m_liveSlotSlotsNum++;
    }
};

// =====================================================================================================================
void OrState::dotWriterVisitor(DotWriter& p_writer) const
{
    if (!p_writer.Print(this))
        return;
    for (unsigned i = 0; i < m_slots.size(); i++) {
        if (!m_slots[i].isEmpty())
            m_slots[i].m_ruleState->dotWriterVisitor(p_writer);
    }
}

// =====================================================================================================================
std::string OrState::printAsDot() const
{
    std::stringstream ret;
    ret << getId() << " [label =<\n";
    ret << "<TABLE BORDER=\"1\" CELLBORDER=\"0\">\n";
    ret << "    <TR>\n";
    ret << "        <TD COLSPAN=\"4\">" << QuoteAsHtml(getRule()->getName()) << "</TD>\n";
    ret << "    </TR>\n";
    ret << "    <TR>\n";
    ret << "        <TD bgcolor=\"wheat\">Or</TD>\n";
    ret << "        <TD bgcolor=\"wheat\">Id: " << m_id << "</TD>\n";
    ret << "        <TD bgcolor=\"wheat\">" << getState() << "</TD>\n";
    ret << "        <TD bgcolor=\"wheat\">" << m_pos << "+" << m_length << "=" << getIndex() << "</TD>\n";
    ret << "    </TR>\n";
    unsigned termCount = 0;
    for (unsigned i = 0; i < m_slots.size(); i++) {
        if (!m_slots[i].isEmpty()) {
            bool isTerm = m_slots[i].m_ruleState->getRule()->getType() == RuleType::Term;
            if (termCount == 0)
                ret << "    <TR>\n";
            if (isTerm) {
                ret << "        <TD ";
            } else {
                ret << "        <TD bgcolor=\"lightgoldenrod2\">" << i << "</TD>\n";
                ret << "        <TD bgcolor=\"lightgoldenrod2\">Id:" << m_slots[i].m_ruleState->getId() << "</TD>\n";
                ret << "        <TD COLSPAN=\"2\" ";
            }
            if (isTerm)
                ret << "bgcolor=\"lightyellow3\"";
            else if (m_slots[i].getState() == RuleStates::Matched)
                ret << "bgcolor=\"palegreen\"";
            if (!isTerm)
                ret << " port=\"f" << i << "\"";
            ret << ">" << QuoteAsHtml(m_slots[i].m_ruleState->getRule()->getName()) << "</TD>\n";

            if (isTerm && termCount < 3)
                termCount++;
            else
                termCount = 0;
            if (termCount == 0)
                ret << "    </TR>\n";
        }
    }
    if (termCount != 0)
        ret << "    </TR>\n";
    ret << "</TABLE>>];\n";

    for (unsigned i = 0; i < m_slots.size(); i++) {
        if (!m_slots[i].isEmpty() && m_slots[i].m_ruleState->getType() != RuleType::Term)
            ret << getId() << ":f" << i << " -> " << m_slots[i].m_ruleState->getId() << ";\n";
    }
    ret << "\n";

    return ret.str();
}

// =====================================================================================================================
TRuleValue OrState::getValue() const
{
    if (m_matchedRule)
        return m_matchedRule->getValue();

    return m_value;
}

// =====================================================================================================================
void OrState::handleRootResult(SharedRuleState p_clone)
{
    if (p_clone->getState() == RuleStates::Matched) {
        if (!m_matchedRule || (m_matchedRule->getIndex() < p_clone->getIndex()))
            m_matchedRule = p_clone;
        for (unsigned i = 0; i < m_slots.size(); ++i) {
            if (m_slots[i].isEmpty())
                continue;
            if (m_slots[i].m_ruleState->getState() == RuleStates::Matched) {
                createTypedSlot(SlotType::One, i, p_clone);
                return;
            }
        }
    }

    saveToNewSlot(p_clone);
}

// =====================================================================================================================
void OrState::saveToNewSlot(SharedRuleState p_clonedState)
{
    size_t size = m_slots.size();
    m_slots.resize(size + 1);
    static_cast<AndOrCommon*>(p_clonedState.get())->updateParentSlot(this, this, size);
    createTypedSlot(SlotType::One, size, p_clonedState);

    if (p_clonedState->getState() == RuleStates::Matched)
        return;

    m_liveSlotSlotsNum++;
    if (getIndex() < p_clonedState->getIndex())
        m_checkedSlotsNum++;
}

// =====================================================================================================================
void OrState::cloneWith(SharedRuleState p_clonedState)
{
    if (m_parentSlots.empty()) {
        handleRootResult(p_clonedState);
        return;
    }

    if (p_clonedState->getState() == RuleStates::Matched)
        notifyAllParentsFromClone(p_clonedState);
    else
        saveToNewSlot(p_clonedState);
}

// =====================================================================================================================
void OrState::prepareStateChange(SlotState& p_slot)
{
    m_checkedSlotsNum++;

    switch (p_slot.getState()) {
    case RuleStates::Growing:
    case RuleStates::Matched:
        m_matchedSlotsNum++;
        break;
    case RuleStates::Accepted:
        m_acceptedSlotsNum++;
        break;
    case RuleStates::Init:
        throw "Invalid slot state!";
    case RuleStates::NotMatched:
        break;
    }
}

// =====================================================================================================================
void OrState::handleMatchedSlot(SharedRuleState p_ruleState)
{
    if (m_acceptedSlotsNum > 0 || m_matchedSlotsNum > 1)
        notifyAllParentsFromClone(p_ruleState);
    else
        notifyAllParentsFromStateChange(p_ruleState);
    m_matchedSlotsNum--;
    m_liveSlotSlotsNum--;
}

// =====================================================================================================================
void OrState::slotStateChanged(SlotState& p_slot)
{
    prepareStateChange(p_slot);
    if (m_liveSlotSlotsNum != m_checkedSlotsNum)
        return;
    m_checkedSlotsNum = 0;
    m_length++;
    //    std::cout << "    DDDD " << GetName() << " SlotStateChanged, len: " << GetIndex() << "\n";

    bool noMatchAndAccept = (m_matchedSlotsNum == 0 && m_acceptedSlotsNum == 0);
    for (SlotState& slotState : m_slots) {
        if (slotState.isEmpty())
            continue;
        SharedRuleState ruleState = slotState.m_ruleState;
        switch (slotState.getState()) {
        case RuleStates::Growing:
        case RuleStates::Matched:
            if (m_parentSlots.empty()) {
                if (m_matchedRule && m_matchedRule->getIndex() > ruleState->getIndex())
                    continue;
                m_matchedRule = ruleState;
                continue;
            }
            handleMatchedSlot(ruleState);
            slotState.reset();
            break;

        case RuleStates::NotMatched:
            m_liveSlotSlotsNum--;
            slotState.reset();
            break;

        case RuleStates::Init:
        case RuleStates::Accepted:
            break;
        }
    }
    if (m_acceptedSlotsNum > 0) {
        m_state = RuleStates::Accepted;
        notifyAllParentsFromStateChange(getSelfSPtr());
    } else if (m_liveSlotSlotsNum == 0 && noMatchAndAccept) {
        m_state = RuleStates::NotMatched;
        notifyAllParentsFromStateChange(getSelfSPtr());
    }
    m_matchedSlotsNum  = 0;
    m_acceptedSlotsNum = 0;
}

// =====================================================================================================================
std::string OrState::printValue() const
{
    return "DDDD COrState::PrintValue";
}

// =====================================================================================================================
void TermRuleState::dotWriterVisitor(DotWriter& p_writer) const
{
    //    p_writer.Print(this);
}

// =====================================================================================================================
std::ostream& operator<<(std::ostream& p_os, const synth::RuleStates p_state)
{
    switch (p_state) {
    case RuleStates::Growing:
        p_os << "Matched";
        break;
    case RuleStates::Matched:
        p_os << "Matched";
        break;
    case RuleStates::Accepted:
        p_os << "Accepted";
        break;
    case RuleStates::NotMatched:
        p_os << "NotMatched";
        break;
    case RuleStates::Init:
        p_os << "Init";
        break;
    }

    return p_os;
}

} // namespace synth
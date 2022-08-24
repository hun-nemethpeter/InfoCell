#pragma once

#include <functional>
#include <map>
#include <memory>
#include <set>

#include <parser/Rule.h>
#include <parser/Value.h>
#include <parser/DotWriter.h>

namespace synth {

class Parser;
class RuleState;
class SlotState;
class AndOrCommon;
typedef std::shared_ptr<RuleState> SharedRuleState;

enum class RuleStates
{
    Init,      // initial state
    Matched,   // state fully matched
    Growing,   // one slot fully matched but it can grow
    Accepted,  // state partially matched
    NotMatched // state not matched
};
typedef std::function<void()> TNotifyFunction;

// =====================================================================================================================
class RuleState
{
public:
    virtual ~RuleState();

    RuleStates getState() const
    {
        return m_state;
    }

    const Rule* getRule() const
    {
        return m_rule;
    }

    const RuleType getType() const
    {
        return m_rule->getType();
    }

    unsigned getPos() const
    {
        return m_pos;
    }

    void setSelfRef(const SharedRuleState& p_selfPtr)
    {
        m_selfRef = p_selfPtr;
    }

    SharedRuleState getSelfSPtr() const
    {
        return m_selfRef.lock();
    }

    virtual void init() { }

    std::string getName() const;

    virtual std::string printAsDot() const
    {
        return m_rule->getName();
    }

    virtual std::string printResult() const
    {
        return m_rule->getName();
    }

    virtual std::string printValue() const
    {
        return getName();
    }

    virtual TRuleValue getValue() const
    {
        return m_value;
    }

    unsigned getId() const
    {
        return m_id;
    }

    unsigned getLength() const
    {
        return m_length;
    }

    unsigned getIndex() const
    {
        return m_length + m_pos;
    }

    virtual void dotWriterVisitor(DotWriter& p_writer) const
    { }

protected:
    RuleState(const Rule* p_rule, Parser& p_parser, unsigned p_pos);

    std::weak_ptr<RuleState> m_selfRef;
    TRuleValue m_value;

    RuleStates m_state;
    const Rule* m_rule;
    Parser& m_parser;
    static unsigned s_id;
    unsigned m_id;

    // start position
    unsigned m_pos;
    unsigned m_length;
};

// =====================================================================================================================
class SlotState
{
public:
    ~SlotState()
    {
        m_state    = RuleStates::Init;
        m_slotType = SlotType::Empty;
        m_ruleState.reset();
    }

    void processEvent(SharedRuleState p_sender);
    void processEventOne(RuleState& p_sender);
    void processEventOneMore(RuleState& p_sender);
    void processTermEvent(RuleStates p_termState, unsigned p_pos);
    void reset();
    void reset(SharedRuleState p_ruleState, SlotType p_slotType);

    bool isEmpty() const
    {
        return m_slotType == SlotType::Empty;
    }

    RuleStates getState() const
    {
        return m_state;
    }

    // rule state in slot
    SharedRuleState m_ruleState;

private:
    SlotType m_slotType = SlotType::Empty;
    RuleStates m_state  = RuleStates::Init;
};

// =====================================================================================================================
class TermRuleState : public RuleState
{
public:
    TermRuleState(const TermRule* p_rule, Parser& p_parser, unsigned p_pos = 0) :
        RuleState(p_rule, p_parser, p_pos)
    {
        m_value  = TRuleValue(new TermValue(p_rule));
        m_state  = RuleStates::Matched;
        m_length = 1;
    }
    virtual void dotWriterVisitor(DotWriter& p_writer) const;
};

// =====================================================================================================================
class AndOrCommon : public RuleState
{
public:
    AndOrCommon(const Rule* p_rule, Parser& p_parser, unsigned p_pos) :
        RuleState(p_rule, p_parser, p_pos),
        m_slots(p_rule->m_slots.size())
    { }

    virtual ~AndOrCommon();

    virtual void removeParentState(AndOrCommon* p_parent)
    {
        m_parentSlots.erase(p_parent);
    }

    virtual void init();

    virtual std::string printResult() const;

    virtual void slotStateChanged(SlotState& p_slot) = 0;

    virtual void cloneWith(SharedRuleState p_clonedState) = 0;

    void processTermEvent(unsigned p_slotIndex, RuleStates p_termState, unsigned p_pos);

    void processEvent(unsigned p_slotIndex, SharedRuleState p_sender);

    void addParentSlot(AndOrCommon* p_parent, unsigned p_slotIndex)
    {
        m_parentSlots[p_parent] = p_slotIndex;
    }

    virtual void updateParentSlot(AndOrCommon* p_oldParent, AndOrCommon* p_newParent, unsigned p_slotIndex) { }

protected:
    void initSlot(unsigned p_slotIndex);

    void postInitTermSlot(unsigned p_slotIndex);

    void createTypedSlot(SlotType p_slotType, unsigned p_slotIndex, SharedRuleState p_ruleState);

    void notifyAllParentsFromStateChange(SharedRuleState p_ruleState);

    void notifyAllParentsFromClone(SharedRuleState p_ruleState);

    std::vector<SlotState> m_slots;
    typedef std::map<AndOrCommon*, unsigned> ParentSlots;
    ParentSlots m_parentSlots;
};

// =====================================================================================================================
class AndState : public AndOrCommon
{
public:
    AndState(const Rule* p_rule, Parser& p_parser, unsigned p_pos) :
        AndOrCommon(p_rule, p_parser, p_pos),
        m_slotIndex(0)
    { }

    virtual void init();

    virtual void slotStateChanged(SlotState& p_slot);

    virtual void cloneWith(SharedRuleState p_clonedState);

    virtual std::string printAsDot() const;

    virtual void dotWriterVisitor(DotWriter& p_writer) const;

    virtual void updateParentSlot(AndOrCommon* p_oldParent, AndOrCommon* p_newParent, unsigned p_slotIndex);

private:
    void initSlot();
    void handleStepping();
    void handleValueSaving(TRuleValue p_ruleValue);
    void handleEmptyMatchClone();
    void handleMatchedClone();
    AndState* cloneMySelf(SharedRuleState& p_sharedClone);

    bool isLastRule() const
    {
        return m_slotIndex + 1 == m_slots.size();
    }

    unsigned m_slotIndex;
};

// =====================================================================================================================
class OrState : public AndOrCommon
{
public:
    OrState(const Rule* p_rule, Parser& p_parser, unsigned p_pos) :
        AndOrCommon(p_rule, p_parser, p_pos),
        m_checkedSlotsNum(0),
        m_liveSlotSlotsNum(0),
        m_matchedSlotsNum(0),
        m_acceptedSlotsNum(0)

    { }

    virtual std::string printValue() const;

    virtual void init();

    virtual void slotStateChanged(SlotState& p_slot);

    virtual void cloneWith(SharedRuleState p_clonedState);

    virtual std::string printAsDot() const;

    virtual TRuleValue getValue() const;

    virtual void dotWriterVisitor(DotWriter& p_writer) const;

private:
    void prepareStateChange(SlotState& p_slot);
    void handleMatchedSlot(SharedRuleState p_ruleState);
    void handleRootResult(SharedRuleState p_clone);
    void saveToNewSlot(SharedRuleState p_clonedState);

    unsigned m_checkedSlotsNum;
    unsigned m_liveSlotSlotsNum;
    unsigned m_matchedSlotsNum;
    unsigned m_acceptedSlotsNum;
    SharedRuleState m_matchedRule;
};

std::ostream& operator<<(std::ostream& p_os, const synth::RuleStates p_state);

} // namespace synth

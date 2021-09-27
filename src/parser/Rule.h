#pragma once
#include <memory>

#include <string>
#include <vector>

namespace synth {

class Grammar;
class Parser;
class Rule;
class RuleState;
class RuleValue;

typedef std::shared_ptr<Rule> TRule;
typedef std::shared_ptr<RuleState> SharedRuleState;
typedef std::shared_ptr<RuleValue> TRuleValue;

enum class SlotType
{
    One,
    OneMore,
    Optional,
    Any,
    Empty,
};

extern SlotType One;
extern SlotType OneMore;
extern SlotType Optional;
extern SlotType Any;

class RuleSlot
{
public:
    RuleSlot(const Rule* p_rule, SlotType p_slotType) :
        m_rule(p_rule), m_type(p_slotType) { }
    const Rule* m_rule;
    SlotType m_type;
};

typedef std::shared_ptr<RuleSlot> TRuleSlot;

enum class RuleType
{
    Term,
    And,
    Or
};

class Rule
{
public:
    Rule(const std::string& p_name, Grammar& p_grammar, RuleType p_type) :
        m_name(p_name), m_grammar(p_grammar), m_type(p_type)
    { }
    virtual ~Rule() { }

    virtual const std::string& getName() const
    {
        return m_name;
    }

    RuleType getType() const
    {
        return m_type;
    }

    SlotType getSlotType(unsigned p_index) const
    {
        return m_slots[p_index].m_type;
    }

    Grammar& getGrammar() const
    {
        return m_grammar;
    }

    virtual SharedRuleState createRuleState(Parser& p_parser, unsigned p_pos) const = 0;
    virtual TRuleValue createRuleValue() const                                      = 0;

    friend TRule operator<<(TRule p_rule1, const TRule& p_rule2);
    friend TRule operator<<(TRule p_rule1, const Rule* p_rule2);
    friend TRule operator<<(TRule p_rule1, unsigned char p_char);
    friend TRule operator<<(TRule p_rule1, const char* p_str);
    friend TRule operator<<(TRule p_rule1, SlotType)
    {
        throw "Grammar error";
    }

    friend TRule operator|(TRule p_rule1, const TRule& p_rule2);
    friend TRule operator|(TRule p_rule1, const Rule* p_rule2);
    friend TRule operator|(TRule p_rule1, const TRuleSlot& p_ruleSlot);
    friend TRule operator|(TRule p_rule1, unsigned char p_char);
    friend TRule operator|(TRule p_rule1, const char* p_str);
    friend TRule operator|(TRule p_rule1, SlotType)
    {
        throw "Grammar error";
    }

    friend TRule operator&(TRule p_rule1, const TRule& p_rule2);
    friend TRule operator&(TRule p_rule1, const Rule* p_rule2);
    friend TRule operator&(TRule p_rule1, const TRuleSlot& p_ruleSlot);
    friend TRule operator&(TRule p_rule1, unsigned char p_char);
    friend TRule operator&(TRule p_rule1, const char* p_str);
    friend TRule operator&(TRule p_rule1, SlotType)
    {
        throw "Grammar error";
    }

    friend TRuleSlot operator%(const Rule* p_rule, SlotType p_slotType);
    friend TRuleSlot operator%(TRule p_rule, SlotType p_slotType);

    struct SSlot
    {
        const Rule* m_rule;
        SlotType m_type;
    };

    std::vector<SSlot> m_slots;

protected:
    virtual void addRule(const Rule* p_rule, SlotType p_slotType)
    {
        SSlot slot = { p_rule, p_slotType };
        m_slots.push_back(slot);
    }

    std::string m_name;

    Grammar& m_grammar;
    RuleType m_type;

    friend class Grammar;
};

std::ostream& operator<<(std::ostream& p_os, const RuleType& p_type);

class TermRule : public Rule
{
public:
    TermRule(unsigned char p_char, Grammar& p_grammar);

    virtual SharedRuleState createRuleState(Parser& p_parser, unsigned p_pos) const;
    virtual TRuleValue createRuleValue() const;
    unsigned char getTermValue() const
    {
        return m_char;
    }

protected:
    virtual void addRule(const Rule* p_rule, SlotType p_slotType)
    {
        throw "error";
    }
    unsigned char m_char;
};

class AndRule : public Rule
{
public:
    AndRule(const std::string& p_name, Grammar& p_grammar) :
        Rule(p_name, p_grammar, RuleType::And)
    { }

    virtual SharedRuleState createRuleState(Parser& p_parser, unsigned p_pos) const;
    virtual TRuleValue createRuleValue() const;
};

template <class ValueType>
class TAndRule : public AndRule
{
public:
    TAndRule(const std::string& p_name, Grammar& p_grammar) :
        AndRule(p_name, p_grammar) { }

    virtual SharedRuleState createRuleState(Parser& p_parser, unsigned p_pos) const
    {
        return AndRule::createRuleState(p_parser, p_pos);
    }

    virtual TRuleValue createRuleValue() const
    {
        return TRuleValue(new ValueType());
    }
};

class OrRule : public Rule
{
public:
    OrRule(const std::string& p_name, Grammar& p_grammar) :
        Rule(p_name, p_grammar, RuleType::Or)
    { }

    virtual SharedRuleState createRuleState(Parser& p_parser, unsigned p_pos) const;
    virtual TRuleValue createRuleValue() const;
};

} // namespace synth
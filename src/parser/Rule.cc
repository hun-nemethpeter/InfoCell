#include <parser/Grammar.h>
#include <parser/RuleState.h>

#include "Rule.h"

namespace synth {

SlotType One      = SlotType::One;
SlotType OneMore  = SlotType::OneMore;
SlotType Optional = SlotType::Optional;
SlotType Any      = SlotType::Any;

// =====================================================================================================================
TRule operator<<(TRule p_rule1, const TRule& p_rule2)
{
    p_rule1->addRule(p_rule2.get(), SlotType::One);
    return p_rule1;
}

// =====================================================================================================================
TRule operator<<(TRule p_rule1, const Rule* p_rule2)
{
    p_rule1->addRule(p_rule2, SlotType::One);
    return p_rule1;
}

// =====================================================================================================================
TRule operator<<(TRule p_rule1, unsigned char p_char)
{
    p_rule1->addRule(p_rule1->getGrammar().GetTermFromChar(p_char), SlotType::One);
    return p_rule1;
}

// =====================================================================================================================
TRule operator<<(TRule p_rule1, const char* p_str)
{
    unsigned i = 0;
    while (p_str[i]) {
        p_rule1& p_str[i++];
    }
    // TODO store this as a string in grammar

    return p_rule1;
}

// =====================================================================================================================
TRule operator|(TRule p_rule1, const TRule& p_rule2)
{
    if (p_rule1->getType() != RuleType::Or)
        throw "Grammar error";
    p_rule1->addRule(p_rule2.get(), SlotType::One);
    return p_rule1;
}

// =====================================================================================================================
TRule operator|(TRule p_rule1, const Rule* p_rule2)
{
    if (p_rule1->getType() != RuleType::Or)
        throw "Grammar error";
    p_rule1->addRule(p_rule2, SlotType::One);
    return p_rule1;
}

// =====================================================================================================================
TRule operator|(TRule p_rule1, const TRuleSlot& p_ruleSlot)
{
    if (p_rule1->getType() != RuleType::Or)
        throw "Grammar error";
    p_rule1->addRule(p_ruleSlot->m_rule, p_ruleSlot->m_type);
    return p_rule1;
}

// =====================================================================================================================
TRule operator|(TRule p_rule1, unsigned char p_char)
{
    if (p_rule1->getType() != RuleType::Or)
        throw "Grammar error";
    p_rule1->addRule(p_rule1->getGrammar().GetTermFromChar(p_char), SlotType::One);
    return p_rule1;
}

// =====================================================================================================================
TRule operator|(TRule p_rule1, const char* p_str)
{
    if (p_rule1->getType() != RuleType::Or)
        throw "Grammar error";
    p_rule1->addRule(p_rule1->getGrammar().GetStringRule(p_str), SlotType::One);
    return p_rule1;
}

// =====================================================================================================================
TRule operator&(TRule p_rule1, const TRule& p_rule2)
{
    if (p_rule1->getType() != RuleType::And)
        throw "Grammar error";
    p_rule1->addRule(p_rule2.get(), SlotType::One);
    return p_rule1;
}

// =====================================================================================================================
TRule operator&(TRule p_rule1, const Rule* p_rule2)
{
    if (p_rule1->getType() != RuleType::And)
        throw "Grammar error";
    p_rule1->addRule(p_rule2, SlotType::One);
    return p_rule1;
}

// =====================================================================================================================
TRule operator&(TRule p_rule1, const TRuleSlot& p_ruleSlot)
{
    if (p_rule1->getType() != RuleType::And)
        throw "Grammar error";
    p_rule1->addRule(p_ruleSlot->m_rule, p_ruleSlot->m_type);
    return p_rule1;
}

// =====================================================================================================================
TRule operator&(TRule p_rule1, unsigned char p_char)
{
    if (p_rule1->getType() != RuleType::And)
        throw "Grammar error";
    p_rule1->addRule(p_rule1->getGrammar().GetTermFromChar(p_char), SlotType::One);
    return p_rule1;
}

// =====================================================================================================================
TRule operator&(TRule p_rule1, const char* p_str)
{
    if (p_rule1->getType() != RuleType::And)
        throw "Grammar error";
    p_rule1->addRule(p_rule1->getGrammar().GetStringRule(p_str), SlotType::One);
    return p_rule1;
}

// =====================================================================================================================
TRuleSlot operator%(const Rule* p_rule, SlotType p_slotType)
{
    return TRuleSlot(new RuleSlot(p_rule, p_slotType));
}

// =====================================================================================================================
TRuleSlot operator%(TRule p_rule, SlotType p_slotType)
{
    return TRuleSlot(new RuleSlot(p_rule.get(), p_slotType));
}

// =====================================================================================================================
SharedRuleState TermRule::createRuleState(Parser& p_parser, unsigned p_pos) const
{
    auto ret = SharedRuleState(new TermRuleState(this, p_parser, p_pos));
    ret->setSelfRef(ret);
    ret->init();
    return ret;
}

// =====================================================================================================================
SharedRuleState AndRule::createRuleState(Parser& p_parser, unsigned p_pos) const
{
    auto ret = SharedRuleState(new AndState(this, p_parser, p_pos));
    ret->setSelfRef(ret);
    return ret;
}

// =====================================================================================================================
SharedRuleState OrRule::createRuleState(Parser& p_parser, unsigned p_pos) const
{
    auto ret = SharedRuleState(new OrState(this, p_parser, p_pos));
    ret->setSelfRef(ret);
    return ret;
}

// =====================================================================================================================
TRuleValue TermRule::createRuleValue() const
{
    return TRuleValue(new TermValue(this));
}

// =====================================================================================================================
TRuleValue AndRule::createRuleValue() const
{
    return TRuleValue(new AndValue(this));
}

// =====================================================================================================================
TRuleValue OrRule::createRuleValue() const
{
    return TRuleValue(new AndValue(this));
}

// =====================================================================================================================
static std::string GetTermName(unsigned char p_char)
{
    switch (p_char) {
    case 10:
        return "\\n";
    default: {
        char charStr[2] = { 0, 0 };
        charStr[0]      = p_char;
        return std::string(charStr);
    }
    }

    throw "Term name";
}

// =====================================================================================================================
TermRule::TermRule(unsigned char p_char, Grammar& p_grammar) :
    Rule(GetTermName(p_char), p_grammar, RuleType::Term), m_char(p_char)
{
}

// =====================================================================================================================
std::ostream& operator<<(std::ostream& p_os, const RuleType& p_type)
{
    switch (p_type) {
    case RuleType::And:
        p_os << "And";
        break;
    case RuleType::Or:
        p_os << "Or";
        break;
    case RuleType::Term:
        p_os << "Term";
        break;
    }
    return p_os;
}

} // namespace synth
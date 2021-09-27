#include <parser/Rule.h>
#include "Grammar.h"

namespace synth {

// =====================================================================================================================
Grammar::Grammar(const std::string& p_name) :
    m_name(p_name)
{
    for (unsigned char ch = 0; ch < 128; ++ch) {
        m_terms.push_back(Term(ch));
    }
}

// =====================================================================================================================
TRule Grammar::Term(unsigned char p_char)
{
    TermRule* rule = new TermRule(p_char, *this);
    m_rules.insert(rule);
    return TRule(rule);
}

// =====================================================================================================================
TRule Grammar::And(const std::string& p_name)
{
    AndRule* rule = new AndRule(p_name, *this);
    m_rules.insert(rule);
    return TRule(rule);
}

// =====================================================================================================================
TRule Grammar::Or(const std::string& p_name)
{
    OrRule* rule = new OrRule(p_name, *this);
    m_rules.insert(rule);
    return TRule(rule);
}

// =====================================================================================================================
TRule Grammar::List(const TRule& p_item, const Rule* p_separator)
{
    std::string listName("ListOf");
    listName += p_item->getName();

    std::string listTailName("ListTailOf");
    listTailName += p_item->getName();

    TRule list     = And(listName);
    TRule listTail = And(listTailName);

    list& p_item& listTail % Any;

    listTail& p_separator& p_item;

    m_generatedRules.push_back(list);
    m_generatedRules.push_back(listTail);

    return list;
}

// =====================================================================================================================
TRule Grammar::List(const TRule& p_item, const TRule& p_separator)
{
    return List(p_item, p_separator.get());
}

// =====================================================================================================================
TRule Grammar::List(const TRule& p_item, const char* p_separator)
{
    return List(p_item, GetStringRule(p_separator));
}

// =====================================================================================================================
TRule Grammar::List(const TRule& p_item, unsigned char p_separator)
{
    return List(p_item, GetTermFromChar(p_separator));
}

// =====================================================================================================================
const Rule* Grammar::GetStringRule(const char* p_str)
{
    const Rule*& strRule = m_strings[p_str];
    if (!strRule) {
        Rule* rule = new AndRule(p_str, *this);
        unsigned i = 0;
        while (p_str[i])
            rule->addRule(GetTermFromChar(p_str[i++]), SlotType::One);
        m_rules.insert(rule);
        m_generatedRules.push_back(TRule(rule));
        strRule = rule;
    }

    return strRule;
}

} // namespace synth
#pragma once
#include <array>
#include <memory>

#include <set>
#include <map>

#include <parser/Rule.h>

namespace synth {

class Grammar
{
public:
    const Rule* GetTermFromChar(unsigned char p_char) const
    {
        return m_terms[p_char].get();
    }

    const Rule* GetStringRule(const char* p_str);

    const std::string& GetName() const
    {
        return m_name;
    }

    // registered rules
    typedef std::set<const Rule*> TRules;
    TRules m_rules;

    typedef std::vector<TRule> TTerms;
    // pre created terms, indexed with ascii chars
    TTerms m_terms;
    TTerms m_generatedRules;

protected:
    // Creates all Term rules
    Grammar(const std::string& p_name);

    // Creates a Term rule than registrates it in this grammar
    TRule Term(unsigned char p_char);

    // Creates an And rule than registrates it in this grammar
    TRule And(const std::string& p_name);

    // Creates an And rule than registrates it in this grammar
    template <class ValueType>
    TRule Obj(const std::string& p_name)
    {
        AndRule* rule = new TAndRule<ValueType>(p_name, *this);
        m_rules.insert(rule);
        return TRule(rule);
    }

    // Creates an Or rule than registrates it in this grammar
    TRule Or(const std::string& p_name);

    const Rule* Str(const char* p_str)
    {
        return GetStringRule(p_str);
    }

    const Rule* Char(unsigned char p_char) const
    {
        return GetTermFromChar(p_char);
    }

    // List macro
    TRule List(const TRule& p_item, const Rule* p_separator);
    TRule List(const TRule& p_item, const TRule& p_separator);
    TRule List(const TRule& p_item, unsigned char p_separator);
    TRule List(const TRule& p_item, const char* p_separator);

    typedef std::map<const char*, const Rule*> TStrings;
    TStrings m_strings;

    const std::string m_name;

    friend class Rule;
};

} // namespace synth
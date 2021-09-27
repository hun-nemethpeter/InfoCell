#include <parser/Rule.h>
#include "Value.h"

namespace synth {

// =====================================================================================================================
RuleValue::~RuleValue()
{ }

// =====================================================================================================================
std::ostream& operator<<(std::ostream& p_os, const TRuleValue& p_value)
{
    if (p_value)
        p_os << p_value->getValueAsString();
    return p_os;
}

// =====================================================================================================================
std::string TermValue::getValueAsString() const
{
    return m_rule->getName();
}

// =====================================================================================================================
unsigned char TermValue::getChar() const
{
    return dynamic_cast<const TermRule*>(m_rule)->getTermValue();
}

// =====================================================================================================================
void DecimalLiteralValue::addValue(const TRuleValue& p_value)
{
    if (!p_value) {
        //        std::cout << "DDDD TODO CDecimalLiteralValue::AddValue" << std::endl;
        return;
    }
    const TermValue& termValue = dynamic_cast<const TermValue&>(*p_value.get());
    m_number *= 10;
    m_number += termValue.getChar() - '0';
}

// =====================================================================================================================
std::string DecimalLiteralValue::getValueAsString() const
{
    std::stringstream ret;
    ret << m_number;
    return ret.str();
}

// =====================================================================================================================
void BinaryOpValue::addValue(const TRuleValue& p_value)
{
    switch (m_count++) {
    case 0:
        m_left = p_value;
        break;
    case 1:
        m_op = p_value;
        break;
    case 2:
        m_right = p_value;
        break;
    default:
        throw "error";
    }
}

// =====================================================================================================================
std::string BinaryOpValue::getValueAsString() const
{
    std::stringstream ret;
    ret << "(" << m_left << m_op << m_right << ")";
    return ret.str();
}

} // namespace synth
#pragma once
#include <memory>
#include <sstream>
#include <vector>

namespace synth {

class Rule;
class RuleValue;
typedef std::shared_ptr<RuleValue> TRuleValue;

class TermRule;

// =====================================================================================================================
class RuleValue
{
public:
    virtual ~RuleValue();

    virtual std::string getValueAsString() const = 0;

    virtual void addValue(const TRuleValue& p_value) = 0;

    virtual TRuleValue clone() = 0;

    const Rule* getRule() const
    {
        return m_rule;
    }

protected:
    const Rule* m_rule;
};
std::ostream& operator<<(std::ostream& p_os, const TRuleValue& p_value);

// =====================================================================================================================
class TermValue : public RuleValue
{
public:
    TermValue(const TermRule* p_termRule)
    {
        m_rule = p_termRule;
    }

    virtual std::string getValueAsString() const;

    virtual void addValue(const TRuleValue& p_value)
    {
        throw "error";
    }

    unsigned char getChar() const;

    virtual TRuleValue clone()
    {
        return TRuleValue(new TermValue(*this));
    }
};

// =====================================================================================================================
class DecimalLiteralValue : public RuleValue
{
public:
    DecimalLiteralValue() :
        m_number(0) { }
    virtual void addValue(const TRuleValue& p_value);

    virtual std::string getValueAsString() const;

    unsigned char getNumber() const
    {
        return m_number;
    }

    virtual TRuleValue clone()
    {
        return TRuleValue(new DecimalLiteralValue(*this));
    }

private:
    int m_number;
};

// =====================================================================================================================
class BinaryOpValue : public RuleValue
{
public:
    BinaryOpValue() :
        m_count(0) { }
    virtual void addValue(const TRuleValue& p_value);

    virtual std::string getValueAsString() const;

    virtual TRuleValue clone()
    {
        return TRuleValue(new BinaryOpValue(*this));
    }

private:
    TRuleValue m_left;
    TRuleValue m_right;
    TRuleValue m_op;
    int m_count;
};

// =====================================================================================================================
class AndValue : public RuleValue
{
public:
    AndValue(const Rule* p_rule)
    {
        m_rule = p_rule;
    }

    virtual std::string getValueAsString() const
    {
        std::stringstream ret;
        for (unsigned i = 0; i < m_values.size(); ++i)
            ret << m_values[i]->getValueAsString();

        return ret.str();
    }

    virtual void addValue(const TRuleValue& p_value)
    {
        if (!p_value)
            return;
        m_values.push_back(p_value);
    }

    const std::vector<TRuleValue>& getValues() const
    {
        return m_values;
    }

    virtual TRuleValue clone()
    {
        return TRuleValue(new AndValue(*this));
    }

private:
    std::vector<TRuleValue> m_values;
};

} // namespace synth
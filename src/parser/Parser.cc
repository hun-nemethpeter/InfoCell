#include <iostream>
#include <fstream>

#include "Parser.h"
namespace synth {

// =====================================================================================================================
Parser::Parser(const Grammar& p_grammar, const TRule& p_startRule, const std::string& p_text, bool p_debug) :
    m_grammar(p_grammar),
    m_startRule(p_startRule),
    m_pos(0), m_text(p_text),
    m_termWaitingStatesIndex(0),
    m_debug(p_debug)
{
    m_ruleStateNotifications.reserve(100);
    debugParsingHeader();
    initialize();
    if (m_debug)
        printAsDot();
    parse();
    handleFinish();
    //    PrintResult();
}

// =====================================================================================================================
void Parser::debugParsingHeader() const
{
    if (!m_debug)
        return;
    std::cout << "Parsing grammar " << m_grammar.GetName() << " : " << m_startRule->getName() << std::endl;
    std::cout << "================";
    for (unsigned i = 0; i < m_grammar.GetName().size() + m_startRule->getName().size() + 3; i++)
        std::cout << "=";
    std::cout << std::endl;
    std::cout << "expr: " << m_text << std::endl
              << std::endl;
}

// =====================================================================================================================
void Parser::initialize()
{
    OrRule* result = new OrRule("Result", const_cast<Grammar&>(m_grammar));
    m_resultRule.reset(result);
    m_resultRule | m_startRule;
    //    const CRule* startRule = p_startRule.get();

    // create term states
    for (Grammar::TTerms::const_iterator it = m_grammar.m_terms.begin(); it != m_grammar.m_terms.end(); ++it) {
        const TermRule* termRule = dynamic_cast<TermRule*>(it->get());
        m_termStates.push_back(std::shared_ptr<TermRuleState>(new TermRuleState(termRule, *this)));
    }

    // create start-rule-state
    m_resultRuleState = getRuleState(result);
    handleStateChanges();
    if (m_debug)
        std::cout << "\n";
}

// =====================================================================================================================
void Parser::parse()
{
    // parsing...
    for (unsigned i = 0; i < m_text.size(); ++i) {
        if (m_debug) {
            //            std::cout << "Trying char '" << m_text[i] << "'" << std::endl;
            //            std::cout << "========================================" << std::endl;
        }
        ++m_pos;
        m_rulesToStates.clear();
        handleTermWaitingStates(m_text[i]);
        handleStateChanges();
        //        if (m_debug)
        //            std::cout << "\n";
        if (m_resultRuleState->getState() == RuleStates::NotMatched) {
            std::cout << "Parse error";
            break;
        }
        if (m_debug) {
            printAsDot();
            printStates();
        }
    }
}

// =====================================================================================================================
void Parser::handleFinish()
{
}

// =====================================================================================================================
void Parser::handleTermWaitingStates(unsigned char p_char)
{
    const Rule* termRule                 = m_grammar.GetTermFromChar(p_char);
    TermWaitingStates& termWaitingStates = getTermWaitingStates();
    flipTermWaitingStatesIndex();

    for (unsigned i = 0; i < termWaitingStates.size(); ++i) {
        TermWaitEvent task = termWaitingStates[i];
        RuleStates state   = RuleStates::NotMatched;

        if (task.m_termRule == termRule)
            state = RuleStates::Matched;

        SharedRuleState ruleState = task.m_ruleState.lock();
        if (ruleState)
            dynamic_cast<AndOrCommon*>(ruleState.get())->processTermEvent(task.m_slotIndex, state, m_pos);
    }
    termWaitingStates.clear();
}

// =====================================================================================================================
void Parser::handleStateChanges()
{
    unsigned processedStateChanges = 0;
    while (m_ruleStateNotifications.size() != processedStateChanges) {
        m_ruleStateNotifications[processedStateChanges++]();
    }
    m_ruleStateNotifications.clear();
}

// =====================================================================================================================
SharedRuleState Parser::getRuleState(const Rule* p_rule, AndOrCommon* p_parent, unsigned p_slotIndex)
{
    if (p_rule->getType() == RuleType::Term) {
        return m_termStates[dynamic_cast<const TermRule*>(p_rule)->getTermValue()];
    }

    SharedRuleState ruleState;
    const RulesToStatesMap::const_iterator found = m_rulesToStates.find(p_rule);
    if (found != m_rulesToStates.end()) {
        ruleState = found->second->getSelfSPtr();
        if (p_parent)
            dynamic_cast<AndOrCommon*>(ruleState.get())->addParentSlot(p_parent, p_slotIndex);
    } else {
        ruleState = p_rule->createRuleState(*this, m_pos);
        //        std::cout << "DDDD ctor " << ruleState->GetName() << std::endl;
        m_rulesToStates[p_rule] = ruleState.get();
        if (p_parent)
            dynamic_cast<AndOrCommon*>(ruleState.get())->addParentSlot(p_parent, p_slotIndex);
        ruleState->init();
    }

    return ruleState;
}

// =====================================================================================================================
void Parser::printAsDot() const
{
    DotWriter writer(this);
}

// =====================================================================================================================
void Parser::printStates() const
{
}

// =====================================================================================================================
void Parser::printResult() const
{
    std::cout << "Expr:\n"
              << m_text << std::endl
              << std::endl;
    if (m_resultRuleState->getState() == RuleStates::NotMatched) {
        std::cout << "Parse error." << std::endl;
        return;
    }

    std::cout << "Result:\n"
              << m_resultRuleState->getValue() << std::endl;
}

// =====================================================================================================================
std::string Parser::getResult() const
{
    std::stringstream ret;
    if (m_resultRuleState->getState() != RuleStates::NotMatched)
        ret << m_resultRuleState->getValue();
    return ret.str();
}

// =====================================================================================================================
void Parser::printResultStates() const
{
    std::cout << "\nResult:\n"
              << std::endl;
    std::cout << "expr: " << m_text << std::endl
              << std::endl;
    std::cout << "\n"
              << std::endl;
}

} // namespace synth
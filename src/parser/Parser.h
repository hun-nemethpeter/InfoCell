#pragma once
#include <queue>

#include <parser/Grammar.h>
#include <parser/RuleState.h>

namespace synth {

class Parser
{
public:
    Parser(const Grammar& p_grammar, const TRule& p_startRule, const std::string& p_text, bool p_debug = false);

    void printResult() const;
    void printAsDot() const;
    std::string getResult() const;

    const RuleState* getResultRuleState() const
    {
        return m_resultRuleState.get();
    }

    unsigned getPos() const
    {
        return m_pos;
    }

    const std::string& getText() const
    {
        return m_text;
    }

protected:
    struct TermWaitEvent
    {
        // this rule-state waiting for a term
        std::weak_ptr<RuleState> m_ruleState;

        // slot index of term-rule in rule-state
        unsigned m_slotIndex;

        // this term is waited
        const Rule* m_termRule;
    };

    // Get or create a new rulestate for current position
    SharedRuleState getRuleState(const Rule* p_rule, AndOrCommon* p_parent = 0, unsigned p_slotIndex = 0);

    void subscribeTermEvent(const TermWaitEvent& p_msg)
    {
        getTermWaitingStates().push_back(p_msg);
    }

    void addRuleStateNotification(const TNotifyFunction& p_notify)
    {
        m_ruleStateNotifications.push_back(p_notify);
    }

    void parse();
    void initialize();
    void handleTermWaitingStates(unsigned char p_char);
    void handleStateChanges();
    void handleFinish();

private:
    void printStates() const;
    void printResultStates() const;

    bool isDebugMode() const
    {
        return m_debug;
    }

    // every rule has only one state for a text position
    typedef std::map<const Rule*, const RuleState*> RulesToStatesMap;
    RulesToStatesMap m_rulesToStates;

    // term states are always the same, so we stored here indexed by its ascii value
    typedef std::vector<SharedRuleState> TermStates;
    TermStates m_termStates;

    typedef std::vector<TermWaitEvent> TermWaitingStates;

    void debugParsingHeader() const;

    void flipTermWaitingStatesIndex()
    {
        m_termWaitingStatesIndex ^= 1;
    }

    TermWaitingStates& getTermWaitingStates()
    {
        return m_termWaitingStates[m_termWaitingStatesIndex];
    }

    const Grammar& m_grammar;
    const TRule m_startRule;
    TRule m_resultRule;
    SharedRuleState m_resultRuleState;
    unsigned m_pos; // position in text
    const std::string m_text;
    std::vector<TNotifyFunction> m_ruleStateNotifications;
    int m_termWaitingStatesIndex;
    std::array<TermWaitingStates, 2> m_termWaitingStates;
    bool m_debug;

    friend class AndOrCommon;
    friend class AndState;
    friend class OrState;
};

} // namespace synth
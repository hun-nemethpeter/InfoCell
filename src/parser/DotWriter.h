#pragma once
#include <set>
#include <sstream>

namespace synth {

class Parser;
class RuleState;

class DotWriter
{
public:
    DotWriter(const Parser* p_parser);
    bool Print(const RuleState* p_state);
    bool IsPrinted(const RuleState* p_state) const;

private:
    std::stringstream m_result;
    typedef std::set<const RuleState*> TPrintedStates;
    TPrintedStates m_printedStates;
};

} // namespace synth
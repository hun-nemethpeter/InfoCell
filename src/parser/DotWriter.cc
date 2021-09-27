#include <iomanip>
#include <fstream>

#include <parser/Parser.h>
#include <parser/RuleState.h>

#include "DotWriter.h"

namespace synth {

// =====================================================================================================================
DotWriter::DotWriter(const Parser* p_parser)
{
    m_result << "digraph structs {\n"
                "    node  [shape=plaintext]\n"
                "    graph [fontname = \"Helvetica\",\n"
                "           fontsize = 36,\n";
    if (p_parser->getPos() < p_parser->getText().size())
        m_result << "           label = \"Parse char:" << p_parser->getText()[p_parser->getPos()] << "\",\n";
    m_result << "          ];\n\n";

    p_parser->getResultRuleState()->dotWriterVisitor(*this);

    m_result << "}\n";

    std::stringstream fileName;
    fileName << "test_" << std::setfill('0') << std::setw(3) << p_parser->getPos() << ".dot";

    std::ofstream file;
    file.open(fileName.str().c_str());
    file << m_result.str();
}

// =====================================================================================================================
bool DotWriter::Print(const RuleState* p_state)
{
    if (IsPrinted(p_state))
        return false;

    m_printedStates.insert(p_state);
    m_result << p_state->printAsDot();

    return true;
}

// =====================================================================================================================
bool DotWriter::IsPrinted(const RuleState* p_state) const
{
    const TPrintedStates::const_iterator found = m_printedStates.find(p_state);
    if (found != m_printedStates.end())
        return true;

    return false;
}

} // namespace synth
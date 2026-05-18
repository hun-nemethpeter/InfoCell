#include "App.h"
#include <CLI/CLI.hpp>

namespace infocell {
namespace cli {

class Pos
{
public:
    int m_x;
    int m_y;
};

class ExternalEdge;
class InternalEdge;

class Shape
{
public:
    ExternalEdge* m_externalEdge;
    std::list<InternalEdge> m_internalEdges;
};

class InternalEdge
{
public:
    void insert(Shape& shape);
    Shape findShape(const Pos& pos);
};

class TestEdgeCommand
{
public:
    TestEdgeCommand()
    {
        std::cout << "Test edge!" << std::endl;
    }

};

App::App() :
    m_app(std::make_unique<CLI::App>("ARC-AGI Solver"))
{
    init();
}

App::~App()
{
}

void App::init()
{
}

int App::run(int argc, char* argv[])
{
    m_app->allow_non_standard_option_names(true);
    auto arcAGI1 = m_app->add_subcommand("arc-agi-1", "ARC-AGI-1 related commands");
    auto arcAGI2 = m_app->add_subcommand("arc-agi-2", "ARC-AGI-2 related commands");

    auto inputCommand = m_app->add_subcommand("input", "Input file(s)");
    inputCommand->allow_non_standard_option_names(true);
    bool hasEvalChallenges;
    bool hasEvalSolutions;
    bool hasTrainingChallenges;
    bool hasTrainingSolutions;
    bool hasTestChallenges;
    inputCommand->add_flag("-ec,--evalChallenges", hasEvalChallenges, "Include the evaluation challenges")->multi_option_policy(CLI::MultiOptionPolicy::Throw);
    inputCommand->add_flag("-es,--evalSolutions", hasEvalSolutions, "Include the evaluation solutions");
    inputCommand->add_flag("-tc,--trainingChallenges", hasTrainingChallenges, "Include the training challenges");
    inputCommand->add_flag("-ts,--trainingSolutions", hasTrainingSolutions, "Include the training solutions");
    inputCommand->add_flag("-tt,--testChallenges", hasTestChallenges, "Include the test challenges");
    std::string file;
    inputCommand->add_option("-tf,--task-file", file, "ARC-AGI task file")->check(CLI::ExistingFile);

    auto outputCommand = m_app->add_subcommand("output", "Output file");

    std::vector<std::string> taskIDs;
    m_app->add_option("-id,--filter-task-id", taskIDs, "task ID to process");
    m_app->add_option("-p,--print", file, "Print the selected task IDs");

    auto testEdgeCommand = m_app->add_subcommand("testEdge", "Test edge");
    testEdgeCommand->callback([&]() {
        TestEdgeCommand testEdgeCommand;
    });

    CLI11_PARSE(*m_app, argc, argv);

    return EXIT_SUCCESS;
}

} // namespace cli
} // namespace infocell
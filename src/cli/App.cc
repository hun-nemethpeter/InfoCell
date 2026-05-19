#include <filesystem>
#include <CLI/CLI.hpp>

#include "App.h"
#include "arc/EdgeDetector.h"
#include "arc/Task.h"
#include "config.h"
#include "util/Log.h"

using namespace infocell::cells;
namespace fs = std::filesystem;

namespace infocell {
namespace cli {

class CompareEdgesCommand
{
public:
    CompareEdgesCommand(cells::brain::Brain& kb, cells::arc::Grid& inputGrid, cells::arc::Grid& outputGrid) :
        kb(kb),
        m_inputGrid(inputGrid),
        m_outputGrid(outputGrid),
        m_inputEdges(kb),
        m_outputEdges(kb)
    {
        std::cout << "Compare edges!" << std::endl;
    }

    void start()
    {
        std::cout << "Start!" << std::endl;
        m_inputEdges.setInputGrid(m_inputGrid);
        m_inputEdges.detect();

        m_outputEdges.setInputGrid(m_outputGrid);
        m_outputEdges.detect();
    }

    cells::brain::Brain& kb;
    cells::arc::Grid& m_inputGrid;
    cells::arc::Grid& m_outputGrid;
    arc::EdgeDetector m_inputEdges;
    arc::EdgeDetector m_outputEdges;
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
    enum class PairKind : int
    {
        Undefined,
        Training,
        Test
    };
    std::map<std::string, PairKind> pairKindMap { { "training", PairKind::Training }, { "test", PairKind::Test } };
    enum class IoKind : int
    {
        Undefined,
        Input,
        Output
    };
    std::map<std::string, IoKind> ioKindMap { { "input", IoKind::Input }, { "output", IoKind::Output } };

    // ===== ARC-AGI-2
    bool agi1Mode = false;
    auto arcAGI1  = m_app->add_subcommand("arc-agi-1", "ARC-AGI-1 related commands")->callback([&]() { agi1Mode = true; });
    std::string arcAGI1DataFolderPathStr;
    auto arcAGI1DataFolderOption = arcAGI1->add_option("-f,--data-folder", arcAGI1DataFolderPathStr, "ARC-AGI-1 data folder")->check(CLI::ExistingDirectory);
    bool arcAGI1DefaultDataFolder = false;
    auto arcAGI1DefaultDataFolderOption = arcAGI1->add_flag("-d,--default-data-folder", arcAGI1DefaultDataFolder, "Use the default data folder")->multi_option_policy(CLI::MultiOptionPolicy::Throw);

    arcAGI1DataFolderOption->excludes(arcAGI1DefaultDataFolderOption);
    arcAGI1DefaultDataFolderOption->excludes(arcAGI1DataFolderOption);

    bool arc1EvaluationFolder = false;
    bool arc1TrainingFolder = false;
    arcAGI1->add_flag("-e,--evaluation", arc1EvaluationFolder, "Use the evaluation challenges folder")->multi_option_policy(CLI::MultiOptionPolicy::Throw);
    arcAGI1->add_flag("-t,--training", arc1TrainingFolder, "Use the training challenges folder")->multi_option_policy(CLI::MultiOptionPolicy::Throw);
    std::string arc1TaskId;
    arcAGI1->add_option("-i,--task-id", arc1TaskId, "ARC-AGI1 task id");

    // ===== ARC-AGI-2
    bool agi2Mode = false;
    auto arcAGI2  = m_app->add_subcommand("arc-agi-2", "ARC-AGI-2 related commands")->callback([&]() { agi2Mode = true; });
    arcAGI2->allow_non_standard_option_names(true);
    std::string arcAGI2DataFolderPathStr;
    auto arcAGI2DataFolderOption = arcAGI2->add_option("-df,--data-folder", arcAGI2DataFolderPathStr, "ARC-AGI-2 data folder")->check(CLI::ExistingDirectory);
    bool arc2DefaultDataFolder = false;
    auto arcAGI2DefaultDataFolderOption = arcAGI2->add_flag("-dd,--default-data-folder", arc2DefaultDataFolder, "Use the default data folder")->multi_option_policy(CLI::MultiOptionPolicy::Throw);

    arcAGI2DataFolderOption->excludes(arcAGI2DefaultDataFolderOption);
    arcAGI2DefaultDataFolderOption->excludes(arcAGI2DataFolderOption);

    bool hasFullEval           = false;
    bool hasFullTraining       = false;
    bool hasEvalChallenges     = false;
    bool hasEvalSolutions      = false;
    bool hasTrainingChallenges = false;
    bool hasTrainingSolutions  = false;
    bool hasTestChallenges     = false;
    arcAGI2->add_flag("-fe,--full-evaluation", hasFullEval, "Include the evaluation challenges with solutions")->multi_option_policy(CLI::MultiOptionPolicy::Throw);
    arcAGI2->add_flag("-ft,--full-training", hasFullTraining, "Include the training challenges with solutions")->multi_option_policy(CLI::MultiOptionPolicy::Throw);
    auto evalChallenegesOption = arcAGI2->add_flag("-ec,--eval-challenges", hasEvalChallenges, "Include the evaluation challenges")->multi_option_policy(CLI::MultiOptionPolicy::Throw);
    arcAGI2->add_flag("-es,--eval-solutions", hasEvalSolutions, "Include the evaluation solutions")->needs(evalChallenegesOption);
    auto trainingChallenegesOption = arcAGI2->add_flag("-tc,--training-challenges", hasTrainingChallenges, "Include the training challenges");
    arcAGI2->add_flag("-ts,--training-solutions", hasTrainingSolutions, "Include the training solutions")->needs(trainingChallenegesOption);
    arcAGI2->add_flag("-tt,--test-challenges", hasTestChallenges, "Include the test challenges");
    std::string arc2TaskId;
    arcAGI2->add_option("-i,--task-id", arc2TaskId, "ARC-AGI2 task id");

    // ===== debug-level
    auto debugCommand = m_app->add_subcommand("debug-level", "Debug level");

    // ===== compare-edges
    auto compareEdgeCommand = m_app->add_subcommand("compare-edges", "Solve an ARC-AGI task");

    auto inputGrid = compareEdgeCommand->add_subcommand("input", "input grid path");
    inputGrid->allow_non_standard_option_names(true);
    PairKind inputPairKind = PairKind::Undefined;
    inputGrid->add_option("-pk,--pair-kind", inputPairKind, "input pair kind (traing or test)")->transform(CLI::CheckedTransformer(pairKindMap, CLI::ignore_case));
    int inputPairIndex;
    inputGrid->add_option("-pi,--pair-index", inputPairIndex, "grid pair index inside the training or test tree (0..n)");
    IoKind inputIoKind = IoKind::Undefined;
    inputGrid->add_option("-ik,--io-kind", inputIoKind, "Input or output grid inside the pair (input or output)")->transform(CLI::CheckedTransformer(ioKindMap, CLI::ignore_case));

    auto outputGrid = compareEdgeCommand->add_subcommand("output", "output grid path");
    outputGrid->allow_non_standard_option_names(true);
    PairKind outputPairKind = PairKind::Undefined;
    outputGrid->add_option("-pk,--pair-kind", outputPairKind, "input pair kind (traing/test)")->transform(CLI::CheckedTransformer(pairKindMap, CLI::ignore_case));
    int outputPairIndex;
    outputGrid->add_option("-pi,--pair-index", outputPairIndex, "grid pair index inside the training or test tree (0..n)");
    IoKind outputIoKind = IoKind::Undefined;
    outputGrid->add_option("-ik,--io-kind", outputIoKind, "Input or output grid inside the pair (input or output)")->transform(CLI::CheckedTransformer(ioKindMap, CLI::ignore_case));

    compareEdgeCommand->callback([&]() {
        cells::brain::Brain kb([]() {
            brain::Brain::Logger::createLogger("edge");
            brain::Brain::Logger::createLogger("shapeCorners");
            brain::Brain::Logger::createLogger("shapeRelations");
            brain::Brain::Logger::createLogger("shapeIdGrid");
            brain::Brain::Logger::createLogger("grid");

            spdlog::get("cells")->set_level(spdlog::level::off);
            spdlog::get("compileStruct")->set_level(spdlog::level::off);
            spdlog::get("compiledSymbols")->set_level(spdlog::level::off);
            spdlog::get("edge")->set_level(spdlog::level::off);
            spdlog::get("shapeCorners")->set_level(spdlog::level::off);
            spdlog::get("shapeIdGrid")->set_level(spdlog::level::off);
            spdlog::get("grid")->set_level(spdlog::level::off);
            spdlog::get("shapeRelations")->set_level(spdlog::level::off);
        });

        std::string folderDir;
        std::string taskId;

        arc::Task* taskPtr = nullptr;
        std::unique_ptr<arc::Task> taskUniquePtr = nullptr;
        std::unique_ptr<arc::TaskSet> taskSetPtr = nullptr;
        if (agi1Mode) {
            fs::path rootDir;
            std::string taskFilePathStr;
            if (arcAGI1DefaultDataFolder) {
                rootDir = INFOCELL_ARC1_DATA_PATH;
            }
            if (!arcAGI1DataFolderPathStr.empty()) {
                rootDir = arcAGI1DataFolderPathStr;
            }
            if (arc1TrainingFolder) {
                folderDir = (rootDir / "training").make_preferred().string();
            }
            if (arc1EvaluationFolder) {
                folderDir = (rootDir / "evaluation").make_preferred().string();
            }
            if (!arc1TaskId.empty()) {
                taskId                = arc1TaskId;
                fs::path rootDir      = folderDir;
                fs::path taskFilePath = rootDir / fmt::format("{}.json", taskId);
                taskFilePathStr       = taskFilePath.make_preferred().string();
                std::cout << "File " << taskFilePathStr << " " << (fs::exists(taskFilePath) ? "exist" : "doesn't exist") << std::endl;
            }
            taskUniquePtr = std::make_unique<arc::Task>(kb, taskId, nlohmann::json::parse(std::ifstream(taskFilePathStr)));
        }
        if (agi2Mode) {
            fs::path rootDir;
            if (arc2DefaultDataFolder) {
                rootDir = INFOCELL_ARC2_DATA_PATH;
            }
            if (hasFullEval || hasEvalChallenges) {
                fs::path taskSetFilePath       = rootDir / INFOCELL_ARC_PRIZE_EVALUATION_CHALLENGES_FILENAME;
                std::string taskSetFilePathStr = taskSetFilePath.make_preferred().string();
                taskSetPtr                     = std::make_unique<arc::TaskSet>(kb, taskSetFilePathStr);
                if (hasFullEval) {
                    fs::path solutionPath       = rootDir / INFOCELL_ARC_PRIZE_EVALUATION_SOLUTIONS_FILENAME;
                    std::string solutionPathStr = solutionPath.make_preferred().string();
                    taskSetPtr->addSolutions(solutionPathStr);
                }
            }
            if (hasFullTraining || hasTrainingChallenges) {
                fs::path taskSetFilePath       = rootDir / INFOCELL_ARC_PRIZE_TRAINING_CHALLENGES_FILENAME;
                std::string taskSetFilePathStr = taskSetFilePath.make_preferred().string();
                taskSetPtr                     = std::make_unique<arc::TaskSet>(kb, taskSetFilePathStr);
                if (hasFullTraining) {
                    fs::path solutionPath       = rootDir / INFOCELL_ARC_PRIZE_TRAINING_SOLUTIONS_FILENAME;
                    std::string solutionPathStr = solutionPath.make_preferred().string();
                    taskSetPtr->addSolutions(solutionPathStr);
                }
            }
            if (!arc2TaskId.empty()) {
                taskId                     = arc2TaskId;
                arc::TaskSet::Tasks& tasks = taskSetPtr->m_tasks;

                auto findIter = tasks.find(taskId);
                if (findIter == tasks.end()) {
                    throw "No task found for this soltion, maybe wrong file?";
                }
                taskPtr = &findIter->second;
            }
        }
        arc::Task& task                     = taskUniquePtr ? *taskUniquePtr : *taskPtr;
        arc::Task::GridPair& inputGridPair  = (inputPairKind == PairKind::Training) ? task.m_examples[inputPairIndex] : task.m_tests[inputPairIndex];
        cells::arc::Grid& inputGrid         = (inputIoKind == IoKind::Input) ? inputGridPair.m_input : *inputGridPair.m_output;
        arc::Task::GridPair& outputGridPair = (outputPairKind == PairKind::Training) ? task.m_examples[outputPairIndex] : task.m_tests[outputPairIndex];
        cells::arc::Grid& outputGrid        = (outputIoKind == IoKind::Input) ? outputGridPair.m_input : *outputGridPair.m_output;
        CompareEdgesCommand compareEdgesCommand(kb, inputGrid, outputGrid);
        compareEdgesCommand.start();
        std::cout << "Finished!" << std::endl;
    });

    CLI11_PARSE(*m_app, argc, argv);

    return EXIT_SUCCESS;
}

} // namespace cli
} // namespace infocell
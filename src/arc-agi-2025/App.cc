#include "App.h"
#include <CLI/CLI.hpp>

namespace infocell {
namespace arc2025 {

App::App() :
    m_cliApp(std::make_unique<CLI::App>("ARC-AGI-2025 Solver"))
{
    init();
}

App::~App()
{
}

void App::init()
{
    m_cliApp->allow_non_standard_option_names(true);
}

int App::run(int argc, char* argv[])
{
    std::string file;
    m_cliApp->add_option("-ft,--task-file", file, "ARC-AGI task file")->check(CLI::ExistingFile);
    m_cliApp->add_option("-fc,--challenges-file", file, "ARC-AGI kaggle challenges file")->check(CLI::ExistingFile);
    m_cliApp->add_option("-fs,--solutions-file", file, "ARC-AGI kaggle solutions file")->check(CLI::ExistingFile);
    std::vector<std::string> taskIDs;
    m_cliApp->add_option("-id,--filter-task-id", taskIDs, "task ID to process");
    m_cliApp->add_option("-p,--print", file, "Print the selected task IDs");

    CLI11_PARSE(*m_cliApp, argc, argv);

    return EXIT_SUCCESS;
}

} // namespace arc2025
} // namespace infocell
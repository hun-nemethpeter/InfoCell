#pragma once

#include <memory>

namespace CLI {
class App;
}

namespace infocell {
namespace cli {

class App
{
public:
    App();
    virtual ~App();

    void init();
    int run(int argc, char* argv[]);

private:
    std::unique_ptr<CLI::App> m_app;
};

} // namespace cli
} // namespace infocell
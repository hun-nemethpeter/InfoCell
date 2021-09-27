#include "App.h"

namespace synth {

void App::init(int argc, char* argv[])
{
    if (argc == 1) {
        arcDb.load("F:\\Devel\\ARC\\ARC\\data\\training\\007bbfb7.json");
    } else {
        arcDb.load(argv[1]);
    }
}

void App::run()
{
}

} // namespace synth
#include "ArcDb.h"

namespace synth {

class App
{
public:
    void init(int argc, char* argv[]);
    void run();

    ArcDb arcDb;
};

} // namespace synth
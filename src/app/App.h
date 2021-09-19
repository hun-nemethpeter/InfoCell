#include "ArcDb.h"

namespace arcsolver {

class App
{
public:
    void init(int argc, char* argv[]);
    void run();

    ArcDb arcDb;
};

} // namespace arcsolver
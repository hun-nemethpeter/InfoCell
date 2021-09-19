#include "app/App.h"

int main(int argc, char* argv[])
{
    arcsolver::App app;
    app.init(argc, argv);
    app.run();

	return 0;
}

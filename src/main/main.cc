#include "tui/App.h"

int main(int argc, char* argv[])
{
    infocell::tui::App app;
    app.init(argc, argv);
    app.run();

	return 0;
}

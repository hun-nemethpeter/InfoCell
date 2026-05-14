#include <cstdlib>
#include <iostream>
#include <fstream>
#include <string>

int main(int argc, char* argv[])
{
    std::string path;
    std::cout << "Executes: " << path << std::endl;
    std::ofstream fs("svg-covert.log");
    fs << "argc: " << argc << "\n";
    for (int i = 0; i < argc; i++) {
        fs << "argv[" << i << "]: " << argv[i] << "\n";
    }
    std::string input = argv[9];
    std::string output = argv[10];

    path = "\"C:\\Program Files\\Inkscape\\bin\\inkscape.exe\" --export-filename=";
    path += input;
    path += " ";
    path += output;
    fs << path << "\n";

//    std::system("\"C:\\Program Files\\Inkscape\\bin\\inkscape.exe\" --export-filename=test.pdf doc\\diagrams\\SampleCell.svg");
    std::system(path.c_str());

    return 0;
}
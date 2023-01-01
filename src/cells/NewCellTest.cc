#include "NewCells.h"

using namespace synth::newcell;
using namespace synth::newcell::cells;

int main(int argc, char* argv[])
{
    StaticInitializations();

    CellValuePrinter valuePrinter;
    CellStructPrinter structPrinter;

    Object colorRed(Type::anyType());
    Object colorGreen(Type::anyType());
    Object colorBlue(Type::anyType());

    Type colorClass("Color",
                    { { "red", Number::t(), colorRed },
                      { "green", Number::t(), colorGreen },
                      { "blue", Number::t(), colorBlue } });

    Object redColor("redColor", colorClass);
    redColor.set(colorRed, Numbers::get(255));
    redColor.set(colorGreen, Numbers::get(0));
    redColor.set(colorBlue, Numbers::get(0));

    Number& number_0   = Numbers::get(0);
    Number& number_255 = Numbers::get(255);

    std::cout << colorClass.printAs(valuePrinter) << std::endl;
    std::cout << colorClass.getSlot("red").printAs(valuePrinter) << std::endl;
    std::cout << redColor.printAs(valuePrinter) << std::endl;
    std::cout << number_255.printAs(valuePrinter) << std::endl;
    std::cout << number_255[value][first][value].printAs(valuePrinter) << std::endl;

    std::cout << redColor.printAs(structPrinter) << std::endl;
    std::cout << colorClass.getSlot("red").printAs(structPrinter) << std::endl;
    std::cout << colorClass.printAs(structPrinter) << std::endl;
    std::cout << number_255.printAs(structPrinter) << std::endl;
    std::cout << number_255[sign].printAs(structPrinter) << std::endl;
    std::cout << number_255[value].printAs(structPrinter) << std::endl;
    std::cout << number_255[value][first].printAs(structPrinter) << std::endl;
    std::cout << number_255[value][first][value].printAs(structPrinter) << std::endl;
    std::cout << number_255[value][last].printAs(structPrinter) << std::endl;
    std::cout << number_255[value][size].printAs(structPrinter) << std::endl;
}
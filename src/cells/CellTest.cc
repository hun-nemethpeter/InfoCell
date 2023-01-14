#include "Cells.h"
#include "SVGPrinter.h"
#include <fstream>

using namespace synth;
using namespace synth::cells;
using namespace synth::cells::data;

void printAsValue(CellI& cell)
{
    CellValuePrinter valuePrinter;
    cell.accept(valuePrinter);

    std::cout << valuePrinter.print() << std::endl;
}

void printAsStruct(CellI& cell)
{
    CellStructPrinter structPrinter;
    cell.accept(structPrinter);

    std::cout << structPrinter.print() << std::endl;
}

int main(int argc, char* argv[])
{
    svg::Printer svgPrinter(800, 600);

    StaticInitializations();

    input::Picture inputPicture("input");
    inputPicture.loadFromJsonArray("[[0, 7, 0], [7, 7, 7], [0, 7, 0]]");
    hybrid::Sensor sensor(inputPicture);

    sensor.accept(svgPrinter);
//    sensor[data::listOfPixels].accept(svgPrinter);
    svgPrinter.writeFile("F:\\Devel\\ARC\\synth\\1.svg");


    Type Variable("Color",
                  { { "name", String::t(), data::coding::name },
                    { "value", Number::t(), data::coding::value } });
    Object var1("var1", Variable);

    control::pipeline::Node pipeNode1;
    control::Same sameOp(sensor, sensor, pipeNode1, data::coding::value);
    pipeNode1();
    std::cout << "SameOp: ";
    printAsValue(var1[data::coding::value]);
    printAsValue(sensor[data::listOfPixels]);


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

    printAsValue(colorClass);
    printAsValue(colorClass.getSlot("red"));
    printAsValue(redColor);
    printAsValue(number_255);
    printAsValue(number_255[value][first][value]);

    printAsStruct(redColor);
    printAsStruct(colorClass.getSlot("red"));
    printAsStruct(colorClass);
    printAsStruct(number_255);
    printAsStruct(number_255[sign]);
    printAsStruct(number_255[value]);
    printAsStruct(number_255[value][first]);
    printAsStruct(number_255[value][first][value]);
    printAsStruct(number_255[value][last]);
    printAsStruct(number_255[value][size]);
}
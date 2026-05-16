#include <array>
#include "Color.h"

namespace infocell {
namespace arc {

#if 0
ARC-AGI-2026 colors

ID |  Name   | Hexadecimal RGB  | Decimal RGB
---+---------+------------------+--------------
0  | black   | 0x00, 0x00, 0x00 |   0,   0,   0
1  | blue    | 0x00, 0x74, 0xD9 |   0, 116, 217
2  | red     | 0xFF, 0x41, 0x36 | 255,  65,  54
3  | green   | 0x2E, 0xCC, 0x40 |  46, 204,  64
4  | yellow  | 0xFF, 0xDC, 0x00 | 255, 220,   0
5  | grey    | 0xAA, 0xAA, 0xAA | 170, 170, 170
6  | fuschia | 0xF0, 0x12, 0xBE | 240,  18, 190
7  | orange  | 0xFF, 0x85, 0x1B | 255, 133,  27
8  | teal    | 0x7F, 0xDB, 0xFF | 127, 219, 255
9  | brown   | 0x87, 0x0C, 0x25 | 135,  12,  37
#endif

static const std::array<Color, 10> colorObjects = {
    Color(ArcColor::black, 0x00, 0x00, 0x00),
    Color(ArcColor::blue, 0x00, 0x74, 0xD9),
    Color(ArcColor::red, 0xFF, 0x41, 0x36),
    Color(ArcColor::green, 0x2E, 0xCC, 0x40),
    Color(ArcColor::yellow, 0xFF, 0xDC, 0x00),
    Color(ArcColor::grey, 0xAA, 0xAA, 0xAA),
    Color(ArcColor::fuschia, 0xF0, 0x12, 0xBE),
    Color(ArcColor::orange, 0xFF, 0x85, 0x1B),
    Color(ArcColor::teal, 0x7F, 0xDB, 0xFF),
    Color(ArcColor::brown, 0x87, 0x0C, 0x25)
};

std::string getArcColorName(ArcColor arcColor)
{
    static std::array<std::string, 10> arcColorNames = {
        std::string("black"),
        std::string("blue"),
        std::string("red"),
        std::string("green"),
        std::string("yellow"),
        std::string("grey"),
        std::string("fuschia"),
        std::string("orange"),
        std::string("teal"),
        std::string("brown")
      };

    return arcColorNames[(int)arcColor];
}

Color colors(int arcColor)
{
    return colorObjects[arcColor];
}

Color colors(ArcColor arcColor)
{
    return colorObjects[(int)arcColor];
}

} // namespace arc
} // namespace infocell

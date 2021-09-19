#include <cells/Cells.h>

using cells::Item;

// =====================================================================================================================
int main()
{

    cells::Sensor sensor;
    // 8K     is 7680x4320 = 33 177 600
    // 4K     is 3840x2160 =  8 294 400
    // FullHD is 1920x1080 =  2 073 600
    // HD720  is 1280x720  =    921 600
    // SVGA   is  800x600  =    480 000
    // VGA    is  640x480  =    307 200
    // CGA    is  320x200  =     64 000
    sensor.width(320).height(200).init();

    cells::Pixel pixel;
    cells::Pixel pixel2;
    pixel.up = &pixel2;
    std::cout << toString(pixel) << std::endl;
    std::cout << toString(sensor.pixels[0]) << std::endl;

    cells::Rule start;
    cells::Rule line;
    line << Item({ cells::members::self, cells::types::Pixel }) << Item({ cells::members::directions::up, cells::types::Pixel });
}

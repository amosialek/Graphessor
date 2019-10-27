#ifndef __PIXEL_HPP__
#define __PIXEL_HPP__

#include <string>
class Pixel
{

    public:
        int r, g, b, x, y;
        std::string label;
        bool _break;

    Pixel(int x, int y, int r, int g, int b);
    Pixel(int x, int y, std::string label);
};
#endif // __PIXEL_HPP__
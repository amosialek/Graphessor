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
        Pixel();
        ~Pixel();
        Pixel(const Pixel& other);
        Pixel & operator= ( Pixel other ) 
        {
            this->x=other.x;
            this->y=other.y;
            this->r=other.r;
            this->g=other.g;
            this->b=other.b;
            this->_break=other._break;
            this->label=other.label;
        };
};
#endif // __PIXEL_HPP__
#include "Pixel.hpp"
#include <iostream>
Pixel::Pixel(int x, int y, int r, int g, int b)
{
    this->x=x;
    this->y=y;
    this->r=r;
    this->g=g;
    this->b=b;
    this->_break=0;
    this->label='P';
}

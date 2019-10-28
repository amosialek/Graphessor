#include "Pixel.hpp"
//#include <iostream>
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

Pixel::Pixel(int x, int y, std::string label)
{
    this->x=x;
    this->y=y;
    this->r=this->g=this->b=-1;
    this->_break=0;
    this->label=label;
}

Pixel::Pixel()
{
    this->x=this->y=-1;
    this->r=this->g=this->b=-1;
    //std::cout<<"created invalid Pixel"<<std::endl;
    this->label="invalid";
}

Pixel::Pixel(const Pixel& other)
{
    this->x=other.x;
    this->y=other.y;
    this->r=other.r;
    this->g=other.g;
    this->b=other.b;
    this->_break=other._break;
    this->label=other.label;
    //std::cout<<"copied Pixel"<<std::endl;
}

Pixel::~Pixel(){}
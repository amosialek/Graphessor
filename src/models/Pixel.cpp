#include "Pixel.hpp"
#include "GraphessorConstants.hpp"

Pixel::Pixel(int x, int y, int r, int g, int b)
{
    this->x=x;
    this->y=y;
    this->r=r;
    this->g=g;
    this->b=b;
    this->_break = 0;
    this->breakLevel=0;
    this->label = NODELABEL_P;
    this->error = -1;
}

Pixel::Pixel(int x, int y, std::string label)
{
    this->x=x;
    this->y=y;
    this->r=this->g=this->b=-1;
    this->_break=0;
    this->label=label;
    this->breakLevel=0;
    this->error = -1;
}

Pixel::Pixel()
{
    this->x=this->y=-1;
    this->r=this->g=this->b=-1;
    //std::cout<<"created invalid Pixel"<<std::endl;
    this->label="invalid";
    this->error = -1;
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
    this->breakLevel=other.breakLevel;
    this->error = other.error;
    this->attributes = other.attributes;
}

Pixel& Pixel::operator=(const Pixel& other)
{
    this->x=other.x;
    this->y=other.y;
    this->r=other.r;
    this->g=other.g;
    this->b=other.b;
    this->_break=other._break;
    this->label=other.label;
    this->breakLevel=other.breakLevel;
    this->error = other.error;
    this->attributes = other.attributes;
    return *this;
}

Pixel::~Pixel(){}
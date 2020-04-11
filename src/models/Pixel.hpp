#ifndef __PIXEL_HPP__
#define __PIXEL_HPP__

#include <string>
class Pixel
{

    public:
        int r, g, b, x, y, breakLevel;
        double error;
        std::string label;
        bool _break, visited;

        Pixel(int x, int y, int r, int g, int b);
        Pixel(int x, int y, std::string label);
        Pixel();
        ~Pixel();
        Pixel(const Pixel& other);
        Pixel& operator=(const Pixel& other); 
        
};
#endif // __PIXEL_HPP__
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
inline
bool operator<(const Pixel& p1, const Pixel& p2)
        {
            if(p1.x<p2.x)
                return true;
            return p1.y<p2.y;

        }
#endif // __PIXEL_HPP__
#ifndef __PIXEL_HPP__
#define __PIXEL_HPP__

#include <string>
#include "Attributes.hpp"
#include <memory>
#include <cereal/types/memory.hpp>
#include <cereal/archives/json.hpp>

class Pixel
{

    public:
        int r, g, b, x, y, breakLevel;
        double error;
        std::string label;
        bool _break, visited;
        std::shared_ptr<Attributes> attributes;

        Pixel(int x, int y, int r, int g, int b);
        Pixel(int x, int y, std::string label);
        Pixel();
        ~Pixel();
        Pixel(const Pixel& other);
        Pixel& operator=(const Pixel& other); 
        
        template<class Archive>
        void serialize(Archive & archive)
        {
            archive( CEREAL_NVP(x), CEREAL_NVP(y), CEREAL_NVP(r), CEREAL_NVP(g), CEREAL_NVP(b), CEREAL_NVP(label), CEREAL_NVP(error), CEREAL_NVP(_break), CEREAL_NVP(visited), CEREAL_NVP(breakLevel), CEREAL_NVP(attributes) ); // serialize things by passing them to the archive
        }
};

inline
bool operator<(const Pixel& p1, const Pixel& p2)
        {
            if(p1.x < p2.x)
                return true;
            return p1.x == p2.x and p1.y < p2.y;
        }
#endif // __PIXEL_HPP__
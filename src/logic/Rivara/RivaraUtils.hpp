#ifndef __RIVARAUTILS_HPP__
#define __RIVARAUTILS_HPP__
#include <math.h>
#include "Pixel.hpp"
#include "RivaraAttributes.hpp"

namespace Rivara
{
    inline double NL(double x1, double y1, double x2, double y2)
    {
        return std::sqrt((x2-x1)*(x2-x1)+(y2-y1)*(y2-y1));
    }

    inline double NL(Pixel p1, Pixel p2)
    {
        return NL(
            p1.attributes -> GetDouble(RIVARA_ATTRIBUTE_X),
            p1.attributes -> GetDouble(RIVARA_ATTRIBUTE_Y),
            p2.attributes -> GetDouble(RIVARA_ATTRIBUTE_X),
            p2.attributes -> GetDouble(RIVARA_ATTRIBUTE_Y));
    }

    inline double NL(double x1, double y1, double x2, double y2, double x3, double y3)
    {
        return std::sqrt(((x1+x2)/2.0-x3)*((x1+x2)/2.0-x3) + ((y1+y2)/2.0-y3)*((y1+y2)/2.0-y3));
    }

    inline double NL(Pixel p1, Pixel p2, Pixel p3)
    {
        return NL(
            p1.attributes -> GetDouble(RIVARA_ATTRIBUTE_X),
            p1.attributes -> GetDouble(RIVARA_ATTRIBUTE_Y),
            p2.attributes -> GetDouble(RIVARA_ATTRIBUTE_X),
            p2.attributes -> GetDouble(RIVARA_ATTRIBUTE_Y), 
            p3.attributes -> GetDouble(RIVARA_ATTRIBUTE_X), 
            p3.attributes -> GetDouble(RIVARA_ATTRIBUTE_Y));
    }

    inline bool EdgeComparator(const Pixel& lhs, const Pixel& rhs)
    {
        return lhs.attributes->GetDouble(RIVARA_ATTRIBUTE_L)<rhs.attributes->GetDouble(RIVARA_ATTRIBUTE_L);
    }
}
#endif // __RIVARAUTILS_HPP__

#ifndef __RIVARAUTILS_HPP__
#define __RIVARAUTILS_HPP__
#include <math.h>
#include "Pixel.hpp"
#include "RivaraAttributes.hpp"

namespace Rivara
{
    double NL(int x1, int y1, int x2, int y2)
    {
        return std::sqrt((x2-x1)*(x2-x1)+(y2-y1)*(y2-y1));
    }

    double NL(Pixel p1, Pixel p2)
    {
        return NL(p1.x, p1.y, p2.x, p2.y);
    }

    double NL(int x1, int y1, int x2, int y2, int x3, int y3)
    {
        return std::sqrt(((x1+x2)/2.0-x3)*((x1+x2)/2.0-x3) + ((y1+y2)/2.0-y3)*((y1+y2)/2.0-y3));
    }

    double NL(Pixel p1, Pixel p2, Pixel p3)
    {
        return NL(p1.x, p1.y, p2.x, p2.y, p3.x, p3.y);
    }

    bool EdgeComparator(const Pixel& lhs, const Pixel& rhs)
    {
        return lhs.attributes->GetDouble(RIVARA_ATTRIBUTE_L)<rhs.attributes->GetDouble(RIVARA_ATTRIBUTE_L);
    }
}
#endif // __RIVARAUTILS_HPP__

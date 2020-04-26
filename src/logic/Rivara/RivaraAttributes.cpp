#include "RivaraAttributes.hpp"
#include <cassert>

using namespace Rivara;

bool RivaraAttributes::GetBool(char attributeName)
{
    switch(attributeName)
    {
        case 'H': return HN;
        case 'B': return B;
        case 'R': return R;
    }
    assert(false && "Bool attribute not found:" + attributeName);
}

double RivaraAttributes::GetDouble(char attributeName)
{
    switch(attributeName)
    {
        case 'L': return L;
    }
    assert(false && "Double attribute not found:" + attributeName);
}

void RivaraAttributes::SetBool(char attributeName, bool value)
{
    switch(attributeName)
    {
        case 'H': HN = value; break;
        case 'B': B = value; break;
        case 'R': R = value; break;
    }
    assert(false && "Bool attribute not found:" + attributeName);
}

void RivaraAttributes::SetDouble(char attributeName, double value)
{
    switch(attributeName)
    {
        case 'L': L = value; break;
    }
    assert(false && "Double attribute not found:" + attributeName);
}
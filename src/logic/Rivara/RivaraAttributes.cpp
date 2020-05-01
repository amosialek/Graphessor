#include "RivaraAttributes.hpp"
#include <cassert>

using namespace Rivara;

bool RivaraAttributes::GetBool(char attributeName)
{
    switch(attributeName)
    {
        case RIVARA_ATTRIBUTE_HN: return HN;
        case RIVARA_ATTRIBUTE_B: return B;
        case RIVARA_ATTRIBUTE_R: return R;
    }
    assert(false && "Bool attribute not found:" + attributeName);
}

double RivaraAttributes::GetDouble(char attributeName)
{
    switch(attributeName)
    {
        case RIVARA_ATTRIBUTE_L: return L;
        case RIVARA_ATTRIBUTE_X: return X;
        case RIVARA_ATTRIBUTE_Y: return Y;
    }
    assert(false && "Double attribute not found:" + attributeName);
}

void RivaraAttributes::SetBool(char attributeName, bool value)
{
    switch(attributeName)
    {
        case RIVARA_ATTRIBUTE_HN: HN = value; return;
        case RIVARA_ATTRIBUTE_B: B = value; return;
        case RIVARA_ATTRIBUTE_R: R = value; return;
    }
    assert(false && "Bool attribute not found:" + attributeName);
}

void RivaraAttributes::SetDouble(char attributeName, double value)
{
    switch(attributeName)
    {
        case RIVARA_ATTRIBUTE_L: L = value; return;
        case RIVARA_ATTRIBUTE_X: X = value; return;
        case RIVARA_ATTRIBUTE_Y: Y = value; return;
    }
    assert(false && "Double attribute not found:" + attributeName);
}
#include "VertexBase.hpp"
class Pixel : public VertexBase
{

    public:
        int r, g, b;

    Pixel(int x, int y, int r, int g, int b);
};
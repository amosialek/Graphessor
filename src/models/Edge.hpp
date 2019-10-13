#include "Pixel.hpp"
#include <string>
#include <vector>
class Edge : public VertexBase
{

    public:
        bool shouldBreak=0;
        std::string label;
        
    Edge(int x, int y, std::string label, std::vector<Pixel> neighbours);
};
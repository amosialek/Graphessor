#include "Edge.hpp"
#include <vector>
#include <string>
Edge::Edge(int x, int y, std::string label, std::vector<Pixel> neighbours)
{
    this->x=x;
    this->y=y;
    this->label = label;
}

#include "P1.hpp"

P1::P1(std::shared_ptr<IGraph> graph,
        vertex_descriptor S,
        std::shared_ptr<Image> image) 
        :
             graph(graph), 
             S(S),
             image(image)
    {}

void P1::Perform()
{
    throw NotImplementedException();
}

std::vector<uint8_t> P1::Serialize()
{
    throw NotImplementedException();
}
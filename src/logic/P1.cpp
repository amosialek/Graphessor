#include "P1.hpp"

P1::P1(std::shared_ptr<IGraph> graph,
        vertex_descriptor S,
        std::shared_ptr<Image> image) 
        :
             graph(graph), 
             S(S),
             image(image)
    {};

void P1::Perform()
{
    spdlog::debug("P1 {}",S);    
    int y = image->height()-1,
        x = image->width()-1,
        r,g,b;
    auto I = S;
    (*graph)[I].x=x/2;
    (*graph)[I].y=y/2;
    graph->ChangeVertexType(I,NODELABEL_I);

    auto BTop = graph -> AddVertex(*(new Pixel(x/2, 0, NODELABEL_B)));
    auto BLeft = graph -> AddVertex(*(new Pixel(0, y/2, NODELABEL_B)));
    auto BBot = graph -> AddVertex(*(new Pixel(x/2, y, NODELABEL_B)));
    auto BRight = graph -> AddVertex(*(new Pixel(x, y/2, NODELABEL_B)));
    
    std::tie(r,g,b) = image->getPixel(0,0);
    auto PTopLeft = graph -> AddVertex(*(new Pixel(0, 0, r,g,b)));
    std::tie(r,g,b) = image->getPixel(x, 0);
    auto PTopRight = graph -> AddVertex(*(new Pixel(x, 0, r,g,b)));
    std::tie(r,g,b) = image->getPixel(x, y);
    auto PBotRight = graph -> AddVertex(*(new Pixel(x, y, r,g,b)));
    std::tie(r,g,b) = image->getPixel(0, y);
    auto PBotLeft = graph -> AddVertex(*(new Pixel(0, y, r,g,b)));

    graph -> AddEdge(PBotLeft, I);
    graph -> AddEdge(PBotRight, I);
    graph -> AddEdge(PTopLeft, I);
    graph -> AddEdge(PTopRight, I);

    graph -> AddEdge(PBotLeft, BBot);
    graph -> AddEdge(PBotRight, BBot);
    graph -> AddEdge(PTopLeft, BTop);
    graph -> AddEdge(PTopRight, BTop);

    graph -> AddEdge(PBotLeft, BLeft);
    graph -> AddEdge(PTopLeft, BLeft);
    graph -> AddEdge(PBotRight, BRight);
    graph -> AddEdge(PTopRight, BRight);
}

std::vector<uint8_t> P1::Serialize()
{
    throw NotImplementedException();
}
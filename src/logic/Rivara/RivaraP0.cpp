#include "RivaraP0.hpp"
#include "RivaraAttributes.hpp"

using namespace Rivara;

RivaraP0::RivaraP0(std::shared_ptr<IGraph> graph,
        vertex_descriptor S,
        std::shared_ptr<Image> image) 
        :
             graph(graph), 
             S(S),
             image(image)
    {}

void RivaraP0::Perform()
{
    spdlog::debug("Rivara P0 {}",S);
    Pixel n1P, n2P, e1P, e2P, e3P, tP;
    n1P.attributes = std::make_shared<RivaraAttributes>(),
    n2P.attributes = std::make_shared<RivaraAttributes>(),
    (*graph)[S].attributes = std::make_shared<RivaraAttributes>(),
    e1P.attributes = std::make_shared<RivaraAttributes>(),
    e2P.attributes = std::make_shared<RivaraAttributes>(),
    e3P.attributes = std::make_shared<RivaraAttributes>(),
    tP.attributes = std::make_shared<RivaraAttributes>();
    int topPixelHeight = image -> height() - 1;
    int rightPixelWidth = image -> width() - 1;
    (*graph)[S].attributes -> SetDouble(RIVARA_ATTRIBUTE_X, rightPixelWidth / 2); 
    (*graph)[S].attributes -> SetDouble(RIVARA_ATTRIBUTE_Y, 0);
    n1P.attributes -> SetDouble(RIVARA_ATTRIBUTE_X, 0);
    n1P.attributes -> SetDouble(RIVARA_ATTRIBUTE_Y, topPixelHeight);
    n2P.attributes -> SetDouble(RIVARA_ATTRIBUTE_X, rightPixelWidth);
    n2P.attributes -> SetDouble(RIVARA_ATTRIBUTE_Y, topPixelHeight);
    (*graph)[S].x = rightPixelWidth / 2; 
    (*graph)[S].y = 0;
    n1P.x = 0;
    n1P.y = topPixelHeight;
    n2P.x = rightPixelWidth;
    n2P.y = topPixelHeight;
    std::tie((*graph)[S].r, (*graph)[S].g, (*graph)[S].b) = image -> getPixel(rightPixelWidth / 2, 0);
    (*graph)[S].attributes -> SetBool(RIVARA_ATTRIBUTE_HN, false);
    graph -> ChangeVertexType(S, NODELABEL_N);
    n1P.label = NODELABEL_N;
    n2P.label = NODELABEL_N;
    e1P.label = e2P.label = e3P.label = NODELABEL_E;
    e1P.attributes -> SetBool(RIVARA_ATTRIBUTE_B, true);
    e2P.attributes -> SetBool(RIVARA_ATTRIBUTE_B, true);
    e3P.attributes -> SetBool(RIVARA_ATTRIBUTE_B, true);
    e1P.attributes -> SetDouble(RIVARA_ATTRIBUTE_L, NL((*graph)[S], n1P));
    e2P.attributes -> SetDouble(RIVARA_ATTRIBUTE_L, NL(n1P, n2P));
    e3P.attributes -> SetDouble(RIVARA_ATTRIBUTE_L, NL((*graph)[S], n2P));
    e1P.x = (n1P.x + (*graph)[S].x)/2;
    e1P.y = (n1P.y + (*graph)[S].y)/2;
    e2P.x = (n1P.x + n2P.x)/2;
    e2P.y = (n1P.y + n2P.y)/2;
    e3P.x = (n2P.x + (*graph)[S].x)/2;
    e3P.y = (n2P.y + (*graph)[S].y)/2;
    tP.label = NODELABEL_T;
    tP.x = (n1P.x + n2P.x + (*graph)[S].x)/3;
    tP.y = (n1P.y + n2P.y + (*graph)[S].y)/3;
    std::tie(n1P.r, n1P.g, n1P.b) = image -> getPixel(0, topPixelHeight);
    std::tie(n2P.r, n2P.g, n2P.b) = image -> getPixel(rightPixelWidth, topPixelHeight);
    auto n1 = graph -> AddVertex(n1P);
    auto n2 = graph -> AddVertex(n2P);
    auto e1 = graph -> AddVertex(e1P);
    auto e2 = graph -> AddVertex(e2P);
    auto e3 = graph -> AddVertex(e3P);
    auto t = graph -> AddVertex(tP);
    graph -> AddEdge(S, e1);
    graph -> AddEdge(n1, e1);
    graph -> AddEdge(n1, e2);
    graph -> AddEdge(n2, e2);
    graph -> AddEdge(n2, e3);
    graph -> AddEdge(S, e3);
    graph -> AddEdge(S, t);
    graph -> AddEdge(n1, t);
    graph -> AddEdge(n2, t);
}

std::vector<uint8_t> RivaraP0::Serialize()
{
    throw NotImplementedException();
}
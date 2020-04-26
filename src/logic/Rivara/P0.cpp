#include "P0.hpp"
#include "RivaraAttributes.hpp"

using namespace Rivara;

P0::P0(std::shared_ptr<IGraph> graph,
        vertex_descriptor S,
        std::shared_ptr<Image> image) 
        :
             graph(graph), 
             S(S),
             image(image)
    {}

void P0::Perform()
{
    Pixel n1P, n2P, n3P, e1P, e2P, e3P, tP;
    int topPixelHeight = image -> height() - 1;
    int rightPixelWidth = image -> width() - 1;
    n3P = (*graph)[S];
    n3P.x = rightPixelWidth / 2; 
    n3P.y = 0;
    n1P.x = 0;
    n1P.y = topPixelHeight;
    n1P.x = rightPixelWidth;
    n1P.y = topPixelHeight;
    std::tie(n3P.r, n3P.g, n3P.b) = image -> getPixel(rightPixelWidth / 2, 0);
    n3P.attributes -> SetBool(RIVARA_ATTRIBUTE_HN, false);
    graph -> ChangeVertexType(S, NODELABEL_N);
    n1P.label = NODELABEL_N;
    n2P.label = NODELABEL_N;
    e1P.label = e2P.label = e3P.label = NODELABEL_E;
    e1P.attributes -> SetBool(RIVARA_ATTRIBUTE_B, true);
    e2P.attributes -> SetBool(RIVARA_ATTRIBUTE_B, true);
    e3P.attributes -> SetBool(RIVARA_ATTRIBUTE_B, true);
    e1P.attributes -> SetDouble(RIVARA_ATTRIBUTE_L, NL(n3P, n1P));
    e2P.attributes -> SetDouble(RIVARA_ATTRIBUTE_L, NL(n1P, n2P));
    e3P.attributes -> SetDouble(RIVARA_ATTRIBUTE_L, NL(n3P, n2P));
    tP.label = NODELABEL_T;
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
    graph -> AddEdge(S, t);
    graph -> AddEdge(S, t);
}

std::vector<uint8_t> P0::Serialize()
{
    throw NotImplementedException();
}
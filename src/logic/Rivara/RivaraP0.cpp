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

///
/// TEST DOT
/// \dot
/// graph RivaraP0{
/// 0[label="N1"
/// pos="0,0!"];
/// 1[label="N2" 
/// pos="4,0!"];
/// 2[label="N3"
/// pos="2,4!"];
/// 3[label="N4" 
/// pos="0,4!"];
/// 4[label="N5" 
/// pos="4,4!"];
/// 5[label="E1" 
/// pos="1,2!"];
/// 6[label="E2" 
/// pos="2,0!"];
/// 7[label="E3" 
/// pos="3,2!"];
/// 8[label="E4" 
/// pos="0,2!"];
/// 9[label="E5" 
/// pos="1,4!"];
/// 10[label="E6" 
/// pos="3,4!"];
/// 11[label="E7" 
/// pos="4,2!"];
/// 12[label="T1" 
/// pos="2,2!"];
/// 13[label="T2" 
/// pos="1,3!"];
/// 14[label="T3" 
/// pos="3,3!"];
/// 0--5 ;
/// 2--5 ;
/// 0--6 ;
/// 1--6 ;
/// 1--7 ;
/// 2--7 ;
/// 0--8 ;
/// 3--8 ;
/// 2--9 ;
/// 3--9 ;
/// 2--10 ;
/// 4--10 ;
/// 1--11 ;
/// 4--11 ;
/// 0--12 ;
/// 1--12 ;
/// 2--12 ;
/// }
/// \enddot
///
void RivaraP0::Perform()
{
    spdlog::debug("Rivara P0 {}",S);
    Pixel n1P, n2P, n4P, n5P, e1P, e2P, e3P, e4P, e5P, e6P, e7P, t1P, t2P, t3P;
    n1P.attributes = std::make_shared<RivaraAttributes>(),
    n2P.attributes = std::make_shared<RivaraAttributes>(),
    n4P.attributes = std::make_shared<RivaraAttributes>(),
    n5P.attributes = std::make_shared<RivaraAttributes>(),
    (*graph)[S].attributes = std::make_shared<RivaraAttributes>(),
    e1P.attributes = std::make_shared<RivaraAttributes>(),
    e2P.attributes = std::make_shared<RivaraAttributes>(),
    e3P.attributes = std::make_shared<RivaraAttributes>(),
    e4P.attributes = std::make_shared<RivaraAttributes>(),
    e5P.attributes = std::make_shared<RivaraAttributes>(),
    e6P.attributes = std::make_shared<RivaraAttributes>(),
    e7P.attributes = std::make_shared<RivaraAttributes>(),
    t1P.attributes = std::make_shared<RivaraAttributes>();
    t2P.attributes = std::make_shared<RivaraAttributes>();
    t3P.attributes = std::make_shared<RivaraAttributes>();
    int topPixelHeight = image -> height() - 1;
    int rightPixelWidth = image -> width() - 1;
    (*graph)[S].attributes -> SetDouble(RIVARA_ATTRIBUTE_X, rightPixelWidth / 2); 
    (*graph)[S].attributes -> SetDouble(RIVARA_ATTRIBUTE_Y, 0);
    (*graph)[S].x = rightPixelWidth / 2; 
    (*graph)[S].y = 0;
    n1P.attributes -> SetDouble(RIVARA_ATTRIBUTE_X, 0);
    n1P.attributes -> SetDouble(RIVARA_ATTRIBUTE_Y, topPixelHeight);
    n1P.x = 0;
    n1P.y = topPixelHeight;
    n2P.attributes -> SetDouble(RIVARA_ATTRIBUTE_X, rightPixelWidth);
    n2P.attributes -> SetDouble(RIVARA_ATTRIBUTE_Y, topPixelHeight);
    n2P.x = rightPixelWidth;
    n2P.y = topPixelHeight;
    n4P.attributes -> SetDouble(RIVARA_ATTRIBUTE_X, 0);
    n4P.attributes -> SetDouble(RIVARA_ATTRIBUTE_Y, 0);
    n4P.x = 0;
    n4P.y = 0;
    n5P.attributes -> SetDouble(RIVARA_ATTRIBUTE_X, rightPixelWidth);
    n5P.attributes -> SetDouble(RIVARA_ATTRIBUTE_Y, 0);
    n5P.x = rightPixelWidth;
    n5P.y = 0;
    (*graph)[S].attributes -> SetBool(RIVARA_ATTRIBUTE_HN, false);
    graph -> ChangeVertexType(S, NODELABEL_N);
    n1P.label = NODELABEL_N;
    n2P.label = NODELABEL_N;
    n4P.label = NODELABEL_N;
    n5P.label = NODELABEL_N;
    e1P.label = e2P.label = e3P.label = e4P.label = e5P.label = e6P.label = e7P.label = NODELABEL_E;
    e1P.attributes -> SetBool(RIVARA_ATTRIBUTE_B, false);
    e2P.attributes -> SetBool(RIVARA_ATTRIBUTE_B, true);
    e3P.attributes -> SetBool(RIVARA_ATTRIBUTE_B, false);
    e4P.attributes -> SetBool(RIVARA_ATTRIBUTE_B, true);
    e5P.attributes -> SetBool(RIVARA_ATTRIBUTE_B, true);
    e6P.attributes -> SetBool(RIVARA_ATTRIBUTE_B, true);
    e7P.attributes -> SetBool(RIVARA_ATTRIBUTE_B, true);
    e1P.attributes -> SetDouble(RIVARA_ATTRIBUTE_L, NL((*graph)[S], n1P));
    e2P.attributes -> SetDouble(RIVARA_ATTRIBUTE_L, NL(n1P, n2P));
    e3P.attributes -> SetDouble(RIVARA_ATTRIBUTE_L, NL((*graph)[S], n2P));
    e4P.attributes -> SetDouble(RIVARA_ATTRIBUTE_L, NL(n4P, n1P));
    e5P.attributes -> SetDouble(RIVARA_ATTRIBUTE_L, NL(n4P, (*graph)[S]));
    e6P.attributes -> SetDouble(RIVARA_ATTRIBUTE_L, NL((*graph)[S], n5P));
    e7P.attributes -> SetDouble(RIVARA_ATTRIBUTE_L, NL(n5P, n2P));
    e1P.x = (n1P.x + (*graph)[S].x)/2;
    e1P.y = (n1P.y + (*graph)[S].y)/2;
    e2P.x = (n1P.x + n2P.x)/2;
    e2P.y = (n1P.y + n2P.y)/2;
    e3P.x = (n2P.x + (*graph)[S].x)/2;
    e3P.y = (n2P.y + (*graph)[S].y)/2;
    e4P.x = (n4P.x + n1P.x)/2;
    e4P.y = (n4P.y + n1P.y)/2;
    e5P.x = (n4P.x + (*graph)[S].x)/2;
    e5P.y = (n4P.y + (*graph)[S].y)/2;
    e6P.x = (n5P.x + (*graph)[S].x)/2;
    e6P.y = (n5P.y + (*graph)[S].y)/2;
    e7P.x = (n2P.x + n5P.x)/2;
    e7P.y = (n2P.y + n5P.y)/2;
    t1P.label = t2P.label = t3P.label = NODELABEL_T;
    t1P.x = (n1P.x + n2P.x + (*graph)[S].x)/3;
    t1P.y = (n1P.y + n2P.y + (*graph)[S].y)/3;
    t2P.x = (n1P.x + n4P.x + (*graph)[S].x)/3;
    t2P.y = (n1P.y + n4P.y + (*graph)[S].y)/3;
    t3P.x = (n5P.x + n2P.x + (*graph)[S].x)/3;
    t3P.y = (n5P.y + n2P.y + (*graph)[S].y)/3;
    std::tie(n1P.r, n1P.g, n1P.b) = image -> getPixel(0, topPixelHeight);
    std::tie(n2P.r, n2P.g, n2P.b) = image -> getPixel(rightPixelWidth, topPixelHeight);
    std::tie((*graph)[S].r, (*graph)[S].g, (*graph)[S].b) = image -> getPixel(rightPixelWidth / 2, 0);
    std::tie(n4P.r, n4P.g, n4P.b) = image -> getPixel(0, 0);
    std::tie(n5P.r, n5P.g, n5P.b) = image -> getPixel(rightPixelWidth, 0);
    auto n1 = graph -> AddVertex(n1P);
    auto n2 = graph -> AddVertex(n2P);
    auto n4 = graph -> AddVertex(n4P);
    auto n5 = graph -> AddVertex(n5P);
    auto e1 = graph -> AddVertex(e1P);
    auto e2 = graph -> AddVertex(e2P);
    auto e3 = graph -> AddVertex(e3P);
    auto e4 = graph -> AddVertex(e4P);
    auto e5 = graph -> AddVertex(e5P);
    auto e6 = graph -> AddVertex(e6P);
    auto e7 = graph -> AddVertex(e7P);
    auto t1 = graph -> AddVertex(t1P);
    auto t2 = graph -> AddVertex(t2P);
    auto t3 = graph -> AddVertex(t3P);
    graph -> AddEdge(S, e1);
    graph -> AddEdge(n1, e1);
    graph -> AddEdge(n1, e2);
    graph -> AddEdge(n2, e2);
    graph -> AddEdge(n2, e3);
    graph -> AddEdge(S, e3);
    graph -> AddEdge(n4, e4);
    graph -> AddEdge(n1, e4);
    graph -> AddEdge(n4, e5);
    graph -> AddEdge(S, e5);
    graph -> AddEdge(n5, e6);
    graph -> AddEdge(S, e6);
    graph -> AddEdge(n2, e7);
    graph -> AddEdge(n5, e7);
    graph -> AddEdge(S, t1);
    graph -> AddEdge(n1, t1);
    graph -> AddEdge(n2, t1);
    graph -> AddEdge(S, t2);
    graph -> AddEdge(n1, t2);
    graph -> AddEdge(n4, t2);
    graph -> AddEdge(S, t3);
    graph -> AddEdge(n5, t3);
    graph -> AddEdge(n2, t3);
}

std::vector<uint8_t> RivaraP0::Serialize()
{
    throw NotImplementedException();
}
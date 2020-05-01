#include "RivaraUtils.hpp"
#include "gtest/gtest.h"
#include "Pixel.hpp"
#include "CachedGraph.hpp"
#include "Image.hpp"
#include "RivaraP1.hpp"

using namespace Rivara;

class RivaraProduction1Test : public ::testing::Test
{
    public:
        Pixel node1;
        Pixel node2;
        Pixel node3;
        Pixel T;
        Pixel E1, E2, E3;
        vertex_descriptor node1Vertex;
        vertex_descriptor node2Vertex;
        vertex_descriptor node3Vertex;
        vertex_descriptor E1Vertex;
        vertex_descriptor E2Vertex;
        vertex_descriptor E3Vertex;
        vertex_descriptor TVertex;
        std::vector<vertex_descriptor> nodes;
        std::unique_ptr<Rivara::RivaraP1> p1;
        std::shared_ptr<CachedGraph> g; 
        std::shared_ptr<Image> image;   
    protected:
        RivaraProduction1Test() {
            g = std::make_shared<CachedGraph>();
            image = std::make_shared<Image>(10,10);
            node1.attributes = std::make_shared<RivaraAttributes>();
            node2.attributes = std::make_shared<RivaraAttributes>();
            node3.attributes = std::make_shared<RivaraAttributes>();
            T.attributes = std::make_shared<RivaraAttributes>();
            E1.attributes = std::make_shared<RivaraAttributes>();
            E2.attributes = std::make_shared<RivaraAttributes>();
            E3.attributes = std::make_shared<RivaraAttributes>();
            node1.attributes->SetDouble(RIVARA_ATTRIBUTE_X, 3);
            node1.attributes->SetDouble(RIVARA_ATTRIBUTE_Y, 4);
            node2.attributes->SetDouble(RIVARA_ATTRIBUTE_X, 6);
            node2.attributes->SetDouble(RIVARA_ATTRIBUTE_Y, 6);
            node3.attributes->SetDouble(RIVARA_ATTRIBUTE_X, 5);
            node3.attributes->SetDouble(RIVARA_ATTRIBUTE_Y, 5);
            node1.label = NODELABEL_N;
            node1.label = NODELABEL_N;
            node2.label = NODELABEL_N;
            node2.label = NODELABEL_N;
            node3.label = NODELABEL_N;
            node3.label = NODELABEL_N;
            E1.attributes -> SetDouble(RIVARA_ATTRIBUTE_L, NL(node1, node2));
            E2.attributes -> SetDouble(RIVARA_ATTRIBUTE_L, NL(node2, node3));
            E3.attributes -> SetDouble(RIVARA_ATTRIBUTE_L, NL(node3, node1));
            E1.label = NODELABEL_E;
            E2.label = NODELABEL_E;
            E3.label = NODELABEL_E;
            T.label = NODELABEL_T;

            node1Vertex = g->AddVertex(node1);
            node2Vertex = g->AddVertex(node2);
            node3Vertex = g->AddVertex(node3);
            E1Vertex = g->AddVertex(E1);
            E2Vertex = g->AddVertex(E2);
            E3Vertex = g->AddVertex(E3);
            TVertex = g->AddVertex(T);

            g->AddEdge(node1Vertex, E1Vertex);
            g->AddEdge(node2Vertex, E1Vertex);
            g->AddEdge(node3Vertex, E2Vertex);
            g->AddEdge(node2Vertex, E2Vertex);
            g->AddEdge(node3Vertex, E3Vertex);
            g->AddEdge(node1Vertex, E3Vertex);
            g->AddEdge(node1Vertex, TVertex);
            g->AddEdge(node2Vertex, TVertex);
            g->AddEdge(node3Vertex, TVertex);

            nodes.push_back(node1Vertex);
            nodes.push_back(node2Vertex);
            p1 = std::make_unique<Rivara::RivaraP1>(g, E1Vertex, TVertex, image); 
        }

        ~RivaraProduction1Test() override {
            // You can do clean-up work that doesn't throw exceptions here.
        }

        void SetUp() override {
            // Code here will be called immediately after the constructor (right
            // before each test).
        }

        void TearDown() override {
            // Code here will be called immediately after each test (right
            // before the destructor).
        }
};

TEST_F(RivaraProduction1Test, P1TestNewENodeLAttribute)
{
    this->E1.attributes->SetBool(RIVARA_ATTRIBUTE_B, true);
    Pixel newNNode = this->p1->GetNewNNode(this->nodes);
    Pixel p = this->p1->GetNewENode(newNNode, this->nodes);
    ASSERT_DOUBLE_EQ(p.attributes -> GetDouble(RIVARA_ATTRIBUTE_L), 1.8027756377319946);
}

TEST_F(RivaraProduction1Test, P1TestNewENodeBAttributeTrue)
{
    this->E1.attributes->SetBool(RIVARA_ATTRIBUTE_B, true);
    Pixel newNNode = this->p1->GetNewNNode(this->nodes);
    Pixel p = this->p1->GetNewENode(newNNode, this->nodes);
    ASSERT_EQ(p.attributes -> GetBool(RIVARA_ATTRIBUTE_B), true);
}

TEST_F(RivaraProduction1Test, P1TestNewENodeBAttributeFalse)
{
    this->E1.attributes->SetBool(RIVARA_ATTRIBUTE_B, false);
    Pixel newNNode = this->p1->GetNewNNode(this->nodes);
    Pixel p = this->p1->GetNewENode(newNNode, this->nodes);
    ASSERT_EQ(p.attributes -> GetBool(RIVARA_ATTRIBUTE_B), false);
}

TEST_F(RivaraProduction1Test, P1TestNewNNode)
{
    Pixel p = this->p1->GetNewNNode(this->nodes);
    ASSERT_DOUBLE_EQ(p.attributes -> GetDouble(RIVARA_ATTRIBUTE_X), 4.5);
    ASSERT_DOUBLE_EQ(p.attributes -> GetDouble(RIVARA_ATTRIBUTE_Y), 5);
}

TEST_F(RivaraProduction1Test, P1TestNewEMiddleNodeBAttribute)
{
    Pixel newNNode = this->p1->GetNewNNode(this->nodes);
    Pixel p = this->p1->GetNewEMiddleNode(newNNode, node3Vertex);
    ASSERT_EQ(p.attributes -> GetBool(RIVARA_ATTRIBUTE_B), false);
}

TEST_F(RivaraProduction1Test, P1TestNewEMiddleNodeLAttribute)
{
    Pixel newNNode = this->p1->GetNewNNode(this->nodes);
    Pixel p = this->p1->GetNewEMiddleNode(newNNode, node3Vertex);
    ASSERT_EQ(p.attributes -> GetDouble(RIVARA_ATTRIBUTE_L), 0.5);
}

TEST_F(RivaraProduction1Test, P1TestNewTNode)
{
    auto TNode = p1 -> GetNewTNode();
    ASSERT_EQ(TNode.attributes -> GetBool(RIVARA_ATTRIBUTE_R), false);
}

TEST_F(RivaraProduction1Test, P1TestProcess)
{
    p1 -> Perform();
    auto nNodes = this -> g -> GetCacheIterator(NODELABEL_N);
    auto tNodes = this -> g -> GetCacheIterator(NODELABEL_T);
    auto eNodes = this -> g -> GetCacheIterator(NODELABEL_E);
    ASSERT_EQ(nNodes.size(), 4);
    ASSERT_EQ(tNodes.size(), 2);
    ASSERT_EQ(eNodes.size(), 5);
    //ASSERT_EQ(false, true);
}

TEST_F(RivaraProduction1Test, P1TestGetBestEdge)
{
    auto eNodes = this -> g -> GetCacheIterator(NODELABEL_E);
    auto bestEdge = p1 -> GetBestEdge(this -> g, std::vector<vertex_descriptor>(eNodes.begin(), eNodes.end()));
    ASSERT_EQ(bestEdge, E1Vertex);
}

TEST(RUTest, NLPointToPointPitagoras)
{
    Pixel p1, p2;
    p1.attributes = std::make_shared<RivaraAttributes>();
    p2.attributes = std::make_shared<RivaraAttributes>();
    p1.attributes->SetDouble(RIVARA_ATTRIBUTE_X, 1);
    p1.attributes->SetDouble(RIVARA_ATTRIBUTE_Y, 4);
    p2.attributes->SetDouble(RIVARA_ATTRIBUTE_X, 5);
    p2.attributes->SetDouble(RIVARA_ATTRIBUTE_Y, 1);
    double actual = Rivara::NL(p1,p2);
    double expected = 5.0;
    ASSERT_DOUBLE_EQ(expected, actual);
}

TEST(RUTest, NLCoordsPitagoras)
{
    int x1 = 1;
    int y1 = 4;
    int x2 = 5;
    int y2 = 1;
    double actual = Rivara::NL(x1,y1,x2,y2);
    double expected = 5.0;
    ASSERT_DOUBLE_EQ(expected, actual);
}
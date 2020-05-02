#include "gtest/gtest.h" 
#include "RivaraUtils.hpp"
#include "Pixel.hpp"
#include "CachedGraph.hpp"
#include "Image.hpp"
#include "RivaraP2.hpp"


namespace Rivara{
    class RivaraProduction2Test : public ::testing::Test
    {
        public:
            Pixel node1;
            Pixel node2;
            Pixel node3;
            Pixel node4;
            Pixel T;
            Pixel E2, E3, E4, E5;
            vertex_descriptor node1Vertex;
            vertex_descriptor node2Vertex;
            vertex_descriptor node3Vertex;
            vertex_descriptor node4Vertex;
            vertex_descriptor E2Vertex;
            vertex_descriptor E3Vertex;
            vertex_descriptor E4Vertex;
            vertex_descriptor E5Vertex;
            vertex_descriptor TVertex;
            std::vector<vertex_descriptor> nodes;
            std::unique_ptr<Rivara::RivaraP2> p2;
            std::shared_ptr<CachedGraph> g; 
            std::shared_ptr<Image> image;   
        protected:
            RivaraProduction2Test() {
                g = std::make_shared<CachedGraph>();
                image = std::make_shared<Image>(10,10);
                node1.attributes = std::make_shared<RivaraAttributes>();
                node2.attributes = std::make_shared<RivaraAttributes>();
                node3.attributes = std::make_shared<RivaraAttributes>();
                node4.attributes = std::make_shared<RivaraAttributes>();
                T.attributes = std::make_shared<RivaraAttributes>();
                E2.attributes = std::make_shared<RivaraAttributes>();
                E3.attributes = std::make_shared<RivaraAttributes>();
                E4.attributes = std::make_shared<RivaraAttributes>();
                E5.attributes = std::make_shared<RivaraAttributes>();
                node1.attributes->SetDouble(RIVARA_ATTRIBUTE_X, 1);
                node1.attributes->SetDouble(RIVARA_ATTRIBUTE_Y, 1);
                node2.attributes->SetDouble(RIVARA_ATTRIBUTE_X, 9);
                node2.attributes->SetDouble(RIVARA_ATTRIBUTE_Y, 1);
                node3.attributes->SetDouble(RIVARA_ATTRIBUTE_X, 5);
                node3.attributes->SetDouble(RIVARA_ATTRIBUTE_Y, 3);
                node4.attributes->SetDouble(RIVARA_ATTRIBUTE_X, 5);
                node4.attributes->SetDouble(RIVARA_ATTRIBUTE_Y, 1);
                node1.label = NODELABEL_N;
                node2.label = NODELABEL_N;
                node3.label = NODELABEL_N;
                node4.label = NODELABEL_N;
                E2.attributes -> SetDouble(RIVARA_ATTRIBUTE_L, NL(node2, node3));
                E3.attributes -> SetDouble(RIVARA_ATTRIBUTE_L, NL(node3, node1));
                E4.attributes -> SetDouble(RIVARA_ATTRIBUTE_L, NL(node1, node2));
                E5.attributes -> SetDouble(RIVARA_ATTRIBUTE_L, NL(node1, node2));
                E2.label = NODELABEL_E;
                E3.label = NODELABEL_E;
                E4.label = NODELABEL_E;
                E5.label = NODELABEL_E;
                T.label = NODELABEL_T;

                node1Vertex = g->AddVertex(node1);
                node2Vertex = g->AddVertex(node2);
                node3Vertex = g->AddVertex(node3);
                node4Vertex = g->AddVertex(node4);
                E2Vertex = g->AddVertex(E2);
                E3Vertex = g->AddVertex(E3);
                E4Vertex = g->AddVertex(E4);
                E5Vertex = g->AddVertex(E5);
                TVertex = g->AddVertex(T);

                g->AddEdge(node1Vertex, E4Vertex);
                g->AddEdge(node4Vertex, E4Vertex);
                g->AddEdge(node2Vertex, E5Vertex);
                g->AddEdge(node4Vertex, E5Vertex);
                g->AddEdge(node3Vertex, E2Vertex);
                g->AddEdge(node2Vertex, E2Vertex);
                g->AddEdge(node3Vertex, E3Vertex);
                g->AddEdge(node1Vertex, E3Vertex);
                g->AddEdge(node1Vertex, TVertex);
                g->AddEdge(node2Vertex, TVertex);
                g->AddEdge(node3Vertex, TVertex);

                nodes.push_back(node1Vertex);
                nodes.push_back(node2Vertex);
                p2 = std::make_unique<Rivara::RivaraP2>(g, node4Vertex, TVertex); 
            }

            ~RivaraProduction2Test() override {
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

    TEST_F(RivaraProduction2Test, P2TestPerform)
    {
        p2 -> Perform();
        auto nNodes = this -> g -> GetCacheIterator(NODELABEL_N);
        auto tNodes = this -> g -> GetCacheIterator(NODELABEL_T);
        auto eNodes = this -> g -> GetCacheIterator(NODELABEL_E);
        ASSERT_EQ(nNodes.size(), 4);
        ASSERT_EQ(tNodes.size(), 2);
        ASSERT_EQ(eNodes.size(), 5);
    }

    TEST_F(RivaraProduction2Test, GetNewENode)
    {
        Pixel newENode = p2 -> GetNewENode(node4, node3Vertex);
        ASSERT_DOUBLE_EQ(newENode.attributes -> GetDouble(RIVARA_ATTRIBUTE_L), 2.0);
        ASSERT_EQ(newENode.attributes -> GetBool(RIVARA_ATTRIBUTE_B), false);
    }

    TEST_F(RivaraProduction2Test, GetNewTNode)
    {
        Pixel newENode = p2 -> GetNewENode(node4, node3Vertex);
        ASSERT_DOUBLE_EQ(newENode.attributes -> GetBool(RIVARA_ATTRIBUTE_R), false);
    }
}
#include "gtest/gtest.h" 
#include "RivaraUtils.hpp"
#include "Pixel.hpp"
#include "CachedGraph.hpp"
#include "Image.hpp"
#include "RivaraP6.hpp"


namespace Rivara{
    class RivaraProduction6Test : public ::testing::Test
    {
        public:
            Pixel node1;
            Pixel node2;
            Pixel node3;
            Pixel node4;
            Pixel node5;
            Pixel node6;
            Pixel T;
            Pixel E4, E5, E6, E7, E8, E9;
            vertex_descriptor node1Vertex;
            vertex_descriptor node2Vertex;
            vertex_descriptor node3Vertex;
            vertex_descriptor node4Vertex;
            vertex_descriptor node5Vertex;
            vertex_descriptor node6Vertex;
            vertex_descriptor E4Vertex;
            vertex_descriptor E5Vertex;
            vertex_descriptor E6Vertex;
            vertex_descriptor E7Vertex;
            vertex_descriptor E8Vertex;
            vertex_descriptor E9Vertex;
            vertex_descriptor TVertex;
            std::vector<vertex_descriptor> nodes;
            std::unique_ptr<Rivara::RivaraP6> p6;
            std::shared_ptr<CachedGraph> g; 
            std::shared_ptr<Image> image;   
        protected:
            RivaraProduction6Test() {
                g = std::make_shared<CachedGraph>();
                node1.attributes = std::make_shared<RivaraAttributes>();
                node2.attributes = std::make_shared<RivaraAttributes>();
                node3.attributes = std::make_shared<RivaraAttributes>();
                node4.attributes = std::make_shared<RivaraAttributes>();
                node5.attributes = std::make_shared<RivaraAttributes>();
                node6.attributes = std::make_shared<RivaraAttributes>();
                T.attributes = std::make_shared<RivaraAttributes>();
                E4.attributes = std::make_shared<RivaraAttributes>();
                E5.attributes = std::make_shared<RivaraAttributes>();
                E6.attributes = std::make_shared<RivaraAttributes>();
                E7.attributes = std::make_shared<RivaraAttributes>();
                E8.attributes = std::make_shared<RivaraAttributes>();
                E9.attributes = std::make_shared<RivaraAttributes>();
                node1.attributes->SetDouble(RIVARA_ATTRIBUTE_X, 1);
                node1.attributes->SetDouble(RIVARA_ATTRIBUTE_Y, 1);
                node2.attributes->SetDouble(RIVARA_ATTRIBUTE_X, 9);
                node2.attributes->SetDouble(RIVARA_ATTRIBUTE_Y, 1);
                node3.attributes->SetDouble(RIVARA_ATTRIBUTE_X, 9);
                node3.attributes->SetDouble(RIVARA_ATTRIBUTE_Y, 9);
                node4.attributes->SetDouble(RIVARA_ATTRIBUTE_X, 5);
                node4.attributes->SetDouble(RIVARA_ATTRIBUTE_Y, 1);
                node5.attributes->SetDouble(RIVARA_ATTRIBUTE_X, 9);
                node5.attributes->SetDouble(RIVARA_ATTRIBUTE_Y, 5);
                node6.attributes->SetDouble(RIVARA_ATTRIBUTE_X, 9);
                node6.attributes->SetDouble(RIVARA_ATTRIBUTE_Y, 5);
                node1.label = NODELABEL_N;
                node2.label = NODELABEL_N;
                node3.label = NODELABEL_N;
                node4.label = NODELABEL_N;
                node5.label = NODELABEL_N;
                node6.label = NODELABEL_N;
                E4.attributes -> SetDouble(RIVARA_ATTRIBUTE_L, NL(node1, node2));
                E5.attributes -> SetDouble(RIVARA_ATTRIBUTE_L, NL(node1, node2));
                E6.attributes -> SetDouble(RIVARA_ATTRIBUTE_L, NL(node2, node3));
                E7.attributes -> SetDouble(RIVARA_ATTRIBUTE_L, NL(node2, node3));
                E8.attributes -> SetDouble(RIVARA_ATTRIBUTE_L, NL(node2, node3));
                E9.attributes -> SetDouble(RIVARA_ATTRIBUTE_L, NL(node2, node3));
                E4.label = NODELABEL_E;
                E5.label = NODELABEL_E;
                E6.label = NODELABEL_E;
                E7.label = NODELABEL_E;
                E8.label = NODELABEL_E;
                E9.label = NODELABEL_E;
                T.label = NODELABEL_T;

                node1Vertex = g->AddVertex(node1);
                node2Vertex = g->AddVertex(node2);
                node3Vertex = g->AddVertex(node3);
                node4Vertex = g->AddVertex(node4);
                node5Vertex = g->AddVertex(node5);
                node6Vertex = g->AddVertex(node6);
                E4Vertex = g->AddVertex(E4);
                E5Vertex = g->AddVertex(E5);
                E6Vertex = g->AddVertex(E6);
                E7Vertex = g->AddVertex(E7);
                E8Vertex = g->AddVertex(E8);
                E9Vertex = g->AddVertex(E9);
                TVertex = g->AddVertex(T);

                g->AddEdge(node3Vertex, E3Vertex);
                g->AddEdge(node1Vertex, E3Vertex);
                g->AddEdge(node1Vertex, E4Vertex);
                g->AddEdge(node4Vertex, E4Vertex);
                g->AddEdge(node2Vertex, E5Vertex);
                g->AddEdge(node4Vertex, E5Vertex);
                g->AddEdge(node5Vertex, E6Vertex);
                g->AddEdge(node2Vertex, E6Vertex);
                g->AddEdge(node5Vertex, E7Vertex);
                g->AddEdge(node3Vertex, E7Vertex);

                g->AddEdge(node1Vertex, TVertex);
                g->AddEdge(node2Vertex, TVertex);
                g->AddEdge(node3Vertex, TVertex);

                nodes.push_back(node2Vertex);
                nodes.push_back(node3Vertex);   
                p6 = std::make_unique<Rivara::RivaraP6>(g, node2Vertex, TVertex); 
            }

            ~RivaraProduction6Test() override {
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

    TEST_F(RivaraProduction6Test, P6TestPerform)
    {
        p6 -> Perform();
        auto nNodes = this -> g -> GetCacheIterator(NODELABEL_N);
        auto tNodes = this -> g -> GetCacheIterator(NODELABEL_T);
        auto eNodes = this -> g -> GetCacheIterator(NODELABEL_E);
        ASSERT_EQ(nNodes.size(), 6);
        ASSERT_EQ(tNodes.size(), 2);
        ASSERT_EQ(eNodes.size(), 7);
    }

    TEST_F(RivaraProduction6Test, P6TestNewEMiddleNodeBAttribute)
    {
        // Pixel newNNode = this->p6->GetNewNNode(this->nodes);
        // Pixel p = this->p6->GetNewEMiddleNode(newNNode, node4Vertex);
        // ASSERT_EQ(p.attributes -> GetBool(RIVARA_ATTRIBUTE_B), false);
    }

    TEST_F(RivaraProduction6Test, P6TestNewEMiddleNodeLAttribute)
    {
        // Pixel newNNode = this->p6->GetNewNNode(this->nodes);
        // Pixel p = this->p6->GetNewEMiddleNode(newNNode, node3Vertex);
        // ASSERT_EQ(p.attributes -> GetDouble(RIVARA_ATTRIBUTE_L), 2.5);
    }

    TEST_F(RivaraProduction6Test, P6TestNewTNode)
    {
        auto TNode = p6 -> GetNewTNode();
        ASSERT_EQ(TNode.attributes -> GetBool(RIVARA_ATTRIBUTE_R), false);
    }
}
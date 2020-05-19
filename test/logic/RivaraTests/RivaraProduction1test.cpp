#include "gtest/gtest.h"
#include "RivaraUtils.hpp"
#include "Pixel.hpp"
#include "CachedGraph.hpp"
#include "Image.hpp"
#include "RivaraP1.hpp"


namespace Rivara{
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
                image = std::make_shared<Image>(10, 10);
                node1.attributes = std::make_shared<RivaraAttributes>();
                node2.attributes = std::make_shared<RivaraAttributes>();
                node3.attributes = std::make_shared<RivaraAttributes>();
                T.attributes = std::make_shared<RivaraAttributes>();
                T.x = 3;
                T.y = 2;
                E1.attributes = std::make_shared<RivaraAttributes>();
                E2.attributes = std::make_shared<RivaraAttributes>();
                E3.attributes = std::make_shared<RivaraAttributes>();
                E1.x = 2;
                E1.y = 3;
                E2.x = 4;
                E2.y = 3;
                E3.x = 2;
                E3.y = 1;
                node1.attributes->SetDouble(RIVARA_ATTRIBUTE_X, 1);
                node1.attributes->SetDouble(RIVARA_ATTRIBUTE_Y, 1);
                node1.x = 1;
                node1.y = 1;
                node2.attributes->SetDouble(RIVARA_ATTRIBUTE_X, 4);
                node2.attributes->SetDouble(RIVARA_ATTRIBUTE_Y, 5);
                node2.x = 4;
                node2.y = 5;
                node3.attributes->SetDouble(RIVARA_ATTRIBUTE_X, 4);
                node3.attributes->SetDouble(RIVARA_ATTRIBUTE_Y, 1);
                node3.x = 4;
                node3.y = 1;
                node1.label = NODELABEL_N;
                node2.label = NODELABEL_N;
                node3.label = NODELABEL_N;
                E1.attributes -> SetDouble(RIVARA_ATTRIBUTE_L, NL(node2, node1));
                E2.attributes -> SetDouble(RIVARA_ATTRIBUTE_L, NL(node2, node3));
                E3.attributes -> SetDouble(RIVARA_ATTRIBUTE_L, NL(node3, node1));
                E1.label = NODELABEL_E;
                E2.label = NODELABEL_E;
                E3.label = NODELABEL_E;
                T.label = NODELABEL_T;
                T.attributes->SetBool(RIVARA_ATTRIBUTE_R, true);

                node1Vertex = g->AddVertex(node1);
                node2Vertex = g->AddVertex(node2);
                node3Vertex = g->AddVertex(node3);
                E1Vertex = g->AddVertex(E1);
                E2Vertex = g->AddVertex(E2);
                E3Vertex = g->AddVertex(E3);
                TVertex = g->AddVertex(T);

                g->AddEdge(node1Vertex, E1Vertex);
                g->AddEdge(node2Vertex, E1Vertex);
                g->AddEdge(node2Vertex, E2Vertex);
                g->AddEdge(node3Vertex, E2Vertex);
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
        Pixel p = GetNewENode(this->g, this->E1Vertex);
        ASSERT_DOUBLE_EQ(p.attributes -> GetDouble(RIVARA_ATTRIBUTE_L), 2.5);
    }

    TEST_F(RivaraProduction1Test, P1TestNewENodeBAttributeTrue)
    {
        this->E1.attributes->SetBool(RIVARA_ATTRIBUTE_B, true);
        Pixel p = GetNewENode(this->g, this->E1Vertex);
        ASSERT_EQ(p.attributes -> GetBool(RIVARA_ATTRIBUTE_B), true);
    }

    TEST_F(RivaraProduction1Test, P1TestNewENodeBAttributeFalse)
    {
        this->E1.attributes->SetBool(RIVARA_ATTRIBUTE_B, false);
        Pixel p = GetNewENode(this->g, this->E1Vertex);
        ASSERT_EQ(p.attributes -> GetBool(RIVARA_ATTRIBUTE_B), false);
    }

    TEST_F(RivaraProduction1Test, P1TestNewNNode)
    {
        Pixel p = GetNewNNode(this->g, this->nodes, this->image);
        ASSERT_DOUBLE_EQ(p.attributes -> GetDouble(RIVARA_ATTRIBUTE_X), 2.5);
        ASSERT_DOUBLE_EQ(p.attributes -> GetDouble(RIVARA_ATTRIBUTE_Y), 3);
    }

    TEST_F(RivaraProduction1Test, P1TestNewEMiddleNodeBAttribute)
    {
        Pixel newNNode = GetNewNNode(this->g, this->nodes, this->image);
        Pixel p = GetNewEMiddleNode(this->g, newNNode, node3Vertex);
        ASSERT_EQ(p.attributes -> GetBool(RIVARA_ATTRIBUTE_B), false);
    }

    TEST_F(RivaraProduction1Test, P1TestNewEMiddleNodeLAttribute)
    {
        Pixel newNNode = GetNewNNode(this->g, this->nodes, this->image);
        Pixel p = GetNewEMiddleNode(this->g, newNNode, node3Vertex);
        ASSERT_EQ(p.attributes -> GetDouble(RIVARA_ATTRIBUTE_L), 2.5);
    }

    TEST_F(RivaraProduction1Test, P1TestNewTNode)
    {
        auto TNode = GetNewTNode();
        ASSERT_EQ(TNode.attributes -> GetBool(RIVARA_ATTRIBUTE_R), false);
    }

    TEST_F(RivaraProduction1Test, P1TestProcess)
    {
        p1 -> Perform();
        auto nNodes = this -> g -> GetCacheIterator(NODELABEL_N);
        auto tNodes = this -> g -> GetCacheIterator(NODELABEL_T);
        auto eNodes = this -> g -> GetCacheIterator(NODELABEL_E);
        ASSERT_EQ(nNodes.size(), 4u);
        ASSERT_EQ(tNodes.size(), 2u);
        ASSERT_EQ(eNodes.size(), 5u);
        for(auto tNode: tNodes)
        {
            ASSERT_EQ((*g)[tNode].attributes->GetBool(RIVARA_ATTRIBUTE_R), false);
            auto vertices = this -> g -> GetAdjacentVertices(tNode);
            for(auto vertex : vertices)
            {
                ASSERT_EQ((*g)[vertex].label, NODELABEL_N);
            }
        }
        ASSERT_DOUBLE_EQ((*g)[E1Vertex].attributes->GetDouble(RIVARA_ATTRIBUTE_L), 2.5);
        //ASSERT_EQ(false, true);
    }

    TEST_F(RivaraProduction1Test, P1TestGetBestEdge)
    {
        auto eNodes = this -> g -> GetCacheIterator(NODELABEL_E);
        auto bestEdge = p1 -> GetBestEdge(this -> g, std::vector<vertex_descriptor>(eNodes.begin(), eNodes.end()));
        ASSERT_EQ(bestEdge, E1Vertex);
    }

    TEST_F(RivaraProduction1Test, P1TestGetBestEdge2)
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

    class RivaraProduction1BestEdgeTestParameters
    {
        public:
            bool HN1, HN2, HN3;
            double L13, L23, L12; 
            bool B13, B23, B12, result;
            RivaraProduction1BestEdgeTestParameters(
                bool HN1, bool HN2, bool HN3, double L13, double L23, double L12, bool B13, bool B23, bool B12,
                 bool result
            ):
            HN1(HN1),
            HN2(HN2),
            HN3(HN3),
            L13(L13),
            L23(L23),
            L12(L12),
            B13(B13),
            B23(B23),
            B12(B12),
            result(result)
            {};

            void Print()
            {
                std::cout<<" "<<HN1<<" "<<HN2<<" "<<HN3<<" "<<L13<<" "<<L23<<" "<<L12<<" "<<B13<<" "<<B23<<" "<<B12<<" "<<result<<std::endl;
            };
    };


    class RivaraProduction1BestEdgeTestFixture :public ::testing::TestWithParam<RivaraProduction1BestEdgeTestParameters> 
    {

    };

    TEST_P(RivaraProduction1BestEdgeTestFixture, BestEdgeTests)
    {
        auto params = GetParam();   
        Pixel node1;
        Pixel node2;
        Pixel node3;
        Pixel T;
        Pixel E13, E23, E12;
        vertex_descriptor node1Vertex;
        vertex_descriptor node2Vertex;
        vertex_descriptor node3Vertex;
        vertex_descriptor E13Vertex;
        vertex_descriptor E23Vertex;
        vertex_descriptor E12Vertex;
        vertex_descriptor TVertex;
        std::vector<vertex_descriptor> nodes;
        std::unique_ptr<Rivara::RivaraP1> p1;
        std::shared_ptr<CachedGraph> g; 
        std::shared_ptr<Image> image;   
        g = std::make_shared<CachedGraph>();
        image = std::make_shared<Image>(10, 10);
        node1.attributes = std::make_shared<RivaraAttributes>();
        node2.attributes = std::make_shared<RivaraAttributes>();
        node3.attributes = std::make_shared<RivaraAttributes>();
        node1.attributes -> SetBool(RIVARA_ATTRIBUTE_HN, params.HN1);
        node2.attributes -> SetBool(RIVARA_ATTRIBUTE_HN, params.HN2);
        node3.attributes -> SetBool(RIVARA_ATTRIBUTE_HN, params.HN3);
        T.attributes = std::make_shared<RivaraAttributes>();
        T.x = 4;
        T.y = 5;
        E13.attributes = std::make_shared<RivaraAttributes>();
        E23.attributes = std::make_shared<RivaraAttributes>();
        E12.attributes = std::make_shared<RivaraAttributes>();
        
        node1.attributes->SetDouble(RIVARA_ATTRIBUTE_X, 3);
        node1.attributes->SetDouble(RIVARA_ATTRIBUTE_Y, 4);
        node1.x = 3;
        node1.y = 4;
        node2.attributes->SetDouble(RIVARA_ATTRIBUTE_X, 6);
        node2.attributes->SetDouble(RIVARA_ATTRIBUTE_Y, 6);
        node2.x = 6;
        node2.y = 6;
        node3.attributes->SetDouble(RIVARA_ATTRIBUTE_X, 5);
        node3.attributes->SetDouble(RIVARA_ATTRIBUTE_Y, 5);
        node3.x = 5;
        node3.y = 5;
        node1.label = NODELABEL_N;
        node2.label = NODELABEL_N;
        node3.label = NODELABEL_N;
        E13.attributes -> SetDouble(RIVARA_ATTRIBUTE_L, params.L13);
        E23.attributes -> SetDouble(RIVARA_ATTRIBUTE_L, params.L23);
        E12.attributes -> SetDouble(RIVARA_ATTRIBUTE_L, params.L12);
        E13.attributes -> SetBool(RIVARA_ATTRIBUTE_B, params.B13);
        E23.attributes -> SetBool(RIVARA_ATTRIBUTE_B, params.B23);
        E12.attributes -> SetBool(RIVARA_ATTRIBUTE_B, params.B12);
        E13.label = NODELABEL_E;
        E23.label = NODELABEL_E;
        E12.label = NODELABEL_E;
        T.label = NODELABEL_T;

        node1Vertex = g->AddVertex(node1);
        node2Vertex = g->AddVertex(node2);
        node3Vertex = g->AddVertex(node3);
        E13Vertex = g->AddVertex(E13);
        E23Vertex = g->AddVertex(E23);
        E12Vertex = g->AddVertex(E12);
        TVertex = g->AddVertex(T);

        g->AddEdge(node1Vertex, E12Vertex);
        g->AddEdge(node2Vertex, E12Vertex);
        g->AddEdge(node2Vertex, E23Vertex);
        g->AddEdge(node3Vertex, E23Vertex);
        g->AddEdge(node3Vertex, E13Vertex);
        g->AddEdge(node1Vertex, E13Vertex);
        g->AddEdge(node1Vertex, TVertex);
        g->AddEdge(node2Vertex, TVertex);
        g->AddEdge(node3Vertex, TVertex);

        std::vector<vertex_descriptor> EEdges;
        EEdges.emplace_back(E13Vertex);
        EEdges.emplace_back(E23Vertex);
        EEdges.emplace_back(E12Vertex);
        if((RivaraP1::GetBestEdge(g, EEdges)==E13Vertex) != params.result)
            params.Print();
        ASSERT_EQ(RivaraP1::GetBestEdge(g, EEdges)==E13Vertex, params.result);
    }

    INSTANTIATE_TEST_SUITE_P(
        BestEdgeTestInstance,
        RivaraProduction1BestEdgeTestFixture,
        ::testing::Values(
            //params: HN1, HN2, HN3, L13, L23, L12, B13, B23, B12, IS_E13_BestEdge
            RivaraProduction1BestEdgeTestParameters(true,true,true,5.0,4.0,3.0,true,true,true,true),
            RivaraProduction1BestEdgeTestParameters(true,true,false,5.0,4.0,3.0,true,true,true,true),
            RivaraProduction1BestEdgeTestParameters(true,false,true,5.0,4.0,3.0,true,true,true,true),
            RivaraProduction1BestEdgeTestParameters(true,false,false,5.0,4.0,3.0,true,true,true,true),
            RivaraProduction1BestEdgeTestParameters(false,true,true,5.0,4.0,3.0,true,true,true,true),
            RivaraProduction1BestEdgeTestParameters(false,true,false,5.0,4.0,3.0,true,true,true,true),
            RivaraProduction1BestEdgeTestParameters(false,false,true,5.0,4.0,3.0,true,true,true,true),
            RivaraProduction1BestEdgeTestParameters(false,false,false,5.0,4.0,3.0,true,true,true,true),
            RivaraProduction1BestEdgeTestParameters(true,true,true,5.0,4.0,3.0,false,false,false,false),
            RivaraProduction1BestEdgeTestParameters(true,true,false,5.0,4.0,3.0,false,false,false,false),
            RivaraProduction1BestEdgeTestParameters(true,false,true,5.0,4.0,3.0,false,false,false,false),
            RivaraProduction1BestEdgeTestParameters(true,false,false,5.0,4.0,3.0,false,false,false,false),
            RivaraProduction1BestEdgeTestParameters(false,true,true,5.0,4.0,3.0,false,false,false,false),
            RivaraProduction1BestEdgeTestParameters(false,true,false,5.0,4.0,3.0,false,false,false,true),
            RivaraProduction1BestEdgeTestParameters(false,false,true,5.0,4.0,3.0,false,false,false,false),
            RivaraProduction1BestEdgeTestParameters(false,false,false,5.0,4.0,3.0,false,false,false,true),
            RivaraProduction1BestEdgeTestParameters(true,true,true,4.0,5.0,3.0,true,true,true,false),
            RivaraProduction1BestEdgeTestParameters(true,true,false,4.0,5.0,3.0,true,true,true,false),
            RivaraProduction1BestEdgeTestParameters(true,false,true,4.0,5.0,3.0,true,true,true,false),
            RivaraProduction1BestEdgeTestParameters(true,false,false,4.0,5.0,3.0,true,true,true,false),
            RivaraProduction1BestEdgeTestParameters(false,true,true,4.0,5.0,3.0,true,true,true,false),
            RivaraProduction1BestEdgeTestParameters(false,true,false,4.0,5.0,3.0,true,true,true,false),
            RivaraProduction1BestEdgeTestParameters(false,false,true,4.0,5.0,3.0,true,true,true,false),
            RivaraProduction1BestEdgeTestParameters(false,false,false,4.0,5.0,3.0,true,true,true,false),
            RivaraProduction1BestEdgeTestParameters(true,true,true,4.0,5.0,3.0,false,false,false,false),
            RivaraProduction1BestEdgeTestParameters(true,true,false,4.0,5.0,3.0,false,false,false,false),
            RivaraProduction1BestEdgeTestParameters(true,false,true,4.0,5.0,3.0,false,false,false,false),
            RivaraProduction1BestEdgeTestParameters(true,false,false,4.0,5.0,3.0,false,false,false,false),
            RivaraProduction1BestEdgeTestParameters(false,true,true,4.0,5.0,3.0,false,false,false,false),
            RivaraProduction1BestEdgeTestParameters(false,true,false,4.0,5.0,3.0,false,false,false,false),
            RivaraProduction1BestEdgeTestParameters(false,false,true,4.0,5.0,3.0,false,false,false,false),
            RivaraProduction1BestEdgeTestParameters(false,false,false,4.0,5.0,3.0,false,false,false,false),
            RivaraProduction1BestEdgeTestParameters(true,true,true,5.0,5.0,3.0,false,false,false,false),
            RivaraProduction1BestEdgeTestParameters(true,true,false,5.0,5.0,3.0,false,false,false,false),
            RivaraProduction1BestEdgeTestParameters(true,false,true,5.0,5.0,3.0,false,false,false,false),
            RivaraProduction1BestEdgeTestParameters(true,false,false,5.0,5.0,3.0,false,false,false,false),
            RivaraProduction1BestEdgeTestParameters(false,true,true,5.0,5.0,3.0,false,false,false,false),
            RivaraProduction1BestEdgeTestParameters(false,true,false,5.0,5.0,3.0,false,false,false,true),
            RivaraProduction1BestEdgeTestParameters(false,false,true,5.0,5.0,3.0,false,false,false,false),
            RivaraProduction1BestEdgeTestParameters(true,true,true,5.0,5.0,3.0,false,true,true,false),
            RivaraProduction1BestEdgeTestParameters(true,true,false,5.0,5.0,3.0,false,true,true,false),
            RivaraProduction1BestEdgeTestParameters(true,false,true,5.0,5.0,3.0,false,true,true,false),
            RivaraProduction1BestEdgeTestParameters(true,false,false,5.0,5.0,3.0,false,true,true,false),
            RivaraProduction1BestEdgeTestParameters(false,true,true,5.0,5.0,3.0,false,true,true,false),
            RivaraProduction1BestEdgeTestParameters(false,true,false,5.0,5.0,3.0,false,true,true,false),
            RivaraProduction1BestEdgeTestParameters(false,false,true,5.0,5.0,3.0,false,true,true,false),
            RivaraProduction1BestEdgeTestParameters(false,false,false,5.0,5.0,3.0,false,true,true,false),
            RivaraProduction1BestEdgeTestParameters(true,true,true,5.0,5.0,3.0,true,false,false,true),
            RivaraProduction1BestEdgeTestParameters(true,true,false,5.0,5.0,3.0,true,false,false,true),
            RivaraProduction1BestEdgeTestParameters(true,false,true,5.0,5.0,3.0,true,false,false,true),
            RivaraProduction1BestEdgeTestParameters(true,false,false,5.0,5.0,3.0,true,false,false,true),
            RivaraProduction1BestEdgeTestParameters(false,true,true,5.0,5.0,3.0,true,false,false,true),
            RivaraProduction1BestEdgeTestParameters(false,true,false,5.0,5.0,3.0,true,false,false,true),
            RivaraProduction1BestEdgeTestParameters(false,false,true,5.0,5.0,3.0,true,false,false,true),
            RivaraProduction1BestEdgeTestParameters(false,false,false,5.0,5.0,3.0,true,false,false,true)
        ));
}

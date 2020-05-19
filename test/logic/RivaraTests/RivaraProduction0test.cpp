#include "gtest/gtest.h" 
#include "RivaraUtils.hpp"
#include "Pixel.hpp"
#include "CachedGraph.hpp"
#include "Image.hpp"
#include "RivaraP0.hpp"

namespace Rivara
{
    TEST(RivaraProduction0Test, P0TestPerform)
    {
        auto graph = std::make_shared<CachedGraph>();
        auto image = std::make_shared<Image>(30,30);
        Pixel S;
        S.label=NODELABEL_S;
        S.attributes = std::make_unique<RivaraAttributes>();
        
        RivaraP0 p0(graph, graph->AddVertex(S), image);
        p0.Perform();
        auto nNodes = graph -> GetCacheIterator(NODELABEL_N);
        auto tNodes = graph -> GetCacheIterator(NODELABEL_T);
        auto eNodes = graph -> GetCacheIterator(NODELABEL_E);
        ASSERT_EQ(nNodes.size(), 3);
        ASSERT_EQ(tNodes.size(), 1);
        ASSERT_EQ(eNodes.size(), 3);
        for(auto node:nNodes)
        {
            ASSERT_TRUE((*graph)[node].attributes!=NULL);
            ASSERT_EQ(graph->GetAdjacentVertices(node, NODELABEL_T).size(),1);
            ASSERT_EQ(graph->GetAdjacentVertices(node, NODELABEL_E).size(),2);
            double x = (*graph)[node].attributes->GetDouble(RIVARA_ATTRIBUTE_X);
            double y = (*graph)[node].attributes->GetDouble(RIVARA_ATTRIBUTE_Y);
            ASSERT_TRUE(
                (abs(x)<0.0000001 && abs(y-29)<0.0000001) 
                || (abs(x-29)<0.0000001 && abs(y-29)<0.0000001) 
                || (abs(x-14)<0.0000001 && abs(y)<0.0000001) );
        }
        for(auto node:tNodes)
        {
            ASSERT_TRUE((*graph)[node].attributes!=NULL);
        }
        for(auto node:eNodes)
        {
            ASSERT_TRUE((*graph)[node].attributes!=NULL);
        }
        for(auto tNode: tNodes)
        {
            auto vertices = graph -> GetAdjacentVertices(tNode);
            for(auto vertex : vertices)
            {
                ASSERT_EQ((*graph)[vertex].label, NODELABEL_N);
            }
        }
    }   
}
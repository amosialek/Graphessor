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
        }
        for(auto node:tNodes)
        {
            ASSERT_TRUE((*graph)[node].attributes!=NULL);
        }
        for(auto node:eNodes)
        {
            ASSERT_TRUE((*graph)[node].attributes!=NULL);
        }
    }

    
}
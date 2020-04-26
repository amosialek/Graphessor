#include "P1.hpp"
#include <algorithm>
#include "RivaraUtils.hpp"

namespace Rivara
{
    P1::P1(std::shared_ptr<IGraph> graph,
            vertex_descriptor EEdge,
            vertex_descriptor TEdge,
            std::shared_ptr<Image> image) 
            :
                graph(graph), 
                EEdge(EEdge),
                TEdge(TEdge),
                image(image)
        {}

    void P1::Perform()
    {
        throw NotImplementedException();
    }

    std::unique_ptr<std::vector<P1>> P1::FindAllMatches(std::shared_ptr<CachedGraph> g, std::shared_ptr<Image> image)
    {
        auto result = std::make_unique<std::vector<P1>>();
        auto triangles = g -> GetCacheIterator(NODELABEL_T);
        for(auto t : triangles)
        {
            if(!(*g)[t].attributes -> GetBool(RIVARA_ATTRIBUTE_R)) //if not marked
                continue;
            auto vertices = g -> GetAdjacentVertices(t);
            std::vector<vertex_descriptor> secondEEdges;
            for(auto v : vertices)
            {
                auto temp = g -> GetAdjacentVertices(v, NODELABEL_E);
                std::copy(temp.begin(), temp.end(), back_inserter(secondEEdges));
            } 
            std::sort(secondEEdges.begin(), secondEEdges.end());
            std::vector<vertex_descriptor> commonEEdges;
            for(size_t i = 0; i < secondEEdges.size() - 1; i++)
            {
                if(secondEEdges[i] == secondEEdges[i+1])
                {
                    commonEEdges.push_back(secondEEdges[i]);
                }
            }

            if(commonEEdges.size() == 3) //checks whether there is any hanging node (3 means none)
            {
                auto maybeBestEdge = P1::GetBestEdge(g, commonEEdges);
                if(maybeBestEdge.has_value())
                {
                    result->emplace_back(g, t, maybeBestEdge.value(), image);
                }
            }
        }
        return result;
    }

    std::optional<vertex_descriptor> P1::GetBestEdge(std::shared_ptr<CachedGraph> g, std::vector<vertex_descriptor> EEdges)
    {
        std::vector<Pixel> commonEEdgesPixels; 
        for(auto EEdge : EEdges)
        {
            commonEEdgesPixels.push_back((*g)[EEdge]);
        }
        std::sort(commonEEdgesPixels.begin(), commonEEdgesPixels.end(), EdgeComparator);
        double maxLength = commonEEdgesPixels.back().attributes->GetDouble(RIVARA_ATTRIBUTE_L);
        for(auto e : EEdges)
        {
            Pixel p = (*g)[e];
            if(p.attributes->GetBool(RIVARA_ATTRIBUTE_B) 
                and p.attributes->GetDouble(RIVARA_ATTRIBUTE_L)==maxLength)
            {    
                return e; //longest edge at the border
            }
        }
        for(auto e : EEdges)
        {
            Pixel p = (*g)[e];
            if(p.attributes->GetDouble(RIVARA_ATTRIBUTE_L)==maxLength)
            {
                auto commonNodes = g -> GetAdjacentVertices(e, NODELABEL_N);
                if(true
                    and !(*g)[commonNodes[0]].attributes->GetBool(RIVARA_ATTRIBUTE_HN)
                    and !(*g)[commonNodes[1]].attributes->GetBool(RIVARA_ATTRIBUTE_HN))
                {    
                    return e; //longest edge without hanging nodes
                }
            }
        }
        return {}; //suitable edge not found
    }

    std::vector<uint8_t> P1::Serialize()
    {
        throw NotImplementedException();
    }
}
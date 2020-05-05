#include "RivaraP7.hpp"

namespace Rivara
{
    RivaraP7::RivaraP7(std::shared_ptr<IGraph> graph,
            vertex_descriptor TEdge) 
            :
                graph(graph), 
                TEdge(TEdge)
        {}

    void RivaraP7::Perform()
    {
        
            (*graph)[TEdge].attributes->SetBool(RIVARA_ATTRIBUTE_R, true);
    }

    std::unique_ptr<std::vector<RivaraP7>> RivaraP7::FindAllMatches(std::shared_ptr<CachedGraph> g, std::shared_ptr<Image> image)
    {
        std::unique_ptr<std::vector<RivaraP7>> result = std::make_unique<std::vector<RivaraP7>>();
        auto triangles = g -> GetCacheIterator(NODELABEL_T);
        for(auto triangle : triangles)
        {
            auto vertices = g -> GetAdjacentVertices(triangle);
            image -> CompareWithInterpolation
        }
        return result; 
    }
    

}
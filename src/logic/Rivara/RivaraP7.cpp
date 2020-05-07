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

    std::unique_ptr<std::vector<RivaraP7>> RivaraP7::FindAllMatches(std::shared_ptr<CachedGraph> g, std::shared_ptr<Image> image, int channel, double epsilon)
    {
        std::unique_ptr<std::vector<RivaraP7>> result = std::make_unique<std::vector<RivaraP7>>();
        auto triangles = g -> GetCacheIterator(NODELABEL_T);
        double maxError = 0;
        std::map<vertex_descriptor, double> triangleErrors;
        for(auto triangle : triangles)
        {
            auto vertices = g -> GetAdjacentVertices(triangle);
            double error = image -> CompareWithInterpolation(
                (*g)[vertices[0]].attributes->GetDouble(RIVARA_ATTRIBUTE_X),
                (*g)[vertices[1]].attributes->GetDouble(RIVARA_ATTRIBUTE_X),
                (*g)[vertices[2]].attributes->GetDouble(RIVARA_ATTRIBUTE_X),
                (*g)[vertices[0]].attributes->GetDouble(RIVARA_ATTRIBUTE_Y),
                (*g)[vertices[1]].attributes->GetDouble(RIVARA_ATTRIBUTE_Y),
                (*g)[vertices[2]].attributes->GetDouble(RIVARA_ATTRIBUTE_Y),
                channel
            );
            maxError = std::max(maxError, error);
            triangleErrors[triangle] = error;
        }
        if(maxError>epsilon)
        {
            for(auto triangle : triangleErrors)
            {
                if(triangle.second>0.33 * maxError)
                {
                    result->emplace_back(g, triangle.first);
                }
            }
        }
        return result; 
    }
    

}
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
        spdlog::debug("Rivara P7");
        std::unique_ptr<std::vector<RivaraP7>> result = std::make_unique<std::vector<RivaraP7>>();
        auto triangles = g -> GetCacheIterator(NODELABEL_T);
        double maxError = 0;
        std::map<vertex_descriptor, double> triangleErrors;
        for(auto triangle : triangles)
        {
            auto vertices = g -> GetAdjacentVertices(triangle);
            double x1 = (*g)[vertices[0]].attributes->GetDouble(RIVARA_ATTRIBUTE_X);
            double x2 = (*g)[vertices[1]].attributes->GetDouble(RIVARA_ATTRIBUTE_X);
            double x3 = (*g)[vertices[2]].attributes->GetDouble(RIVARA_ATTRIBUTE_X);
            double y1 = (*g)[vertices[0]].attributes->GetDouble(RIVARA_ATTRIBUTE_Y);
            double y2 = (*g)[vertices[1]].attributes->GetDouble(RIVARA_ATTRIBUTE_Y);
            double y3 = (*g)[vertices[2]].attributes->GetDouble(RIVARA_ATTRIBUTE_Y);
            if(abs(x1*y2+x2*y3+x3*y1-x1*y3-x2*y1-x3*y2)>400)
            {    
                if((*g)[triangle].error == -1)
                    (*g)[triangle].error = image -> CompareWithInterpolation(
                        x1, x2, x3, y1, y2, y3, channel
                    );
                double error = (*g)[triangle].error;
                maxError = std::max(maxError, error);
                triangleErrors[triangle] = error;
            }
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
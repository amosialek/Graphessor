
#include "RivaraCachedGraph.hpp"
#include "RivaraAttributes.hpp"

namespace Rivara
{
    std::vector<std::vector<Pixel>> RivaraCachedGraph::GetPixelsForBilinearInterpolation()
    {
        return std::vector<std::vector<Pixel>>();
    }
    std::vector<std::vector<Pixel>> RivaraCachedGraph::GetPixelsForBaricentricInterpolation()
    {
        std::vector<std::vector<Pixel>> result;
        auto TEdges = GetCacheIterator(NODELABEL_T);
        for(auto tEdge : TEdges)
        {
            auto vertices = GetAdjacentVertices(tEdge);
            std::vector<Pixel> verticesPixels;
            for(auto vertex: vertices)
            {
                verticesPixels.emplace_back(this->operator[](vertex));
            }
            result.emplace_back(verticesPixels);
        }
        return result;
    }
    const std::set<vertex_descriptor>& RivaraCachedGraph::GetPixels()
    {
        return GetCacheIterator(NODELABEL_N);
    }
}
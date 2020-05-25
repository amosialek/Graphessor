
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
    void RivaraCachedGraph::RegisterHangingNode(vertex_descriptor v)
    {
        InsertCacheElement("H", v);
    }
    std::set<vertex_descriptor> RivaraCachedGraph::GetHangingNodes()
    {
        return GetCacheIterator("H");
    }
    void RivaraCachedGraph::DeregisterHangingNode(vertex_descriptor v)
    {
        typeToVerticesCache["H"]->erase(v);
    }
    vertex_descriptor RivaraCachedGraph::AddVertex(Pixel p)
    {
        std::vector<vertex_descriptor> noLongerHangingNodes;
        auto hangingNodes = GetCacheIterator("H");
        for(auto hn : hangingNodes)
            if(!this->operator[](hn).attributes->GetBool(RIVARA_ATTRIBUTE_HN))
                noLongerHangingNodes.emplace_back(hn);
        vertex_descriptor v = CachedGraph::AddVertex(p);
        if(p.attributes->GetBool(RIVARA_ATTRIBUTE_HN))
        {
            RegisterHangingNode(v);
        }
        for(auto nlhn:noLongerHangingNodes)
            DeregisterHangingNode(nlhn);
        return v;
    } 
}
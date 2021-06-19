#include "CachedGraph.hpp"
#include "spdlog/spdlog.h"
#include "GraphessorConstants.hpp"
#include <cereal/archives/json.hpp>
#include <cereal/types/vector.hpp>
#include <cereal/types/base_class.hpp>
#include <cereal/types/polymorphic.hpp>

void CachedGraph::EnsureCacheExists(std::string type)
{
    std::map<std::string, std::shared_ptr<std::set<vertex_descriptor>>>::iterator it = typeToVerticesCache.find(type);
    if(it==typeToVerticesCache.end())
    {
        typeToVerticesCache[type] = std::make_shared<std::set<vertex_descriptor>>();
    }
}
void CachedGraph::InsertCacheElement(std::string type, vertex_descriptor v)
{
    EnsureCacheExists(type);
    {
        typeToVerticesCache[type] -> emplace(v);
    }
}
void CachedGraph::ChangeCachedElementType(vertex_descriptor v, std::string type1, std::string type2)
{
    EnsureCacheExists(type1);
    EnsureCacheExists(type2);
    typeToVerticesCache[type1] -> erase(v);
    typeToVerticesCache[type2] -> emplace(v);
    spdlog::debug("emplaced {} to {}", v, type2);  
}

CachedGraph::CachedGraph()
{
    graph = std::make_unique<PixelGraph>();
}

CachedGraph::CachedGraph(const CachedGraph& other)
{
    this->graph = std::make_unique<PixelGraph>((*(other.graph)));
    this->typeToVerticesCache = other.typeToVerticesCache;
}

void CachedGraph::AddEdge(vertex_descriptor v1,vertex_descriptor v2) 
{
    add_edge(v1,v2,*graph);
}

vertex_descriptor CachedGraph::AddVertex(Pixel p) 
{
    vertex_descriptor v = add_vertex(p, *graph);
    InsertCacheElement(p.label, v);
    return v;
}

void CachedGraph::RemoveEdge(vertex_descriptor v1,vertex_descriptor v2) 
{
    remove_edge(v1,v2,*graph);
}

void CachedGraph::ChangeVertexType(vertex_descriptor v, std::string type) 
{
    ChangeCachedElementType(v, (*graph)[v].label, type);
    (*graph)[v].label = type;
}

Pixel& CachedGraph::operator[](vertex_descriptor v) 
{
    return (*graph)[v];
}

std::set<vertex_descriptor>& CachedGraph::GetCacheIterator(std::string type)
{
    EnsureCacheExists(type);
    return *(typeToVerticesCache[type]);
}

std::vector<vertex_descriptor> CachedGraph::GetAdjacentVertices(vertex_descriptor v)
{
    std::vector<vertex_descriptor> result;
    
    adjacency_iterator currentNeighbour, endOfNeighbours;
    std::tie(currentNeighbour, endOfNeighbours) = adjacent_vertices(v, *graph);
    for(;currentNeighbour!=endOfNeighbours;++currentNeighbour)
    {
        result.push_back(*currentNeighbour);
    }
    return result;
}

std::vector<vertex_descriptor> CachedGraph::GetAdjacentVertices(vertex_descriptor v, std::string type)
{
    std::vector<vertex_descriptor> result;
    
    adjacency_iterator currentNeighbour, endOfNeighbours;
    std::tie(currentNeighbour, endOfNeighbours) = adjacent_vertices(v, *graph);
    for(;currentNeighbour!=endOfNeighbours;++currentNeighbour)
    {
        if(this-> operator[](*currentNeighbour).label==type)
            result.push_back(*currentNeighbour);
    }
    return result;
}

PixelGraph CachedGraph::GetGraph()
{
    return *graph;
}

std::vector<std::vector<Pixel>> CachedGraph::GetPixelsForBilinearInterpolation()
{
    std::vector<std::vector<Pixel>> result;
    for(auto v : GetCacheIterator(NODELABEL_I))
    {
        auto adjacentPixels = GetAdjacentVertices(v);
        if(adjacentPixels.size()==4)
        {
            std::vector<Pixel> s;
            for(auto adjacentPixel : adjacentPixels)
                s.emplace_back(this->operator[](adjacentPixel));
            result.emplace_back(s);
        }
    }
    return result;
}
std::vector<std::vector<Pixel>> CachedGraph::GetPixelsForBaricentricInterpolation()
{
    std::vector<std::vector<Pixel>> result;
    auto FEdges = GetCacheIterator(NODELABEL_F);
    auto IEdges = GetCacheIterator(NODELABEL_I);
    std::set<vertex_descriptor> danglingFedges;
    for(auto fEdge : FEdges)
    {
        if(GetAdjacentVertices(fEdge).size()==1)
            danglingFedges.insert(fEdge);
    }
    for(auto f : danglingFedges)
    {
        auto adjacentPixel = GetAdjacentVertices(f)[0];
        auto adjacentIEdges = GetAdjacentVertices(adjacentPixel,NODELABEL_I);

        std::sort(adjacentIEdges.begin(), adjacentIEdges.end(), [f, this](const vertex_descriptor& v1, const vertex_descriptor& v2)->bool
            {
                auto fx = (*this)[f].x;
                auto fy = (*this)[f].y;
                auto v1x = (*this)[v1].x;
                auto v1y = (*this)[v1].y;
                auto v2x = (*this)[v2].x;
                auto v2y = (*this)[v2].y;
                return (v1x-fx)*(v1x-fx)+(v1y-fy)*(v1y-fy) < (v2x-fx)*(v2x-fx)+(v2y-fy)*(v2y-fy);
            });
        
        std::set<vertex_descriptor> closerPixels;
        std::set<vertex_descriptor> outerPixels;
        auto a0 = GetAdjacentVertices(adjacentIEdges[0], NODELABEL_P);
        auto a1 = GetAdjacentVertices(adjacentIEdges[1], NODELABEL_P);
        std::copy(a0.begin(),a0.end(),std::inserter(closerPixels, closerPixels.end()));
        std::copy(a1.begin(),a1.end(),std::inserter(closerPixels, closerPixels.end()));
        a0 = GetAdjacentVertices(adjacentIEdges[2], NODELABEL_P);
        a1 = GetAdjacentVertices(adjacentIEdges[3], NODELABEL_P);
        std::copy(a0.begin(),a0.end(),std::inserter(outerPixels, outerPixels.end()));
        std::copy(a1.begin(),a1.end(),std::inserter(outerPixels, outerPixels.end()));

        std::vector<vertex_descriptor> otherTriangleVertices;

        std::set_difference(closerPixels.begin(), closerPixels.end(), outerPixels.begin(), outerPixels.end(), std::inserter(otherTriangleVertices, otherTriangleVertices.begin()));
        std::vector<Pixel> r
        {
            graph -> operator[](adjacentPixel),
            graph -> operator[](otherTriangleVertices[0]),
            graph -> operator[](otherTriangleVertices[1])
        }; 
        result.emplace_back(r);
    }

    for(auto IEdge : IEdges)
    {
        auto adjacentPixels = GetAdjacentVertices(IEdge);
        if(adjacentPixels.size()==3)
        {
            std::vector<Pixel> r
            {
                graph -> operator[](adjacentPixels[0]),
                graph -> operator[](adjacentPixels[1]),
                graph -> operator[](adjacentPixels[2])
            };
            result.emplace_back(r);
        }
    }

    return result;
}

std::vector<std::vector<Pixel>> CachedGraph::GetPixelsForSVDInterpolation()
{
    std::vector<std::vector<Pixel>> s;
    return s;
}

const std::set<vertex_descriptor>& CachedGraph::GetPixels()
{
    return GetCacheIterator(NODELABEL_P);
}

void CachedGraph::Serialize(std::ostream& stream)
{
    PixelGraph::vertex_iterator v, vend;
    PixelGraph::edge_iterator e, eend;
    std::set<vertex_descriptor> vertices;
    std::vector<Pixel> pixels;
    std::set<std::pair<vertex_descriptor, vertex_descriptor>> edges;
    for (boost::tie(v, vend) = boost::vertices(*graph); v != vend; ++v)
    {
        vertices.insert(*v);
        pixels.emplace_back((*graph)[*v]);
    }
    for (boost::tie(e, eend) = boost::edges(*graph); e != eend; ++e)
    {
        edges.insert(std::make_pair(boost::source(*e, *graph), boost::target(*e, *graph)));
    }
    stream<<vertices.size()<<" "<<edges.size()<<std::endl;
    for(auto v : vertices)
    {
        stream<<v<<std::endl;
    }
    for(auto e : edges)
    {
        stream<<e.first<<" "<<e.second<<std::endl;
    }

    cereal::JSONOutputArchive jsonArchive(stream, cereal::JSONOutputArchive::Options::NoIndent());
    jsonArchive(pixels);
}  

void CachedGraph::Deserialize(std::istream& stream)
{
    std::set<vertex_descriptor> vertices;
    std::vector<Pixel> pixels;
    std::set<std::pair<vertex_descriptor, vertex_descriptor>> edges;
    int n,m;
    stream>>n>>m;
    vertex_descriptor v1,v2;
    for(int i=0;i<n;i++)
    {
        stream>>v1;
        vertices.insert(v1);
    }
    for(int i=0;i<m;i++)
    {
        stream>>v1>>v2;
        edges.insert(std::make_pair(v1,v2));
    }
    cereal::JSONInputArchive jsonArchive(stream);
    jsonArchive(pixels);
    for(auto p : pixels)
    {
        AddVertex(p);
    }
    for(auto e : edges)
    {
        AddEdge(e.first,e.second);
    }
}
void CachedGraph::DecreaseXAndYByRatio(int ratio)
{
    PixelGraph::vertex_iterator v, vend;
    for (boost::tie(v, vend) = boost::vertices(*graph); v != vend; ++v)
    {
        (*graph)[*v].x/=ratio;
        (*graph)[*v].y/=ratio;
    }
}
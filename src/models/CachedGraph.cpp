#include "CachedGraph.hpp"
#include "spdlog/spdlog.h"

void CachedGraph::EnsureCacheExists(std::string type)
{
    std::map<std::string, std::unique_ptr<std::set<vertex_descriptor>>>::iterator it = typeToVerticesCache.find(type);
    if(it==typeToVerticesCache.end())
    {
        typeToVerticesCache[type] = std::make_unique<std::set<vertex_descriptor>>();
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

const std::set<vertex_descriptor>& CachedGraph::GetCacheIterator(std::string type)
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
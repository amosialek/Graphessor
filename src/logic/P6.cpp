#include "P6.hpp"

std::queue<vertex_descriptor> toBeVisited;

std::vector<uint8_t> P6::Serialize()
{
    throw NotImplementedException();
}

P6::P6(std::shared_ptr<IGraph> graph, 
    vertex_descriptor IEdge):
        graph(graph),
        IEdge(IEdge)
        {}

auto P6::PerformAndGetAdjacentEdges()
{
    spdlog::debug("P6 {}", IEdge);    
    std::vector<vertex_descriptor> adjacentEdges;
    auto adjacentVertices = graph -> GetAdjacentVertices(IEdge);
    for(auto v: adjacentVertices)
    {
        auto adjacentEdgesTemp = graph -> GetAdjacentVertices(v);
        auto adjacentEdgesTemp2 = adjacentEdgesTemp 
        | linq::where([this, v](vertex_descriptor IEdge2)
        {
            return haveCommonEdge(*graph, IEdge, IEdge2, v);
        });
        adjacentEdges.insert(adjacentEdges.end(), adjacentEdgesTemp2.begin(), adjacentEdgesTemp2.end());
    }
    // auto adjacentIEdges = adjacentEdges 
    //     | linq::where([this](vertex_descriptor v){
    //         return true
    //             && (*graph)[v].label==NODELABEL_I 
    //             && (*graph)[v]._break==0 
    //             && (*graph)[v].breakLevel<(*graph)[IEdge].breakLevel;}
    //             );
    std::vector<vertex_descriptor> adjacentIEdges;
    for(auto a : adjacentEdges)
    {
        if(true
           && (*graph)[a]._break==0 
           && (*graph)[a].label==NODELABEL_I 
           && (*graph)[a].breakLevel<(*graph)[IEdge].breakLevel)
            {
                (*graph)[a]._break=1;
                adjacentIEdges.emplace_back(a);
            }
        
    }
    return adjacentIEdges;
}

std::unique_ptr<std::vector<P6>> P6::PerformAllMatches(std::shared_ptr<CachedGraph> graph)
{
    auto result = std::make_unique<std::vector<P6>>();
    for(auto IEdge: graph->GetCacheIterator(NODELABEL_I))
    {
        (*graph)[IEdge].visited = 0;
    }
    for(auto IEdge: graph->GetCacheIterator(NODELABEL_I))
    {
        if((*graph)[IEdge]._break and !(*graph)[IEdge].visited)
        {
            std::vector<vertex_descriptor> adjacentEdges;
            toBeVisited.push(IEdge);
            spdlog::debug("pushing P6 {}", IEdge);    

            while(!toBeVisited.empty()) //BFS through IEdges with break==0
            {
                long currentIEdge = toBeVisited.front();
                toBeVisited.pop();
                result->emplace_back(graph, currentIEdge);
                for(auto a : result->back().PerformAndGetAdjacentEdges())
                {
                    spdlog::debug("pushing2 P6 {}", a);  
                    toBeVisited.push(a);
                }
            }
        }
    }
    return result;
}

void P6::Perform()
{
    PerformAndGetAdjacentEdges();
}
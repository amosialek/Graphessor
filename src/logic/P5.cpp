#include "P5.hpp"

std::map<vertex_descriptor,std::vector<vertex_descriptor>> vertexToNeighbours;
std::map<vertex_descriptor, double> IEdgeToError;

std::vector<uint8_t> P5::Serialize()
{
    throw NotImplementedException();
}

P5::P5(
    std::shared_ptr<IGraph> graph, 
    vertex_descriptor IEdge):
        graph(graph),
        IEdge(IEdge)
    {}

void P5::Perform()
{
    spdlog::debug("P5 {}", IEdge);    
    (*graph)[IEdge]._break = 1;
}

std::unique_ptr<std::vector<P5>> P5::FindAllMatches(std::shared_ptr<CachedGraph> graph,
    std::shared_ptr<Image> image,
    int channel,
    double epsilon)
{
    auto result = std::make_unique<std::vector<P5>>();
    double sumError = 0;
    double maxError = 0;
    vertexToNeighbours.clear();
    int width = image->width();
    int height = image->height();
    for(auto v : graph -> GetCacheIterator(NODELABEL_I))
        vertexToNeighbours[v] = graph -> GetAdjacentVertices(v); 
    auto IEdges = graph -> GetCacheIterator(NODELABEL_I);
    for(auto IEdge : IEdges)
    {
        // spdlog::debug("TOPKEK");  
        int minx = width;
        int maxx = 0;
        int miny = height;
        int maxy = 0;
        for(auto v : vertexToNeighbours[IEdge])
        {
            minx = std::min(minx, (*graph)[v].x);
            maxx = std::max(maxx, (*graph)[v].x);
            miny = std::min(miny, (*graph)[v].y);
            maxy = std::max(maxy, (*graph)[v].y); 
        } 
        double error = image->CompareWithInterpolation(minx, maxx, miny, maxy, channel);
        IEdgeToError[IEdge] = error;
        sumError += error;
        maxError = std::max(maxError, error);
    //std::cout<<"error: "<<error<<" "<<minx<<" "<<maxx<<" "<<miny<<" "<<maxy<<std::endl;
    }
    spdlog::debug("MaxErrorFound: {}", maxError);  
    if(maxError > epsilon)
        for(auto IEdge : IEdges)
        {
            if(IEdgeToError[IEdge] > 0.33 * maxError && IEdgeToError[IEdge] > epsilon)
                {
                    result -> emplace_back(graph, IEdge);
                }
        }
    spdlog::debug("Found {} P5 Matches", result->size());  
    return result;
}
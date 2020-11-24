#include "P5.hpp"
#include "PbiHelper.cpp"

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
    std::shared_ptr<Array2D> image,
    std::shared_ptr<Array2D> interpolation,
    int channel,
    double epsilon)
{
    auto result = std::make_unique<std::vector<P5>>();
    double sumError = 0;
    double maxError = 0;
    vertexToNeighbours.clear();
    int width = image->width;
    int height = image->height;
    for(auto v : graph -> GetCacheIterator(NODELABEL_I))
        vertexToNeighbours[v] = graph -> GetAdjacentVertices(v); 
    auto IEdges = graph -> GetCacheIterator(NODELABEL_I);
    for(auto IEdge : IEdges)
    { 
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
        if((*graph)[IEdge].error < 0)
        {
            //interpolation -> FillWith(0);
            interpolation -> BilinearInterpolation(*image, minx, maxx, miny, maxy);
            auto minyFuncoefficient = GetSquareInterpolationOfEdge(*interpolation, minx, maxx, miny);
            auto maxyFuncoefficient = GetSquareInterpolationOfEdge(*interpolation, minx, maxx, maxy);
            auto minxFuncoefficient = GetSquareInterpolationOfYEdge(*interpolation, minx, miny, maxy);
            auto maxxFuncoefficient = GetSquareInterpolationOfYEdge(*interpolation, maxx, miny, maxy);
            interpolation->Subtract([minx, maxx, miny, maxy, minyFuncoefficient](double x, double y){return -minyFuncoefficient*(maxx-x)*(x-minx)*(maxy-y)/(maxy-miny);},minx, maxx, miny, maxy);
            interpolation->Subtract([minx, maxx, miny, maxy, maxyFuncoefficient](double x, double y){return -maxyFuncoefficient*(maxx-x)*(x-minx)*(y-miny)/(maxy-miny);},minx, maxx, miny, maxy);
            interpolation->Subtract([minx, maxx, miny, maxy, minxFuncoefficient](double x, double y){return -minxFuncoefficient*(maxy-y)*(y-miny)*(maxx-x)/(maxx-minx);},minx, maxx, miny, maxy);
            interpolation->Subtract([minx, maxx, miny, maxy, maxxFuncoefficient](double x, double y){return -maxxFuncoefficient*(maxy-y)*(y-miny)*(x-minx)/(maxx-minx);},minx, maxx, miny, maxy);

            (*graph)[IEdge].error = image->CompareWith(*interpolation, minx, maxx, miny, maxy);
        }
        IEdgeToError[IEdge] = (*graph)[IEdge].error;
        sumError += (*graph)[IEdge].error;
        maxError = std::max(maxError, (*graph)[IEdge].error);
    //std::cout<<"error: "<<error<<" "<<minx<<" "<<maxx<<" "<<miny<<" "<<maxy<<std::endl;
    }
    spdlog::debug("MaxErrorFound: {}", maxError);  
    if(maxError > epsilon)
        for(auto IEdge : IEdges)
        {
            if(/*IEdgeToError[IEdge] > 0.33 * maxError && */ IEdgeToError[IEdge] > epsilon)
                {
                    result -> emplace_back(graph, IEdge);
                }
        }
    spdlog::debug("Found {} P5 Matches", result->size());  
    return result;
}
#include "P5.hpp"
#include "PbiHelper.hpp"

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
            auto imageCopy = image->GetCopy(minx, maxx, miny, maxy);
            auto interpolationCopy = interpolation->GetCopy(minx, maxx, miny, maxy);

            //auto tmperror = image->CompareWith(*interpolation, minx, maxx, miny, maxy);

            imageCopy.Subtract(interpolationCopy);
            int xDiff = maxx-minx;
            int yDiff = maxy-miny;
            auto minyFuncoefficient = GetSquareInterpolationOfEdge(imageCopy, 0, xDiff, 0);

            auto coefficients = GetInterpolationsOfEdgeOfDifferentOrders(imageCopy, 0, xDiff, yDiff/2);
            auto coefficients2 = GetInterpolationsOfRectangleOfDifferentOrders(imageCopy, 0, xDiff, 0, yDiff);

            auto maxyFuncoefficient = GetSquareInterpolationOfEdge(imageCopy, 0, xDiff, yDiff);
            auto minxFuncoefficient = GetSquareInterpolationOfYEdge(imageCopy, 0, 0, yDiff);
            auto maxxFuncoefficient = GetSquareInterpolationOfYEdge(imageCopy, xDiff, 0, yDiff);
            delete [] coefficients;
            delete [] coefficients2;
            interpolation->Subtract([xDiff, maxy, yDiff, minyFuncoefficient](double x, double y){return minyFuncoefficient*(xDiff-x)*(x)*(maxy-y)/(yDiff);},minx, maxx, miny, maxy, minx, 0);
            interpolation->Subtract([xDiff, yDiff, miny, maxyFuncoefficient](double x, double y){return maxyFuncoefficient*(xDiff-x)*(x)*(y-miny)/(yDiff);},minx, maxx, miny, maxy, minx, 0);
            interpolation->Subtract([xDiff, maxx, yDiff, minxFuncoefficient](double x, double y){return minxFuncoefficient*(yDiff-y)*(y)*(maxx-x)/(xDiff);},minx, maxx, miny, maxy, 0, miny);
            interpolation->Subtract([minx, xDiff, yDiff, maxxFuncoefficient](double x, double y){return maxxFuncoefficient*(yDiff-y)*(y)*(x-minx)/(xDiff);},minx, maxx, miny, maxy, 0, miny);

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
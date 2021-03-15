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
    double epsilon,
    int orders)
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

            imageCopy.Subtract(interpolationCopy);
            int xDiff = maxx-minx;
            int yDiff = maxy-miny;
            interpolationCopy.FillWith(0);
            auto minyFuncoefficients = GetInterpolationsOfEdgeOfDifferentOrders(imageCopy, 0, xDiff, 0, 0, orders);
            auto maxyFuncoefficients = GetInterpolationsOfEdgeOfDifferentOrders(imageCopy, 0, xDiff, yDiff, yDiff, orders);
            auto minxFuncoefficients = GetInterpolationsOfEdgeOfDifferentOrders(imageCopy, 0, 0, 0, yDiff, orders);
            auto maxxFuncoefficients = GetInterpolationsOfEdgeOfDifferentOrders(imageCopy, xDiff, xDiff, 0, yDiff, orders);
            for(int i=0;i<orders;i++)
            {
                double minyFuncoefficient = minyFuncoefficients[i];
                double maxyFuncoefficient = maxyFuncoefficients[i];
                double minxFuncoefficient = minxFuncoefficients[i];
                double maxxFuncoefficient = maxxFuncoefficients[i];
                auto splitXFunction = GetFunctionSplitToNElements(testFunctions[i+2],0,1,xDiff+1);
                auto splitYFunction = GetFunctionSplitToNElements(testFunctions[i+2],0,1,yDiff+1);
                interpolationCopy.Subtract([xDiff, maxy, yDiff, minyFuncoefficient, i, &splitXFunction](double x, double y){return -splitXFunction[x][0]*minyFuncoefficient*(maxy-y)/(yDiff);},0, xDiff, 0, 0, minx, 0);
                interpolationCopy.Subtract([xDiff, yDiff, miny, maxyFuncoefficient, i, &splitXFunction](double x, double y){return -splitXFunction[x][0]*maxyFuncoefficient*(y-miny)/(yDiff);},0, xDiff, yDiff, yDiff, minx, 0);
                interpolationCopy.Subtract([xDiff, maxx, yDiff, minxFuncoefficient, i, &splitYFunction](double x, double y){return -splitYFunction[y][0]*minxFuncoefficient*(maxx-x)/(xDiff);},0, 0, 0, yDiff, 0, miny);
                interpolationCopy.Subtract([minx, xDiff, yDiff, maxxFuncoefficient, i, &splitYFunction](double x, double y){return -splitYFunction[y][0]*maxxFuncoefficient*(x-minx)/(xDiff);},xDiff, xDiff, 0, yDiff, 0, miny);
            }
            imageCopy.Subtract(interpolationCopy);
            interpolationCopy.Multiply(-1);
            interpolation->Subtract(interpolationCopy, minx, maxx, miny, maxy);
            auto coefficients2D = GetInterpolationsOfRectangleOfDifferentOrders(imageCopy, 0, xDiff, 0, yDiff, orders);
            for(int i=0;i<orders;i++)
                for(int j=0;j<orders;j++)
                {
                    auto splitXFunction = GetFunctionSplitToNElements(testFunctions[i+2],0,1,xDiff+1);
                    auto splitYFunction = GetFunctionSplitToNElements(testFunctions[j+2],0,1,yDiff+1);
                    double coefficient = coefficients2D[i*orders+j];
                    interpolation->Subtract([coefficient, i, j, &splitXFunction, &splitYFunction](double x, double y){return -coefficient*splitXFunction[x][0]*splitYFunction[y][0];},minx, maxx, miny, maxy, minx, miny);
                }
            // delete [] coefficients;
            // delete [] coefficients2D;
            

            (*graph)[IEdge].error = image->CompareWith(*interpolation, minx, maxx, miny, maxy);
            int debugVariable = 2;
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
#include<InterpolationHelper.hpp>
#include "PbiHelper.hpp"
std::vector<double> GetSingleVertexFEdgeCoefficients(Array2D & image, IGraph& graph, vertex_descriptor edge, vertex_descriptor vertex, int minx, int miny, int maxx, int maxy, double secondVertexValue, int orders)
{
    std::vector<double> result;
    result.reserve(orders);
    std::function<double(double, double)> linearInterpolation;
    int secondXCoord;
    int secondYCoord;
    double firstValue;
    double secondValue;
    if(graph[vertex].x == graph[edge].x)
    {
        int yDiff = maxy-miny;
        if(graph[vertex].y == miny)
        {
            secondYCoord = maxy;
            firstValue = image[graph[vertex].x][graph[vertex].y];
            secondValue = secondVertexValue;
        }
        else
        {
            secondYCoord = miny;
            secondValue = image[graph[vertex].x][graph[vertex].y];
            firstValue = secondVertexValue;
        }
        linearInterpolation = [yDiff, firstValue, secondValue](double x, double y){return secondValue*y/yDiff + firstValue*(yDiff-y)/yDiff;};
        Array2D imageCopy = image.GetCopy(graph[vertex].x, graph[vertex].x, miny, maxy);
        imageCopy.Subtract(linearInterpolation, 0, 0, 0, yDiff);
        auto coefficientsForFullEdge = GetInterpolationsOfEdgeOfDifferentOrders(imageCopy, 0, 0, 0, yDiff, orders);
        for(int i=0;i<orders;i++)
            result.push_back(coefficientsForFullEdge[i]);
    }
    else if(graph[vertex].y == graph[edge].y)
    {
        int xDiff = maxx-minx;
        if(graph[vertex].x == minx)
        {
            secondXCoord = maxx;
            firstValue = image[graph[vertex].x][graph[vertex].y];
            secondValue = secondVertexValue;
        }
        else
        {
            secondXCoord = minx;
            secondValue = image[graph[vertex].x][graph[vertex].y];
            firstValue = secondVertexValue;
        }
        linearInterpolation = [xDiff, firstValue, secondValue](double x, double y){return secondValue*x/xDiff + firstValue*(xDiff-x)/xDiff;};
        Array2D imageCopy = image.GetCopy(minx, maxx, graph[vertex].y, graph[vertex].y);
        imageCopy.Subtract(linearInterpolation, 0, xDiff, 0, 0);
        auto coefficientsForFullEdge = GetInterpolationsOfEdgeOfDifferentOrders(imageCopy, 0, xDiff, 0, 0, orders);
        for(int i=0;i<orders;i++)
            result.push_back(coefficientsForFullEdge[i]);
    }
    return result;
}
#include<InterpolationHelper.hpp>
#include "PbiHelper.hpp"

template<typename T, typename Pred>
std::set<T> where(std::set<T> inVec, Pred predicate)
{
  std::set<T> result;
  std::copy_if(inVec.begin(), inVec.end(), std::inserter(result, result.begin()), predicate);
  return result;
}

std::set<vertex_descriptor> GetEdges(IGraph& graph, vertex_descriptor iEdge, int minx, int maxx, int miny, int maxy)
{
    auto vertices = graph.GetAdjacentVertices(iEdge);
    std::set<vertex_descriptor> edges;
    for(auto v: vertices)
    {
        auto fedges = graph.GetAdjacentVertices(v, NODELABEL_F);
        auto bedges = graph.GetAdjacentVertices(v, NODELABEL_B);
        edges.insert(fedges.begin(), fedges.end());
        edges.insert(bedges.begin(), bedges.end());
    }
    return where(edges, [minx, maxx, miny, maxy, &graph](vertex_descriptor v){
        return 
        (
            (graph[v].x==minx or graph[v].x==maxx)
            and graph[v].y>=miny 
            and graph[v].y<=maxy
        ) 
        or 
        (
            (graph[v].y==miny or graph[v].y==maxy)
            and graph[v].x>=minx 
            and graph[v].x<=maxx
        );}
    );
}

std::tuple<std::map<int, std::map<int, double>>,std::map<std::string, std::vector<double>>> GetFEdgeWithTwoVerticesCoefficients(Array2D& image, Pixel& v0, Pixel& v1,int minx, int maxx, int miny, int maxy, int orders)
{
    std::map<int, std::map<int, double>> resultCorners;    
    std::map<std::string, std::vector<double>> resultCoefficients;    
    std::unique_ptr<double[]> coefficientsForFullEdge;
    std::vector<double> resultCoefficientsVector;
    resultCoefficientsVector.reserve(orders);
    if(v0.x == v1.x)
    {
        auto imageCopy = image.GetCopy(v1.x, v1.x, std::min(v0.y, v1.y), std::max(v0.y, v1.y));
        double yDiff = abs(v1.y-v0.y);
        double val1 = imageCopy[0][0];
        double val2 = imageCopy[0][yDiff];
        imageCopy.xOffset=0;
        imageCopy.yOffset=0;
        imageCopy.Subtract([val1, val2, yDiff](double x, double y){return val2*y/yDiff + val1*(yDiff-y)/yDiff;});
        coefficientsForFullEdge = GetInterpolationsOfEdgeOfDifferentOrders(imageCopy, 0, 0, 0, yDiff, orders);
        if(v0.y==miny or v0.y==maxy)
            resultCorners[v0.x][v0.y] = image[v0.x][v0.y];
        if(v1.y==maxy or v1.y==miny)
            resultCorners[v1.x][v1.y] = image[v1.x][v1.y];
        if((v1.y!=miny and v1.y!=maxy) or (v0.y!=miny and v0.y!=maxy))
        {
            double midValue = (val1+val2)/2;
            for(int order=0;order<orders;order++)
            {
                midValue += coefficientsForFullEdge[order]*testFunctions[order](0.5,0);
            }
            if(v1.y==miny or v0.y==miny)
                resultCorners[v0.x][maxy] = midValue;
            else
                resultCorners[v0.x][miny] = midValue;
            imageCopy = Array2D(1,101);
            for(int order=0;order<orders;order++)
            {
                auto temporaryArray = GetFunctionSplitToNElements(Multiply(testFunctions[order], coefficientsForFullEdge[order]),0,1,101);
                temporaryArray.Transpose();
                imageCopy.Subtract(temporaryArray);
            }   
            imageCopy.Multiply(-1);
            imageCopy.Subtract([val1, val2](double x, double y){return -(val2*y/100 + val1*(100-y)/100);});
            if(v0.y == miny or v1.y==miny)
            {
                imageCopy.Subtract([val1, midValue](double x, double y){return midValue*y/50.0 + val1*(50-y)/50.0;},0,0,0,50);
                coefficientsForFullEdge = GetInterpolationsOfEdgeOfDifferentOrders(imageCopy, 0, 0, 0, 50, orders);
            }
            else if(v0.y == maxy or v1.y==maxy)
            {
                imageCopy.Subtract([val2, midValue](double x, double y){return val2*y/50.0 + midValue*(50-y)/50.0;},0,0,50,100,0,50);
                coefficientsForFullEdge = GetInterpolationsOfEdgeOfDifferentOrders(imageCopy, 0, 0, 50, 100, orders);
            }
        }
        for(int i=0;i<orders;i++)
            resultCoefficientsVector.push_back(coefficientsForFullEdge[i]);
        if(v0.x==minx)
            resultCoefficients["left"] = resultCoefficientsVector;
        else
            resultCoefficients["right"] = resultCoefficientsVector;
    }
    else if(v0.y == v1.y)
    {
        auto imageCopy = image.GetCopy(std::min(v0.x, v1.x), std::max(v0.x, v1.x), v1.y, v1.y);
        double xDiff = abs(v1.x-v0.x);
        double val1 = imageCopy[0][0];
        double val2 = imageCopy[xDiff][0];
        imageCopy.xOffset=0;
        imageCopy.yOffset=0;
        imageCopy.Subtract([val1, val2, xDiff](double x, double y){return val2*x/xDiff + val1*(xDiff-x)/xDiff;});
        coefficientsForFullEdge = GetInterpolationsOfEdgeOfDifferentOrders(imageCopy, 0, xDiff, 0, 0, orders);
        if(v0.x==minx or v0.x==maxx)
            resultCorners[v0.x][v0.y] = image[v0.x][v0.y];
        if(v1.x==maxx or v1.x==minx)
            resultCorners[v1.x][v1.y] = image[v1.x][v1.y];
        if((v1.x!=minx and v1.x!=maxx) or (v0.x!=minx and v0.x!=maxx))
        {
            double midValue = (val1+val2)/2;
            for(int order=0;order<orders;order++)
            {
                midValue += coefficientsForFullEdge[order]*testFunctions[order](0.5,0);
            }
            if(v1.x==minx or v0.x==minx)
                resultCorners[maxx][v0.y] = midValue;
            else
                resultCorners[minx][v0.y] = midValue;
            
            imageCopy = Array2D(101,1);
            for(int order=0;order<orders;order++)
            {
                imageCopy.Subtract(GetFunctionSplitToNElements(Multiply(testFunctions[order],coefficientsForFullEdge[order]),0,1,101));
            }   
            imageCopy.Multiply(-1);
            imageCopy.Subtract([val1, val2](double x, double y){return -(val2*x/100 + val1*(100-x)/100);});
            if(v0.x == minx or v1.x==minx)
            {
                imageCopy.Subtract([val1, midValue](double x, double y){return midValue*x/50.0 + val1*(50-x)/50.0;},0,50,0,0);
                coefficientsForFullEdge = GetInterpolationsOfEdgeOfDifferentOrders(imageCopy, 0, 50, 0, 0, orders);
            }
            else if(v0.x == maxx or v1.x==maxx)
            {
                imageCopy.Subtract([val2, midValue](double x, double y){return val2*x/50.0 + midValue*(50-x)/50.0;},50,100,0,0,50,0);
                coefficientsForFullEdge = GetInterpolationsOfEdgeOfDifferentOrders(imageCopy, 50, 100, 0, 0, orders);
            }
        }
        for(int i=0;i<orders;i++)
            resultCoefficientsVector.push_back(coefficientsForFullEdge[i]);
        if(v0.y==miny)
            resultCoefficients["top"] = resultCoefficientsVector;
        else
            resultCoefficients["bottom"] = resultCoefficientsVector;
    }
    assert(resultCorners[minx].size() + resultCorners[maxx].size() == 2 and "GetFEdgeWithTwoVerticesCoefficients Wrong number of calculated corners");
    return std::make_tuple(resultCorners, resultCoefficients);
}

std::vector<double> GetSingleVertexFEdgeCoefficients(Array2D & image, IGraph& graph, vertex_descriptor edge, vertex_descriptor vertex, int minx, int miny, int maxx, int maxy, double secondVertexValue, int orders)
{
    std::vector<double> result;
    result.reserve(orders);
    std::function<double(double, double)> linearInterpolation;
    double firstValue;
    double secondValue;
    if(graph[vertex].x == graph[edge].x)
    {
        int yDiff = maxy-miny;
        if(graph[vertex].y == miny)
        {
            firstValue = image[graph[vertex].x][graph[vertex].y];
            secondValue = secondVertexValue;
        }
        else
        {
            secondValue = image[graph[vertex].x][graph[vertex].y];
            firstValue = secondVertexValue;
        }
        linearInterpolation = [yDiff, firstValue, secondValue](double x, double y){return secondValue*y/yDiff + firstValue*(yDiff-y)/yDiff;};
        Array2D imageCopy = image.GetCopy(graph[vertex].x, graph[vertex].x, miny, maxy);
        imageCopy.xOffset=0;
        imageCopy.yOffset=0;
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
            firstValue = image[graph[vertex].x][graph[vertex].y];
            secondValue = secondVertexValue;
        }
        else
        {
            secondValue = image[graph[vertex].x][graph[vertex].y];
            firstValue = secondVertexValue;
        }
        linearInterpolation = [xDiff, firstValue, secondValue](double x, double y){return secondValue*x/xDiff + firstValue*(xDiff-x)/xDiff;};
        Array2D imageCopy = image.GetCopy(minx, maxx, graph[vertex].y, graph[vertex].y);
        imageCopy.xOffset=0;
        imageCopy.yOffset=0;
        imageCopy.Subtract(linearInterpolation, 0, xDiff, 0, 0);
        auto coefficientsForFullEdge = GetInterpolationsOfEdgeOfDifferentOrders(imageCopy, 0, xDiff, 0, 0, orders);
        for(int i=0;i<orders;i++)
            result.push_back(coefficientsForFullEdge[i]);
    }
    return result;
}
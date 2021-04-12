#include "P5.hpp"
#include "PbiHelper.hpp"
#include "InterpolationHelper.hpp"

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
    return where(edges, [minx, maxx, miny, maxy, &graph](vertex_descriptor v){return graph[v].x==minx or graph[v].x==maxx or graph[v].y==miny or graph[v].y==maxy;});
}



std::tuple<std::map<int, std::map<int, double>>,std::map<std::string, std::vector<double>>> GetCornerValues(Array2D& image, IGraph& graph, std::set<vertex_descriptor>& edges, int minx, int maxx, int miny, int maxy, int orders)
{
    std::map<int, std::map<int, double>> resultCorners;    
    std::map<std::string, std::vector<double>> resultCoefficients;    
    std::vector<vertex_descriptor> sortedEdges;
    sortedEdges.insert(sortedEdges.begin(), edges.begin(),edges.end());
    std::sort(sortedEdges.begin(), sortedEdges.end(), [&graph](const auto &a, const auto & b){return graph.GetAdjacentVertices(a).size()>graph.GetAdjacentVertices(b).size();});
    std::unique_ptr<double[]> coefficientsForFullEdge;
    for(auto edge: sortedEdges)
    {
        auto vertices = graph.GetAdjacentVertices(edge);
        if(vertices.size()==2)
        {
            std::vector<double> resultCoefficientsVector;
            resultCoefficientsVector.reserve(orders);
            if(graph[vertices[0]].x == graph[vertices[1]].x)
            {
                auto imageCopy = image.GetCopy(graph[vertices[1]].x, graph[vertices[1]].x, std::min(graph[vertices[0]].y, graph[vertices[1]].y), std::max(graph[vertices[0]].y, graph[vertices[1]].y));
                double yDiff = maxy-miny;
                double val1 = imageCopy[0][0];
                double val2 = imageCopy[0][yDiff];
                imageCopy.Subtract([val1, val2, yDiff](double x, double y){return val2*y/yDiff + val1*(yDiff-y)/yDiff;});
                coefficientsForFullEdge = GetInterpolationsOfEdgeOfDifferentOrders(imageCopy, 0, 0, 0, yDiff, orders);
                if((graph[vertices[0]].y==miny and graph[vertices[1]].y==maxy) or (graph[vertices[1]].y==miny and graph[vertices[0]].y==maxy))
                {
                    resultCorners[graph[vertices[0]].x][graph[vertices[0]].y] = image[graph[vertices[0]].x][graph[vertices[0]].y];
                    resultCorners[graph[vertices[1]].x][graph[vertices[1]].y] = image[graph[vertices[1]].x][graph[vertices[1]].y];
                }
                else if((graph[vertices[1]].y!=miny and graph[vertices[1]].y!=maxy) or (graph[vertices[0]].y!=miny and graph[vertices[0]].y!=maxy))
                {
                    double midValue = (val1+val2)/2;
                    for(int order=0;order<orders;order++)
                    {
                        midValue += coefficientsForFullEdge[order]*testFunctions[order](0.5,0);
                    }
                    resultCorners[graph[vertices[0]].x][yDiff/2] = midValue;
                    imageCopy = Array2D(1,101);
                    for(int order=0;order<orders;order++)
                    {
                        auto temporaryArray = GetFunctionSplitToNElements(testFunctions[order],0,1,101);
                        temporaryArray.Transpose();
                        imageCopy.Subtract(temporaryArray);
                    }   
                    imageCopy.Multiply(-1);
                    imageCopy.Subtract([val1, val2, yDiff](double x, double y){return -val2*y/100 + val1*(100-y)/100;});
                    if(graph[vertices[0]].y == miny or graph[vertices[1]].y==miny)
                    {
                        imageCopy.Subtract([val1, midValue](double x, double y){return -midValue*y/50.0 + val1*(50-y)/50.0;},0,0,0,50);
                        coefficientsForFullEdge = GetInterpolationsOfEdgeOfDifferentOrders(imageCopy, 0, 0, 0, 50, orders);
                    }
                    else if(graph[vertices[0]].y == maxy or graph[vertices[1]].y==maxy)
                    {
                        imageCopy.Subtract([val2, midValue](double x, double y){return -val2*y/50.0 + midValue*(50-y)/50.0;},0,0,50,100,0,50);
                        coefficientsForFullEdge = GetInterpolationsOfEdgeOfDifferentOrders(imageCopy, 0, 0, 50, 100, orders);
                    }
                }
                for(int i=0;i<orders;i++)
                    resultCoefficientsVector.push_back(coefficientsForFullEdge[i]);
                if(graph[vertices[0]].x==minx)
                    resultCoefficients["left"] = resultCoefficientsVector;
                else
                    resultCoefficients["right"] = resultCoefficientsVector;
            }
            else if(graph[vertices[0]].y == graph[vertices[1]].y)
            {
                auto imageCopy = image.GetCopy(std::min(graph[vertices[0]].x, graph[vertices[1]].x), std::max(graph[vertices[0]].x, graph[vertices[1]].x), graph[vertices[1]].y, graph[vertices[1]].y);
                double xDiff = maxx-minx;
                double val1 = imageCopy[0][0];
                double val2 = imageCopy[xDiff][0];
                imageCopy.Subtract([val1, val2, xDiff](double x, double y){return val2*x/xDiff + val1*(xDiff-x)/xDiff;});
                coefficientsForFullEdge = GetInterpolationsOfEdgeOfDifferentOrders(imageCopy, 0, xDiff, 0, 0, orders);
                if((graph[vertices[0]].x==minx and graph[vertices[1]].x==maxx) or (graph[vertices[1]].x==minx and graph[vertices[0]].x==maxx))
                {
                    resultCorners[graph[vertices[0]].x][graph[vertices[0]].y] = image[graph[vertices[0]].x][graph[vertices[0]].y];
                    resultCorners[graph[vertices[1]].x][graph[vertices[1]].y] = image[graph[vertices[1]].x][graph[vertices[1]].y];
                }
                else if((graph[vertices[1]].x!=minx and graph[vertices[1]].x!=maxx) or (graph[vertices[0]].x!=minx and graph[vertices[0]].x!=maxx))
                {
                    double midValue = (val1+val2)/2;
                    for(int order=0;order<orders;order++)
                    {
                        midValue += coefficientsForFullEdge[order]*testFunctions[order](0.5,0);
                    }
                    resultCorners[xDiff/2][graph[vertices[0]].y] = midValue;
                    imageCopy = Array2D(101,1);
                    for(int order=0;order<orders;order++)
                    {
                        imageCopy.Subtract(GetFunctionSplitToNElements(testFunctions[order],0,1,101));
                    }   
                    imageCopy.Multiply(-1);
                    imageCopy.Subtract([val1, val2](double x, double y){return -val2*x/100 + val1*(100-x)/100;});
                    if(graph[vertices[0]].x == minx or graph[vertices[1]].x==minx)
                    {
                        imageCopy.Subtract([val1, midValue](double x, double y){return -midValue*x/50.0 + val1*(50-x)/50.0;},0,50,0,0);
                        coefficientsForFullEdge = GetInterpolationsOfEdgeOfDifferentOrders(imageCopy, 0, 50, 0, 0, orders);
                    }
                    else if(graph[vertices[0]].x == maxx or graph[vertices[1]].x==maxx)
                    {
                        imageCopy.Subtract([val2, midValue](double x, double y){return -val2*x/50.0 + midValue*(50-x)/50.0;},50,100,0,0,50,0);
                        coefficientsForFullEdge = GetInterpolationsOfEdgeOfDifferentOrders(imageCopy, 50, 100, 0, 0, orders);
                    }
                }
                for(int i=0;i<orders;i++)
                    resultCoefficientsVector.push_back(coefficientsForFullEdge[i]);
                if(graph[vertices[0]].y==miny)
                    resultCoefficients["top"] = resultCoefficientsVector;
                else
                    resultCoefficients["bottom"] = resultCoefficientsVector;
            }
        }
        else if(vertices.size()==1)
        {
            resultCorners[graph[vertices[0]].x][graph[vertices[0]].y] = image[graph[vertices[0]].x][graph[vertices[0]].y];
            double valueForSecondVertex;
            if(graph[vertices[0]].x==graph[edge].x)
            {
                if(graph[vertices[0]].y==miny)
                    valueForSecondVertex = resultCorners[graph[vertices[0]].x][maxy];
                else
                    valueForSecondVertex = resultCorners[graph[vertices[0]].x][miny];

                if(graph[vertices[0]].x==minx)
                    resultCoefficients["left"] = GetSingleVertexFEdgeCoefficients(image, graph, edge, vertices[0], minx, miny, maxx, maxy, valueForSecondVertex, orders);
                else 
                    resultCoefficients["right"] = GetSingleVertexFEdgeCoefficients(image, graph, edge, vertices[0], minx, miny, maxx, maxy, valueForSecondVertex, orders);
            }
            else
            {
                if(graph[vertices[0]].x==minx)
                    valueForSecondVertex = resultCorners[maxx][graph[vertices[0]].y];
                else
                    valueForSecondVertex = resultCorners[minx][graph[vertices[0]].y];
                if(graph[vertices[0]].y==miny)
                    resultCoefficients["top"] = GetSingleVertexFEdgeCoefficients(image, graph, edge, vertices[0], minx, miny, maxx, maxy, valueForSecondVertex, orders);
                else 
                    resultCoefficients["bottom"] = GetSingleVertexFEdgeCoefficients(image, graph, edge, vertices[0], minx, miny, maxx, maxy, valueForSecondVertex, orders);
            }
            
        }
        else
            assert(false and "GetCornerValues Wrong number of vertices adjacent to the edge");
    }
    return std::make_tuple(resultCorners, resultCoefficients);
}

void InterpolateRectangle(IGraph& graph, vertex_descriptor iEdge,  Array2D& image, Array2D& interpolation, int minx, int maxx, int miny, int maxy, int orders)
{
            auto properEdges = GetEdges(graph, iEdge, minx, maxx, miny, maxy);
            std::map<int, std::map<int, double>> cornerValues;
            std::map<std::string, std::vector<double>> edgeCoefficients;
            std::tie(cornerValues, edgeCoefficients) = GetCornerValues(image, graph, properEdges, minx, maxx, miny, maxy, orders);
            interpolation.TrivialBilinearInterpolation(minx, maxx, miny, maxy,cornerValues[minx][miny], cornerValues[minx][maxy], cornerValues[maxx][miny], cornerValues[maxx][maxy]);
            //getCoefficientsForEdges(properEdges);
            
            //interpolation.TrivialBilinearInterpolation(image, minx, maxx, miny, maxy);
            auto imageCopy = image.GetCopy(minx, maxx, miny, maxy);
            auto interpolationCopy = interpolation.GetCopy(minx, maxx, miny, maxy);

            imageCopy.Subtract(interpolationCopy);
            int xDiff = maxx-minx;
            int yDiff = maxy-miny;
            interpolationCopy.FillWith(0);
            auto minyFuncoefficients = std::move(edgeCoefficients["top"]);//GetInterpolationsOfEdgeOfDifferentOrders(imageCopy, 0, xDiff, 0, 0, orders);
            auto maxyFuncoefficients = std::move(edgeCoefficients["bottom"]);//GetInterpolationsOfEdgeOfDifferentOrders(imageCopy, 0, xDiff, yDiff, yDiff, orders);
            auto minxFuncoefficients = std::move(edgeCoefficients["left"]);//GetInterpolationsOfEdgeOfDifferentOrders(imageCopy, 0, 0, 0, yDiff, orders);
            auto maxxFuncoefficients = std::move(edgeCoefficients["right"]);//GetInterpolationsOfEdgeOfDifferentOrders(imageCopy, xDiff, xDiff, 0, yDiff, orders);
            for(int i=0;i<orders;i++)
            {
                double minyFuncoefficient = minyFuncoefficients[i];
                double maxyFuncoefficient = maxyFuncoefficients[i];
                double minxFuncoefficient = minxFuncoefficients[i];
                double maxxFuncoefficient = maxxFuncoefficients[i];
                auto splitXFunction = GetFunctionSplitToNElements(testFunctions[i],0,1,xDiff+1);
                auto splitYFunction = GetFunctionSplitToNElements(testFunctions[i],0,1,yDiff+1);
                interpolationCopy.Subtract([xDiff, maxy, yDiff, minyFuncoefficient, i, &splitXFunction](double x, double y){return -splitXFunction[x][0]*minyFuncoefficient*(maxy-y)/(yDiff);},0, xDiff, 0, yDiff, minx, 0);
                interpolationCopy.Subtract([xDiff, yDiff, miny, maxyFuncoefficient, i, &splitXFunction](double x, double y){return -splitXFunction[x][0]*maxyFuncoefficient*(y-miny)/(yDiff);},0, xDiff, 0, yDiff, minx, 0);
                interpolationCopy.Subtract([xDiff, maxx, yDiff, minxFuncoefficient, i, &splitYFunction](double x, double y){return -splitYFunction[y][0]*minxFuncoefficient*(maxx-x)/(xDiff);},0, xDiff, 0, yDiff, 0, miny);
                interpolationCopy.Subtract([minx, xDiff, yDiff, maxxFuncoefficient, i, &splitYFunction](double x, double y){return -splitYFunction[y][0]*maxxFuncoefficient*(x-minx)/(xDiff);},0, xDiff, 0, yDiff, 0, miny);
            }
            imageCopy.Subtract(interpolationCopy);
            interpolationCopy.Multiply(-1);
            interpolation.Subtract(interpolationCopy, minx, maxx, miny, maxy);
            auto coefficients2D = GetInterpolationsOfRectangleOfDifferentOrders(imageCopy, 0, xDiff, 0, yDiff, orders);
            for(int i=0;i<orders;i++)
                for(int j=0;j<orders;j++)
                {
                    auto splitXFunction = GetFunctionSplitToNElements(testFunctions[i],0,1,xDiff+1);
                    auto splitYFunction = GetFunctionSplitToNElements(testFunctions[j],0,1,yDiff+1);
                    double coefficient = coefficients2D[i*orders+j];
                    interpolation.Subtract([coefficient, i, j, &splitXFunction, &splitYFunction](double x, double y){return -coefficient*splitXFunction[x][0]*splitYFunction[y][0];},minx, maxx, miny, maxy, minx, miny);
                }
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
            InterpolateRectangle(*graph, IEdge, *image, *interpolation, minx, maxx, miny, maxy, orders);
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
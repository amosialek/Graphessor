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

template<typename Key1, typename Key2, typename Value>
void JoinNestedMaps(std::map<Key1,std::map<Key2, Value>>& target, std::map<Key1,std::map<Key2, Value>>& from)
{
    for(auto outerKeyValue: from)
    {
        target[outerKeyValue.first].insert(outerKeyValue.second.begin(), outerKeyValue.second.end());
    }
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
            std::map<int, std::map<int, double>> partialResultCorners;
            std::map<std::string, std::vector<double>>partialResultCoefficients;
            std::tie(partialResultCorners, partialResultCoefficients)  = GetFEdgeWithTwoVerticesCoefficients(image, graph[vertices[0]], graph[vertices[1]], minx, maxx, miny, maxy, orders);
            JoinNestedMaps(resultCorners, partialResultCorners);
            resultCoefficients.insert(partialResultCoefficients.begin(), partialResultCoefficients.end());
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
    assert(resultCorners[minx].size() + resultCorners[maxx].size() == 4 and "GetCornerValues Wrong number of calculated corners");
    return std::make_tuple(resultCorners, resultCoefficients);
}

void InterpolateRectangle(IGraph& graph, vertex_descriptor iEdge,  Array2D& image, Array2D& interpolation, int minx, int maxx, int miny, int maxy, int orders)
{
    auto properEdges = GetEdges(graph, iEdge, minx, maxx, miny, maxy);
    assert(properEdges.size()==4 && "GetEdges() didn't return 4 values");
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
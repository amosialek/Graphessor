#include<vector>
#include "Pixel.hpp"
#include "CachedGraph.hpp"
#include "Image.hpp"
#include "GraphessorConstants.hpp"
#include "Productions.hpp"
#include "PbiHelper.hpp"

double GetInterpolatedPixel(Array2D& image, int x1, int x2, int y1, int y2, int x, int y)
{
    int r11,r21,r12,r22;
    r11 = image[x1][y1];
    r21 = image[x2][y1];
    r12 = image[x1][y2];
    r22 = image[x2][y2];
    double coef22 = (x-x1)*(y-y1)/(1.0*(x2-x1)*(y2-y1));
    double coef12 = (x2-x)*(y-y1)/(1.0*(x2-x1)*(y2-y1));
    double coef21 = (x-x1)*(y2-y)/(1.0*(x2-x1)*(y2-y1));
    double coef11 = (x2-x)*(y2-y)/(1.0*(x2-x1)*(y2-y1));
    double rInterpolated = r11 * coef11 + r21 * coef21 + r12 * coef12 + r22 * coef22;
    return rInterpolated;
}


std::vector<std::vector<Pixel>> GetPixelsForBilinearInterpolation(CachedGraph& graph)
{
    std::vector<std::vector<Pixel>> result;
    for(auto v : graph.GetCacheIterator(NODELABEL_I))
    {
        auto adjacentPixels = graph.GetAdjacentVertices(v);
        if(adjacentPixels.size()==4)
        {
            std::vector<Pixel> s;
            for(auto adjacentPixel : adjacentPixels)
                s.emplace_back(graph[adjacentPixel]);
            result.emplace_back(s);
        }
    }
    return result;
}

std::vector<std::vector<Pixel>> GetPixelsForBaricentricInterpolation(CachedGraph& graph)
{
    std::vector<std::vector<Pixel>> result;
    auto FEdges = graph.GetCacheIterator(NODELABEL_F);
    auto IEdges = graph.GetCacheIterator(NODELABEL_I);
    std::set<vertex_descriptor> danglingFedges;
    for(auto fEdge : FEdges)
    {
        if(graph.GetAdjacentVertices(fEdge).size()==1)
            danglingFedges.insert(fEdge);
    }
    for(auto f : danglingFedges)
    {
        auto adjacentPixel = graph.GetAdjacentVertices(f)[0];
        auto adjacentIEdges = graph.GetAdjacentVertices(adjacentPixel,NODELABEL_I);

        std::sort(adjacentIEdges.begin(), adjacentIEdges.end(), [f, &graph](const vertex_descriptor& v1, const vertex_descriptor& v2)->bool
            {
                auto fx = graph[f].x;
                auto fy = graph[f].y;
                auto v1x = graph[v1].x;
                auto v1y = graph[v1].y;
                auto v2x = graph[v2].x;
                auto v2y = graph[v2].y;
                return (v1x-fx)*(v1x-fx)+(v1y-fy)*(v1y-fy) < (v2x-fx)*(v2x-fx)+(v2y-fy)*(v2y-fy);
            });
        
        std::set<vertex_descriptor> closerPixels;
        std::set<vertex_descriptor> outerPixels;
        auto a0 = graph.GetAdjacentVertices(adjacentIEdges[0], NODELABEL_P);
        auto a1 = graph.GetAdjacentVertices(adjacentIEdges[1], NODELABEL_P);
        std::copy(a0.begin(),a0.end(),std::inserter(closerPixels, closerPixels.end()));
        std::copy(a1.begin(),a1.end(),std::inserter(closerPixels, closerPixels.end()));
        a0 = graph.GetAdjacentVertices(adjacentIEdges[2], NODELABEL_P);
        a1 = graph.GetAdjacentVertices(adjacentIEdges[3], NODELABEL_P);
        std::copy(a0.begin(),a0.end(),std::inserter(outerPixels, outerPixels.end()));
        std::copy(a1.begin(),a1.end(),std::inserter(outerPixels, outerPixels.end()));

        std::vector<vertex_descriptor> otherTriangleVertices;

        std::set_difference(closerPixels.begin(), closerPixels.end(), outerPixels.begin(), outerPixels.end(), std::inserter(otherTriangleVertices, otherTriangleVertices.begin()));
        std::vector<Pixel> r
        {
            graph[adjacentPixel],
            graph[otherTriangleVertices[0]],
            graph[otherTriangleVertices[1]]
        }; 
        result.emplace_back(r);
    }

    for(auto IEdge : IEdges)
    {
        auto adjacentPixels = graph.GetAdjacentVertices(IEdge);
        if(adjacentPixels.size()==3)
        {
            std::vector<Pixel> r
            {
                graph[adjacentPixels[0]],
                graph[adjacentPixels[1]],
                graph[adjacentPixels[2]]
            };
            result.emplace_back(r);
        }
    }

    return result;
}

void BilinearInterpolation(std::vector<Pixel> pixels, std::shared_ptr<Array2D> image, std::shared_ptr<Array2D> interpolation)
{
    int minx,miny,maxx,maxy;
    minx = maxx = pixels.begin() -> x;
    miny = maxy = pixels.begin() -> y;
    for(auto adjacentVertex : pixels)
    {
        minx = std::min(minx, adjacentVertex.x);
        maxx = std::max(maxx, adjacentVertex.x);
        miny = std::min(miny, adjacentVertex.y);
        maxy = std::max(maxy, adjacentVertex.y);
    }
    for(int y=miny;y<=maxy;y++)
        for(int x=minx;x<=maxx;x++)
            (*interpolation)[x][y] = GetInterpolatedPixel(*image, minx, maxx, miny, maxy, x, y);
}

void BaricentricInterpolation(std::vector<Pixel> pixels, std::shared_ptr<Array2D> image, std::shared_ptr<Array2D> interpolation)
{
    int y2y3 = pixels[1].y-pixels[2].y;
    int x3x2 = pixels[2].x-pixels[1].x;
    int y3y1 = pixels[2].y-pixels[0].y;
    int y1y3 = pixels[0].y-pixels[2].y;
    int x1x3 = pixels[0].x-pixels[2].x;
    int minx = std::min(pixels[0].x,(std::min(pixels[1].x,pixels[2].x)));
    int miny = std::min(pixels[0].y,(std::min(pixels[1].y,pixels[2].y)));
    int maxx = std::max(pixels[0].x,(std::max(pixels[1].x,pixels[2].x)));
    int maxy = std::max(pixels[0].y,(std::max(pixels[1].y,pixels[2].y)));
    double w1Coefficient = 1.0/(y2y3 * x1x3 + x3x2 * y1y3);
    double w2Coefficient = w1Coefficient;
    double w1,w2,w3;
    Pixel p;
    for(int y = miny; y <= maxy; y++)
        for(int x=minx;x<=maxx;x++)
        {
            double value = (*interpolation)[x][y];
            if(value==0)
            {
                w1 = w1Coefficient*(y2y3*(x-pixels[2].x) + x3x2 * (y-pixels[2].y));
                w2 = w2Coefficient*(y3y1*(x-pixels[2].x) + x1x3*(y-pixels[2].y));
                w3 = 1 - w1 - w2;
                if(w1>=-0.01 and w2>=-0.01 and w3>=-0.01)
                    (*interpolation)[x][y] = w1 * pixels[0].r
                        + w2 * pixels[1].r
                        + w3 * pixels[2].r;
            }
        }
}

void DoLinearInterpolation1(std::shared_ptr<Array2D> image, std::shared_ptr<Array2D> interpolation, std::shared_ptr<CachedGraph> graph)
{
    auto IEdges = graph->GetCacheIterator(NODELABEL_I);
    for(auto IEdge : IEdges)
    {
        auto pixels = graph->GetAdjacentVertices(IEdge,NODELABEL_P);
        int minX = (*graph)[pixels[0]].x;
        int maxX = (*graph)[pixels[0]].x;
        int minY = (*graph)[pixels[0]].y;
        int maxY = (*graph)[pixels[0]].y;
        for(auto pixel : pixels)
        {
            minX = std::min((*graph)[pixel].x, minX);
            maxX = std::max((*graph)[pixel].x, maxX);
            minY = std::min((*graph)[pixel].y, minY);
            maxY = std::max((*graph)[pixel].y, maxY);
        }
        interpolation -> BilinearInterpolation(*image, minX, maxX, minY, maxY);        
    }
}

void DoLinearInterpolation2(std::shared_ptr<Array2D> image, std::shared_ptr<Array2D> interpolation, std::shared_ptr<CachedGraph> graph)
{
    std::set<vertex_descriptor> pixels;
    pixels = graph -> GetPixels();
    for(auto pixel : pixels)
    {
        (*interpolation)[(*graph)[pixel].x][(*graph)[pixel].y] = (*graph)[pixel].r;
         //img._view[graph->operator[](pixel).y * (width+1) + graph -> operator[](pixel).x][channel] = this -> GetRGBChannelValue(graph->operator[](pixel), channel);
        spdlog::debug("Setting x={} y={} in red channel to {}", (*graph)[pixel].x,(*graph)[pixel].y, (*graph)[pixel].r);    
    }
    auto PixelsForBilinearInterpolation = graph -> GetPixelsForBilinearInterpolation();
    auto PixelsForBaricentricInterpolation = graph -> GetPixelsForBaricentricInterpolation();
    for(auto v : PixelsForBilinearInterpolation)
    {
        BilinearInterpolation(v, image, interpolation);
    }
    for(auto v : PixelsForBaricentricInterpolation)
    {
        BaricentricInterpolation(v, image, interpolation);
    }

}

void InterpolateEdge(
    std::shared_ptr<Array2D> image, 
    Array2D& imageCopy, 
    std::shared_ptr<Array2D> interpolation, 
    std::shared_ptr<CachedGraph> graph,
    vertex_descriptor edge,
    int orders)
{
    bool isVertivcal=false;
    auto vertices = graph->GetAdjacentVertices(edge);
    if((*graph)[vertices[0]].x == (*graph)[vertices[1]].x)
        isVertivcal=true;
    double coefficient;
    int x1 = (*graph)[vertices[0]].x;
    int x2 = (*graph)[vertices[1]].x;
    int y1 = (*graph)[vertices[0]].y;
    int y2 = (*graph)[vertices[1]].y;
    int minX = x1;
    int maxX = x1;
    int minY = y1;
    int maxY = y1;
    auto neighbourIEdges = graph -> GetAdjacentVertices(vertices[0], NODELABEL_I);
    auto neighbourIEdges2 = graph -> GetAdjacentVertices(vertices[1], NODELABEL_I);
    std::set<vertex_descriptor> distinctIEdges(neighbourIEdges.begin(), neighbourIEdges.end());
    distinctIEdges.insert(neighbourIEdges2.begin(), neighbourIEdges2.end());
    int yOffset = std::min(y1,y2);
    int xOffset = std::min(x1,x2);
    std::function<double(double, double)> interpolationFunctionOnLine = [](double x, double y){return 0;};
    std::unique_ptr<double[]>coefficients;
    if(isVertivcal)
    {
        int yDiff = std::abs(y1-y2);
        coefficient = GetSquareInterpolationOfYEdge(imageCopy, (*graph)[vertices[0]].x,std::min(y1,y2), std::max(y1,y2));
        coefficients = GetInterpolationsOfEdgeOfDifferentOrders(*interpolation, (*graph)[vertices[0]].x, (*graph)[vertices[0]].x, std::min(y1,y2), std::max(y1,y2), orders);
        //interpolationFunctionOnLine = [yDiff,coefficient](double x, double y){return -coefficient*(y)*(y-yDiff);};
        distinctIEdges = where(distinctIEdges,[graph, y1, y2](vertex_descriptor v){return (*graph)[v].y>=std::min(y1,y2) && (*graph)[v].y<=std::max(y1,y2);});
    }
    else
    {
        int xDiff = std::abs(x1-x2);
        coefficient = GetSquareInterpolationOfEdge(imageCopy, std::min(x1,x2), std::max(x1,x2), (*graph)[vertices[1]].y);
        coefficients = GetInterpolationsOfEdgeOfDifferentOrders(*interpolation, std::min(x1,x2), std::max(x1,x2), (*graph)[vertices[1]].y, (*graph)[vertices[1]].y, orders);
        //interpolationFunctionOnLine = [xDiff, coefficient](double x, double y){return -coefficient*(x)*(x-xDiff);};
        distinctIEdges = where(distinctIEdges,[graph, x1, x2](vertex_descriptor v){return (*graph)[v].x>=std::min(x1,x2) && (*graph)[v].x<=std::max(x1,x2);});
    }
    for(int order=0; order<orders; order++)
    Add(interpolationFunctionOnLine, Multiply(testFunctions[order], coefficients[order]));
    for(auto IEdge : distinctIEdges)
    {
        auto pixels = graph->GetAdjacentVertices(IEdge, NODELABEL_P);
        for(auto pixel : pixels)
        {
            minX = std::min((*graph)[pixel].x, minX);
            maxX = std::max((*graph)[pixel].x, maxX);
            minY = std::min((*graph)[pixel].y, minY);
            maxY = std::max((*graph)[pixel].y, maxY);
        }
    }
    if(isVertivcal)
        {
        std::function<double(double, double)> interpolationFunctionLowerX =
            x2==minX
            ? [](double x, double y){return 0.0;}
            : Multiply(interpolationFunctionOnLine,[minX, x2](double x, double y){return (x-minX)/(x2-minX);});
        std::function<double(double, double)> interpolationFunctionHigherX =
            x2==maxX
            ? [](double x, double y){return 0.0;}
            : Multiply(interpolationFunctionOnLine,[x2, maxX](double x, double y){return (maxX-x)/(maxX-x2);});
        interpolation->Subtract(interpolationFunctionLowerX, minX, x2, minY, maxY, 0, yOffset);
        interpolation->Subtract(interpolationFunctionHigherX, x2+1, maxX, minY, maxY, 0 , yOffset);
    }
    else
    {
        std::function<double(double, double)> interpolationFunctionLowerY = 
            y2==minY
            ? [](double x, double y){return 0.0;}
            : Multiply(interpolationFunctionOnLine,[minY, y2](double x, double y){return (y-minY)/(y2-minY);});
        std::function<double(double, double)> interpolationFunctionHigherY = 
            y2==maxY
            ? [](double x, double y){return 0.0;}
            : Multiply(interpolationFunctionOnLine,[y2, maxY](double x, double y){return (maxY-y)/(maxY-y2);});
        interpolation->Subtract(interpolationFunctionLowerY, minX, maxX, minY, y2, xOffset, 0);
        interpolation->Subtract(interpolationFunctionHigherY, minX, maxX, y2+1, maxY, xOffset, 0);
    }
}

void InteriorInterpolation(std::shared_ptr<Array2D> image, std::shared_ptr<Array2D> interpolation, std::shared_ptr<CachedGraph> graph)
{
    auto imageCopy = image->GetCopy(0, image->width-1, 0, image->height-1);
    imageCopy.Subtract(*interpolation);

    auto IEdges = graph->GetCacheIterator(NODELABEL_I);
    for(auto IEdge : IEdges)
    {
        auto pixels = graph->GetAdjacentVertices(IEdge, NODELABEL_P);
        int minX = (*graph)[pixels[0]].x;
        int maxX = (*graph)[pixels[0]].x;
        int minY = (*graph)[pixels[0]].y;
        int maxY = (*graph)[pixels[0]].y;
        for(auto pixel : pixels)
            {
                minX = std::min((*graph)[pixel].x, minX);
                maxX = std::max((*graph)[pixel].x, maxX);
                minY = std::min((*graph)[pixel].y, minY);
                maxY = std::max((*graph)[pixel].y, maxY);
            }
        double coefficient = GetSquareInterpolationOfRectangle(imageCopy, minX, maxX, minY, maxY);
        int xDiff = maxX-minX;
        int yDiff = maxY-minY;
        auto interpolationFunction = [xDiff, yDiff, coefficient](double x, double y){return coefficient * (x) * (x - xDiff) * (y) * (y - yDiff);};
        interpolation->Subtract(interpolationFunction, minX, maxX, minY, maxY, minX, minY);
    }
}

void EdgeInterpolation(std::shared_ptr<Array2D> image, std::shared_ptr<Array2D> interpolation, std::shared_ptr<CachedGraph> graph)
{
    auto imageCopy = image->GetCopy(0, image->width-1, 0, image->height-1);
    imageCopy.Subtract(*interpolation);

    auto fedges = graph->GetCacheIterator(NODELABEL_F);
    auto bedges = graph->GetCacheIterator(NODELABEL_B);
    auto filteredFedges = where(fedges,[graph](vertex_descriptor v){return graph->GetAdjacentVertices(v).size()==2;});
    filteredFedges.insert(bedges.begin(), bedges.end());//merge bedges and fedges
    for(auto edge : filteredFedges)
    {
        InterpolateEdge(image, imageCopy, interpolation, graph, edge, 1);
    }
}

void RectangularInterpolation2(std::shared_ptr<Array2D> image, std::shared_ptr<Array2D> interpolation, std::shared_ptr<CachedGraph> graph)
{
    DoLinearInterpolation2(image, interpolation, graph);
    EdgeInterpolation(image, interpolation, graph);
    InteriorInterpolation(image, interpolation, graph);
}
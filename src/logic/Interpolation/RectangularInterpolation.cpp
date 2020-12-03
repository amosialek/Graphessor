#include<vector>
#include "Pixel.hpp"
#include "CachedGraph.hpp"
#include "Image.hpp"
#include "GraphessorConstants.hpp"

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


std::vector<std::vector<Pixel>> CachedGraph::GetPixelsForBilinearInterpolation()
{
    std::vector<std::vector<Pixel>> result;
    for(auto v : GetCacheIterator(NODELABEL_I))
    {
        auto adjacentPixels = GetAdjacentVertices(v);
        if(adjacentPixels.size()==4)
        {
            std::vector<Pixel> s;
            for(auto adjacentPixel : adjacentPixels)
                s.emplace_back(this->operator[](adjacentPixel));
            result.emplace_back(s);
        }
    }
    return result;
}

std::vector<std::vector<Pixel>> CachedGraph::GetPixelsForBaricentricInterpolation()
{
    std::vector<std::vector<Pixel>> result;
    auto FEdges = GetCacheIterator(NODELABEL_F);
    auto IEdges = GetCacheIterator(NODELABEL_I);
    std::set<vertex_descriptor> danglingFedges;
    for(auto fEdge : FEdges)
    {
        if(GetAdjacentVertices(fEdge).size()==1)
            danglingFedges.insert(fEdge);
    }
    for(auto f : danglingFedges)
    {
        auto adjacentPixel = GetAdjacentVertices(f)[0];
        auto adjacentIEdges = GetAdjacentVertices(adjacentPixel,NODELABEL_I);

        std::sort(adjacentIEdges.begin(), adjacentIEdges.end(), [f, this](const vertex_descriptor& v1, const vertex_descriptor& v2)->bool
            {
                auto fx = (*this)[f].x;
                auto fy = (*this)[f].y;
                auto v1x = (*this)[v1].x;
                auto v1y = (*this)[v1].y;
                auto v2x = (*this)[v2].x;
                auto v2y = (*this)[v2].y;
                return (v1x-fx)*(v1x-fx)+(v1y-fy)*(v1y-fy) < (v2x-fx)*(v2x-fx)+(v2y-fy)*(v2y-fy);
            });
        
        std::set<vertex_descriptor> closerPixels;
        std::set<vertex_descriptor> outerPixels;
        auto a0 = GetAdjacentVertices(adjacentIEdges[0], NODELABEL_P);
        auto a1 = GetAdjacentVertices(adjacentIEdges[1], NODELABEL_P);
        std::copy(a0.begin(),a0.end(),std::inserter(closerPixels, closerPixels.end()));
        std::copy(a1.begin(),a1.end(),std::inserter(closerPixels, closerPixels.end()));
        a0 = GetAdjacentVertices(adjacentIEdges[2], NODELABEL_P);
        a1 = GetAdjacentVertices(adjacentIEdges[3], NODELABEL_P);
        std::copy(a0.begin(),a0.end(),std::inserter(outerPixels, outerPixels.end()));
        std::copy(a1.begin(),a1.end(),std::inserter(outerPixels, outerPixels.end()));

        std::vector<vertex_descriptor> otherTriangleVertices;

        std::set_difference(closerPixels.begin(), closerPixels.end(), outerPixels.begin(), outerPixels.end(), std::inserter(otherTriangleVertices, otherTriangleVertices.begin()));
        std::vector<Pixel> r
        {
            graph -> operator[](adjacentPixel),
            graph -> operator[](otherTriangleVertices[0]),
            graph -> operator[](otherTriangleVertices[1])
        }; 
        result.emplace_back(r);
    }

    for(auto IEdge : IEdges)
    {
        auto adjacentPixels = GetAdjacentVertices(IEdge);
        if(adjacentPixels.size()==3)
        {
            std::vector<Pixel> r
            {
                graph -> operator[](adjacentPixels[0]),
                graph -> operator[](adjacentPixels[1]),
                graph -> operator[](adjacentPixels[2])
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
            double value = (*image)[x][y];
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
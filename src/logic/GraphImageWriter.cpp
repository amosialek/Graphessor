#include "GraphImageWriter.hpp"
#include <boost/gil/extension/io/bmp.hpp>
#include "GraphessorConstants.hpp"
#include "Image.hpp"

void GraphImageWriter::DrawPixels(std::shared_ptr<CachedGraph> graph, std::string filename)
{
    int maxx=0,maxy=0;
    auto pixels = graph->GetCacheIterator(NODELABEL_P) ;
    for(auto a : pixels)
    {
        maxx = std::max(maxx,graph->operator[](a).x);
        maxy = std::max(maxy,graph->operator[](a).y);
    }
    Image* img = new Image(++maxx, ++maxy);
    for (auto p : pixels)
    {
        auto pixel = graph->operator[](p);
        img->SetSquare(pixel.x,pixel.y,0,0,3);
        img->SetSquare(pixel.x,pixel.y,1,0,3);
        img->SetSquare(pixel.x,pixel.y,2,0,3);
        auto adjacentEdges = graph -> GetAdjacentVertices(p);
        std::vector<vertex_descriptor> adjacentPixels;
        for(auto edge : adjacentEdges)
        {
            auto tmp= graph -> GetAdjacentVertices(edge, NODELABEL_P);
            std::copy(tmp.begin(), tmp.end(), std::back_inserter(adjacentPixels));
        }
        for(auto adjacentPixel : adjacentPixels)
        {
            img -> DrawBlackLine(pixel.x,pixel.y, graph->operator[](adjacentPixel).x,graph->operator[](adjacentPixel).y);
        }
    }
    img -> save(filename);
    delete img;
    img=0;
}
void GraphImageWriter::DrawFullGraph(std::shared_ptr<CachedGraph> graph, std::string filename)
{
    int maxx=0,maxy=0;
    auto pixels = graph->GetCacheIterator(NODELABEL_P) ;
    for(auto a : pixels)
    {
        maxx = std::max(maxx,graph->operator[](a).x);
        maxy = std::max(maxy,graph->operator[](a).y);
    }
    Image* img = new Image(++maxx, ++maxy);
    for (auto p : pixels)
    {
        auto pixel = graph->operator[](p);
        img->SetSquare(pixel.x,pixel.y,0,0,3);
        img->SetSquare(pixel.x,pixel.y,1,0,3);
        img->SetSquare(pixel.x,pixel.y,2,0,3);
        auto adjacentEdges = graph -> GetAdjacentVertices(p);
        std::vector<vertex_descriptor> adjacentPixels;
        for(auto edge : adjacentEdges)
        {
            img -> DrawBlackLine(pixel.x,pixel.y, graph->operator[](edge).x,graph->operator[](edge).y);
        }
    }
    img -> save(filename);
    delete img;
    img=0;
}




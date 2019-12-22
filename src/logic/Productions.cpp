#include <iostream>
#include "Productions.hpp"
#include "GraphessorConstants.hpp"
#include <linq.h>

using namespace linq;

void P2(MyGraph& graph, std::vector<vertex_descriptor>& listOfIEdges, Image image)
{
    vertex_iterator currentEdge, lastEdge;
    std::tie(currentEdge, lastEdge) = vertices(graph);
    std::cout<<(*currentEdge)<<std::endl;

    adjacency_iterator currentNeighbour, endOfNeighbours;
    std::vector<vertex_descriptor> neighbours;
    for(vertex_descriptor e : listOfIEdges)
    {
        if(graph[e].label==NODELABEL_I and graph[e]._break==1)
        {
            std::tie(currentNeighbour, endOfNeighbours) = adjacent_vertices(e, graph);
            for(;currentNeighbour!=endOfNeighbours;++currentNeighbour)
            {
                neighbours.push_back(*currentNeighbour);
            }
            if(neighbours.size()==4)
            {
                int x = graph[e].x;
                int y = graph[e].y;
                int r,g,b;
                std::tie(r,g,b) = image.getPixel(x,y);
                int dx = abs(x - graph[neighbours[0]].x);
                int dy = abs(y - graph[neighbours[0]].y);
                auto newPixel = e;
                graph[e].r = r;
                graph[e].g = g;
                graph[e].b = b;
                graph[e].label = NODELABEL_P;
                graph[e]._break = 0;
                for(int i=0;i<=3;i++)
                {
                    auto newIEdge = add_vertex(*(new Pixel((x+graph[neighbours[i]].x)/2,(y+graph[neighbours[i]].y)/2, NODELABEL_I)), graph);
                    add_edge(neighbours[i], newIEdge, graph);
                    add_edge(newPixel, newIEdge, graph);
                    remove_edge(neighbours[i],e,graph);
                }
                auto F11 = add_vertex(*(new Pixel(x, y - dy/2, NODELABEL_F)), graph);
                add_edge(newPixel, F11, graph);
                auto F12 = add_vertex(*(new Pixel(x, y + dy/2, NODELABEL_F)), graph);
                add_edge(newPixel, F12, graph);
                auto F21 = add_vertex(*(new Pixel(x - dx/2, y, NODELABEL_F)), graph);
                add_edge(newPixel, F21, graph);
                auto F22 = add_vertex(*(new Pixel(x + dx/2, y, NODELABEL_F)), graph);              
                add_edge(newPixel, F22, graph);
            }
        }
    }
}

std::vector<vertex_descriptor> getAdjacentVertices(vertex_descriptor v, MyGraph g)
{
    std::vector<vertex_descriptor> result;
    adjacency_iterator currentNeighbour, endOfNeighbours;
    std::tie(currentNeighbour, endOfNeighbours) = adjacent_vertices(v, g);
    for(;currentNeighbour!=endOfNeighbours;++currentNeighbour)
    {
        result.push_back(*currentNeighbour);
    }
    return result;
}

void P3(MyGraph& graph, std::vector<vertex_descriptor>& listOfBEdges, Image image)
{
    vertex_iterator currentEdge, lastEdge;
    std::tie(currentEdge, lastEdge) = vertices(graph);
    std::cout<<(*currentEdge)<<std::endl;

    adjacency_iterator currentNeighbour, endOfNeighbours;
    std::vector<vertex_descriptor> neighbours;
    std::vector<vertex_descriptor> toBeAdded;
    for(vertex_descriptor e : listOfBEdges)
    {
        if(graph[e].label==NODELABEL_B)
        {
            std::tie(currentNeighbour, endOfNeighbours) = adjacent_vertices(e, graph);
            for(;currentNeighbour!=endOfNeighbours;++currentNeighbour)
            {
                neighbours.push_back(*currentNeighbour);
            }
            if(neighbours.size()!=2)
            {
                std::cerr<<"incorrect B-edge containing "<<neighbours.size()<<" neighbours"<<std::endl;
                continue;
            }
            vertex_descriptor leftPixel = neighbours[0], rightPixel = neighbours[1];
            bool isVertical = graph[neighbours[0]].x==graph[neighbours[1]].x;
            std::vector<vertex_descriptor> leftIEdges = getAdjacentVertices(neighbours[0], graph);
            std::vector<vertex_descriptor> rightIEdges = getAdjacentVertices(neighbours[1], graph);
            auto common = (leftIEdges | intersect(rightIEdges));
            if(!common.empty())
            {
                continue; //there is common IEdge between neighbour pixels, so this B cannot be used in P3 production
            }
            std::vector<vertex_descriptor> leftPixels;
            std::vector<vertex_descriptor> rightPixels;
            for(auto IEdge : leftIEdges)
            {
                auto adjacentElems = getAdjacentVertices(IEdge, graph);
                leftPixels.insert(leftPixels.end(), adjacentElems.begin(), adjacentElems.end()); 
            }
            for(auto IEdge : rightIEdges)
            {
                auto adjacentElems = getAdjacentVertices(IEdge, graph);
                rightPixels.insert(rightPixels.end(), adjacentElems.begin(), adjacentElems.end()); 
            }
            common = leftPixels | intersect(rightPixels);
            if(common.empty())
            {
                std::cerr<<"P3: no common pixel found. Probably a bug."<<std::endl;
                continue;
            }
            vertex_descriptor centerPixel = common | first;
            vertex_descriptor leftI = common | first;
            vertex_descriptor rightI = common | first;
            auto adjacentVertices = getAdjacentVertices(centerPixel, graph);
            auto FEdges =  adjacentVertices | where([graph](vertex_descriptor v){return graph[v].label==NODELABEL_F;});
            vertex_descriptor FEdge = FEdges | first; //TODO: this FEdge might already be used by another BEdge. should be filtered to only right one.
            int x=(graph[leftPixel].x + graph[rightPixel].x)/2,
                y=(graph[leftPixel].y + graph[rightPixel].y)/2;
            auto newPixel = add_vertex(*(new Pixel(x, y, NODELABEL_P)), graph);
            int r,g,b;
            std::tie(r,g,b) = image.getPixel(x,y);
            graph[newPixel].r = r;
            graph[newPixel].g = g;
            graph[newPixel].b = b;
            add_edge(newPixel, e, graph);
            add_edge(newPixel, FEdge, graph);

            auto temp = getAdjacentVertices(leftPixel, graph);
            auto leftIEdge =  temp | intersect(getAdjacentVertices(centerPixel,graph)) | first;
            temp = getAdjacentVertices(rightPixel, graph);
            auto rightIEdge = temp | intersect(getAdjacentVertices(centerPixel,graph)) | first;

            add_edge(newPixel, leftIEdge, graph);
            add_edge(newPixel, rightIEdge, graph);

            auto newBEdge = add_vertex(*(new Pixel((graph[leftPixel].x+x)/2, (graph[leftPixel].y+y)/2, NODELABEL_B)), graph);
            graph[e].x = (graph[rightPixel].x+x)/2;
            graph[e].y = (graph[rightPixel].y+y)/2;
            
            add_edge(leftPixel, newBEdge,graph);
            add_edge(newPixel, newBEdge,graph);

            remove_edge(e, leftPixel, graph);
            toBeAdded.push_back(newBEdge);
        }
    }
    listOfBEdges.insert(listOfBEdges.end(), toBeAdded.begin(), toBeAdded.end());
}
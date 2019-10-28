#include <iostream>
#include "Productions.hpp"

void P2(MyGraph& graph, std::vector<vertex_descriptor> listOfIEdges, Image image)
{
    vertex_iterator currentEdge, lastEdge;
    std::tie(currentEdge, lastEdge) = vertices(graph);
    std::cout<<(*currentEdge)<<std::endl;

    adjacency_iterator currentNeighbour, endOfNeighbours;
    std::vector<vertex_descriptor> neighbours;
    for(vertex_descriptor e : listOfIEdges)
    {
        if(graph[e].label=="I" and graph[e]._break==1)
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
                graph[e].label = "P";
                graph[e]._break = 0;
                for(int i=0;i<=3;i++)
                {
                    auto newIEdge = add_vertex(*(new Pixel((x+graph[neighbours[i]].x)/2,(y+graph[neighbours[i]].y)/2, "I")), graph);
                    add_edge(neighbours[i], newIEdge, graph);
                    add_edge(newPixel, newIEdge, graph);
                    remove_edge(neighbours[i],e,graph);
                }
                auto F11 = add_vertex(*(new Pixel(x, y - dy/2, "F1")), graph);
                add_edge(newPixel, F11, graph);
                auto F12 = add_vertex(*(new Pixel(x, y + dy/2, "F1")), graph);
                add_edge(newPixel, F12, graph);
                auto F21 = add_vertex(*(new Pixel(x - dx/2, y, "F2")), graph);
                add_edge(newPixel, F21, graph);
                auto F22 = add_vertex(*(new Pixel(x + dx/2, y, "F2")), graph);              
                add_edge(newPixel, F22, graph);
            }
        }
    }
}
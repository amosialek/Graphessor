#include <vector>
#include <iostream>
#include "MyGraph.hpp"
#include "Image.hpp"

void P2(MyGraph graph, std::vector<vertex_descriptor> listOfIEdges, Image image)
{
    vertex_iterator currentEdge, lastEdge;
    std::tie(currentEdge, lastEdge) = vertices(graph);
    std::cout<<(*currentEdge)<<std::endl;

    adjacency_iterator currentNeighbour, endOfNeighbours;
    std::vector<vertex_descriptor> neighbours;
    for(auto e : listOfIEdges)
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
                auto newPixel = add_vertex(*(new Pixel(x,y, r,g,b)), graph);
                for(int i=0;i<3;i++)
                {
                    auto newIEdge = add_vertex(*(new Pixel((x+graph[neighbours[i]].x)/2,(y+graph[neighbours[i]].y)/2, "I")), graph);
                    //add_edge(neighbours[0], newIEdge, graph);
                    //add_edge(newPixel, newIEdge, graph);
                }
                auto F11 = add_vertex(*(new Pixel(x, y - dy, "F1")), graph);
                auto F12 = add_vertex(*(new Pixel(x, y + dy, "F1")), graph);
                auto F21 = add_vertex(*(new Pixel(x - dx, y, "F2")), graph);
                auto F22 = add_vertex(*(new Pixel(x + dx, y, "F2")), graph);

                //remove_vertex(e,graph);
            }
        }
    }
}
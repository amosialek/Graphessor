#include <iostream>
#include "Productions.hpp"
#include "GraphessorConstants.hpp"
#include <linq.h>
#include <algorithm>

using namespace linq;

void P1(
    MyGraph& graph,
    vertex_descriptor S,
    std::vector<vertex_descriptor>& listOfIEdges,
    std::vector<vertex_descriptor>& listOfBEdges,
    Image image)
{
    int y = image.height(),
        x = image.width(),
        r,g,b;
    auto I = S;
    graph[I].x=x/2;
    graph[I].y=y/2;
    graph[I].label=NODELABEL_I;
    listOfIEdges.push_back(I);

    auto BTop = add_vertex(*(new Pixel(x/2, 0, NODELABEL_B)), graph);
    auto BLeft = add_vertex(*(new Pixel(0, y/2, NODELABEL_B)), graph);
    auto BBot = add_vertex(*(new Pixel(x/2, y-1, NODELABEL_B)), graph);
    auto BRight = add_vertex(*(new Pixel(x-1, y/2, NODELABEL_B)), graph);
    
    listOfBEdges.push_back(BTop);
    listOfBEdges.push_back(BBot);
    listOfBEdges.push_back(BLeft);
    listOfBEdges.push_back(BRight);
    
    std::tie(r,g,b) = image.getPixel(0,0);
    auto PTopLeft = add_vertex(*(new Pixel(0, 0, r,g,b)), graph);
    std::tie(r,g,b) = image.getPixel(x-1, 0);
    auto PTopRight = add_vertex(*(new Pixel(x-1, 0, r,g,b)), graph);
    std::tie(r,g,b) = image.getPixel(x-1, y-1);
    auto PBotRight = add_vertex(*(new Pixel(x-1, y-1, r,g,b)), graph);
    std::tie(r,g,b) = image.getPixel(0, y-1);
    auto PBotLeft = add_vertex(*(new Pixel(0, y-1, r,g,b)), graph);

    add_edge(PBotLeft, I, graph);
    add_edge(PBotRight, I, graph);
    add_edge(PTopLeft, I, graph);
    add_edge(PTopRight, I, graph);

    add_edge(PBotLeft, BBot , graph);
    add_edge(PBotRight, BBot, graph);
    add_edge(PTopLeft, BTop, graph);
    add_edge(PTopRight, BTop, graph);

    add_edge(PBotLeft, BLeft, graph);
    add_edge(PTopLeft, BLeft, graph);
    add_edge(PBotRight, BRight, graph);
    add_edge(PTopRight, BRight, graph);
}

void P2(
    MyGraph& graph, 
    std::vector<vertex_descriptor>& listOfIEdges,
    Image image)
{
    vertex_iterator currentEdge, lastEdge;
    std::tie(currentEdge, lastEdge) = vertices(graph);
    std::cout<<(*currentEdge)<<std::endl;

    adjacency_iterator currentNeighbour, endOfNeighbours;
    std::vector<vertex_descriptor> neighbours;
    std::vector<vertex_descriptor> IEdgesToBeAdded;
    std::set<vertex_descriptor> IEdgesToBeDeleted;
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
                IEdgesToBeDeleted.insert(e);
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
                    IEdgesToBeAdded.push_back(newIEdge);
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
    listOfIEdges.insert(listOfIEdges.end(), IEdgesToBeAdded.begin(), IEdgesToBeAdded.end());
    listOfIEdges.erase(std::remove_if(listOfIEdges.begin(),listOfIEdges.end(),[IEdgesToBeDeleted](vertex_descriptor v){return IEdgesToBeDeleted.find(v) != IEdgesToBeDeleted.end();}),listOfIEdges.end());
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

const double GetDistance(const int x1,const int x2,const int y1,const int y2)//TODO: Consider long longs (for very large images)?
{
    return (x1-x2)*(x1-x2)+(y1-y2)*(y1-y2);
}

const double GetDistance(const Pixel p1, const Pixel p2)
{
    return GetDistance(p1.x, p2.x, p1.y, p2.y);
}

void P3(MyGraph& graph, std::vector<vertex_descriptor>& listOfBEdges, Image image)
{
    vertex_iterator currentEdge, lastEdge;
    std::tie(currentEdge, lastEdge) = vertices(graph);
    std::cout<<(*currentEdge)<<std::endl;

    adjacency_iterator currentNeighbour, endOfNeighbours;
    std::vector<vertex_descriptor> neighbours;
    std::vector<vertex_descriptor> toBeAdded;
    std::vector<vertex_descriptor> leftPixels;
    std::vector<vertex_descriptor> rightPixels;
    for(vertex_descriptor e : listOfBEdges)
    {
        std::cout<<"---------------------another B edge---------------------"<<std::endl;
        if(graph[e].label==NODELABEL_B)
        {
            leftPixels.clear();
            rightPixels.clear();
            neighbours.clear();


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
            std::cout<<"neighbours[0] "<<graph[neighbours[0]].x<<" "<<graph[neighbours[0]].y<<std::endl;
            std::cout<<"neighbours[1] "<<graph[neighbours[1]].x<<" "<<graph[neighbours[1]].y<<std::endl;
            bool isVertical = graph[neighbours[0]].x==graph[neighbours[1]].x;
            auto leftAdjacent = getAdjacentVertices(neighbours[0], graph);
            auto rightAdjacent = getAdjacentVertices(neighbours[1], graph);
            auto leftIEdges = leftAdjacent | where([graph](vertex_descriptor v){return graph[v].label==NODELABEL_I;});
            auto rightIEdges = rightAdjacent | where([graph](vertex_descriptor v){return graph[v].label==NODELABEL_I;});

            auto common2 = (leftIEdges | where([graph](vertex_descriptor v){return graph[v].label==NODELABEL_I;}) 
                | intersect(rightIEdges | where([graph](vertex_descriptor v){return graph[v].label==NODELABEL_I;})));
            if(!common2.empty())
            {
                std::cout<<"commonIEdge"<<std::endl;
                vertex_descriptor tmpIEdge = common2 | first;
                std::cout<<"x: "<<graph[tmpIEdge].x<<", y: "<<graph[tmpIEdge].y<<std::endl;
                continue; //there is common IEdge between neighbour pixels, so this B cannot be used in P3 production
            }
            
            for(const auto &edge : leftIEdges)
            {
                auto adjacentElems = getAdjacentVertices(edge, graph);
                leftPixels.insert(leftPixels.end(), adjacentElems.begin(), adjacentElems.end()); 
            }
            for(const auto &edge : rightIEdges)
            {
                auto adjacentElems = getAdjacentVertices(edge, graph);
                rightPixels.insert(rightPixels.end(), adjacentElems.begin(), adjacentElems.end()); 
            }
            auto common = leftPixels | intersect(rightPixels);
            if(common.empty())
            {
                std::cerr<<"P3: no common pixel found. Probably a bug."<<std::endl;
                continue;
            }
            std::cout<<"producing..."<<std::endl;
            vertex_descriptor centerPixel = common | first;
            std::cout<<"center pixel "<<graph[centerPixel].x<<" "<<graph[centerPixel].y<<std::endl;
            auto adjacentVertices = getAdjacentVertices(centerPixel, graph);
            vertex_descriptor leftI = leftIEdges | where([adjacentVertices](vertex_descriptor v){return adjacentVertices | contains(v);}) | first;
            vertex_descriptor rightI = rightIEdges | where([adjacentVertices](vertex_descriptor v){return adjacentVertices | contains(v);}) | first;
            auto FEdges =  adjacentVertices | where([graph](vertex_descriptor v){return graph[v].label==NODELABEL_F;});
            std::cout<<"FEdges.size() "<< (FEdges.end()==FEdges.begin()) <<std::endl;
            auto FEdgesOrdered = LINQ(from(v, FEdges) orderby(GetDistance(graph[v], graph[e])));
            std::cout<<graph[*FEdgesOrdered.begin()].x<<" "<<graph[*FEdgesOrdered.begin()].y<<std::endl;;
            for(const auto &it :FEdgesOrdered)
            {
                std::cout<<"graph[fedge]"<<graph[it].x<<" "<<graph[it].y<<" "<<GetDistance(graph[it],graph[e])<< std::endl;
                
            }
            auto FEdge =*(FEdgesOrdered.begin());
            std::cout<<"chosen FEdge: "<<graph[FEdge].x<<" "<<graph[FEdge].y<<std::endl;
            std::cout<<"tam"<<std::endl;
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
#include <iostream>
#include "Productions.hpp"
#include "GraphessorConstants.hpp"
#include <linq.h>
#include <algorithm>
#include <queue>

template<typename T, typename Pred>
std::vector<T> where(std::vector<T>& inVec, Pred predicate)
{
  std::vector<T> result;
  std::copy_if(inVec.begin(), inVec.end(), std::back_inserter(result), predicate);
  return result;
}

template<typename T>
std::vector<T> intersect(std::vector<T>& inVec1, std::vector<T>& inVec2)
{
  std::vector<T> result;
  if(inVec1.size()<inVec2.size())
  {
    std::set<T> tempset(inVec1.begin(), inVec1.end());
    std::copy_if(inVec2.begin(), inVec2.end(), std::back_inserter(result), [tempset](T t){return tempset.find(t)!=tempset.end();});
  }
  else
  {
    std::set<T> tempset(inVec2.begin(), inVec2.end());
    std::copy_if(inVec1.begin(), inVec1.end(), std::back_inserter(result), [tempset](T t){return tempset.find(t)!=tempset.end();});  
  }
  return result;
}

std::vector<vertex_descriptor> GetAdjacentVertices(vertex_descriptor v, MyGraph& g)
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

template <class T>
std::vector<vertex_descriptor> GetAdjacentVertices(const T &vertices, MyGraph& g)
{
    std::vector<vertex_descriptor> result;
    for(auto v : vertices)
    {
        auto currentNeighbours = GetAdjacentVertices(v, g);
        result.insert(result.end(),currentNeighbours.begin(), currentNeighbours.end());
    }
    return result;
}

void P1(
    MyGraph& graph,
    vertex_descriptor S,
    std::vector<vertex_descriptor>& listOfIEdges,
    std::vector<vertex_descriptor>& listOfBEdges,
    Image& image)
{
    int y = image.height()-1,
        x = image.width()-1,
        r,g,b;
    auto I = S;
    graph[I].x=x/2;
    graph[I].y=y/2;
    graph[I].label=NODELABEL_I;
    listOfIEdges.push_back(I);

    auto BTop = add_vertex(*(new Pixel(x/2, 0, NODELABEL_B)), graph);
    auto BLeft = add_vertex(*(new Pixel(0, y/2, NODELABEL_B)), graph);
    auto BBot = add_vertex(*(new Pixel(x/2, y, NODELABEL_B)), graph);
    auto BRight = add_vertex(*(new Pixel(x, y/2, NODELABEL_B)), graph);
    
    listOfBEdges.push_back(BTop);
    listOfBEdges.push_back(BBot);
    listOfBEdges.push_back(BLeft);
    listOfBEdges.push_back(BRight);
    
    std::tie(r,g,b) = image.getPixel(0,0);
    auto PTopLeft = add_vertex(*(new Pixel(0, 0, r,g,b)), graph);
    std::tie(r,g,b) = image.getPixel(x, 0);
    auto PTopRight = add_vertex(*(new Pixel(x, 0, r,g,b)), graph);
    std::tie(r,g,b) = image.getPixel(x, y);
    auto PBotRight = add_vertex(*(new Pixel(x, y, r,g,b)), graph);
    std::tie(r,g,b) = image.getPixel(0, y);
    auto PBotLeft = add_vertex(*(new Pixel(0, y, r,g,b)), graph);

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
    std::vector<vertex_descriptor>& listOfFEdges,
    Image& image)
{
    vertex_iterator currentEdge, lastEdge;
    std::tie(currentEdge, lastEdge) = vertices(graph);

    std::vector<vertex_descriptor> neighbours;
    std::vector<vertex_descriptor> IEdgesToBeAdded;
    std::set<vertex_descriptor> IEdgesToBeDeleted;
    for(vertex_descriptor e : listOfIEdges)
    {
        if(graph[e].label==NODELABEL_I and graph[e]._break==1)
        {
            neighbours = GetAdjacentVertices(e, graph);
            if(neighbours.size()==4)
            {
                IEdgesToBeDeleted.insert(e);
                int x = graph[e].x;
                int y = graph[e].y;
                int r,g,b;
                std::tie(r,g,b) = image.getPixel(x,y);
                int dx = abs(x - graph[neighbours[0]].x);
                int dy = abs(y - graph[neighbours[0]].y);
                if(dx<=2 || dy<=2)
                    continue; //don't want to have too many pixels
                auto newPixel = e;
                graph[e].r = r;
                graph[e].g = g;
                graph[e].b = b;
                graph[e].label = NODELABEL_P;
                graph[e]._break = 0;
                for(int i=0;i<=3;i++)
                {
                    auto newIEdge = add_vertex(*(new Pixel((x+graph[neighbours[i]].x)/2,(y+graph[neighbours[i]].y)/2, NODELABEL_I)), graph);
                    graph[newIEdge].breakLevel = graph[e].breakLevel+1;
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
                listOfFEdges.push_back(F11);
                listOfFEdges.push_back(F12);
                listOfFEdges.push_back(F21);
                listOfFEdges.push_back(F22);
            }
        }
    }
    listOfIEdges.insert(listOfIEdges.end(), IEdgesToBeAdded.begin(), IEdgesToBeAdded.end());
    listOfIEdges.erase(std::remove_if(listOfIEdges.begin(),listOfIEdges.end(),[IEdgesToBeDeleted](vertex_descriptor v){return IEdgesToBeDeleted.find(v) != IEdgesToBeDeleted.end();}),listOfIEdges.end());
}

const double GetDistance(const int x1,const int x2,const int y1,const int y2)//TODO: Consider long longs (for very large images)?
{
    return (x1-x2)*(x1-x2)+(y1-y2)*(y1-y2);
}

const double GetDistance(const Pixel p1, const Pixel p2)
{
    return GetDistance(p1.x, p2.x, p1.y, p2.y);
}

void P3(MyGraph& graph, std::vector<vertex_descriptor>& listOfBEdges, Image& image)
{
    vertex_iterator currentEdge, lastEdge;
    std::tie(currentEdge, lastEdge) = vertices(graph);

    adjacency_iterator currentNeighbour, endOfNeighbours;
    std::vector<vertex_descriptor> neighbours;
    std::vector<vertex_descriptor> toBeAdded;
    std::vector<vertex_descriptor> leftPixels;
    std::vector<vertex_descriptor> rightPixels;
    for(vertex_descriptor BEdge : listOfBEdges)
    {
        if(graph[BEdge].label==NODELABEL_B)
        {
            leftPixels.clear();
            rightPixels.clear();
            neighbours.clear();


            std::tie(currentNeighbour, endOfNeighbours) = adjacent_vertices(BEdge, graph);
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
            auto leftAdjacent = GetAdjacentVertices(neighbours[0], graph);
            auto rightAdjacent = GetAdjacentVertices(neighbours[1], graph);
            auto leftIEdges = where(leftAdjacent,[&graph](const vertex_descriptor& v){return graph[v].label==NODELABEL_I;});
            auto rightIEdges = where(rightAdjacent, [&graph](const vertex_descriptor &v){return graph[v].label==NODELABEL_I;});

            auto common2 = intersect(leftIEdges,rightIEdges);
            if(!common2.empty())
            {
                continue; //there is common IEdge between neighbour pixels, so this B cannot be used in P3 production
            }
            
            for(const auto &edge : leftIEdges)
            {
                auto adjacentElems = GetAdjacentVertices(edge, graph);
                leftPixels.insert(leftPixels.end(), adjacentElems.begin(), adjacentElems.end()); 
            }
            for(const auto &edge : rightIEdges)
            {
                auto adjacentElems = GetAdjacentVertices(edge, graph);
                rightPixels.insert(rightPixels.end(), adjacentElems.begin(), adjacentElems.end()); 
            }
            auto common = intersect(leftPixels,rightPixels);
            if(common.empty())
            {
                std::cerr<<"P3: no common pixel found. Probably a bug."<<std::endl;
                continue;
            }
            vertex_descriptor centerPixel = *common.begin();
            auto adjacentVertices = GetAdjacentVertices(centerPixel, graph);
            vertex_descriptor leftI = *where(leftIEdges, [adjacentVertices](vertex_descriptor v){return adjacentVertices | linq::contains(v);}).begin();
            vertex_descriptor rightI = *where(rightIEdges, [adjacentVertices](vertex_descriptor v){return adjacentVertices | linq::contains(v);}).begin();
            auto FEdges =  where(adjacentVertices, [&graph](vertex_descriptor v){return graph[v].label==NODELABEL_F;});
            auto FEdgesOrdered = LINQ(from(v, FEdges) orderby(GetDistance(graph[v], graph[BEdge])));
            auto FEdge =*(FEdgesOrdered.begin());
            int x=(graph[leftPixel].x + graph[rightPixel].x)/2,
                y=(graph[leftPixel].y + graph[rightPixel].y)/2;
            auto newPixel = add_vertex(*(new Pixel(x, y, NODELABEL_P)), graph);
            int r,g,b;
            std::tie(r,g,b) = image.getPixel(x,y);
            graph[newPixel].r = r;
            graph[newPixel].g = g;
            graph[newPixel].b = b;
            add_edge(newPixel, BEdge, graph);
            add_edge(newPixel, FEdge, graph);

            auto temp = GetAdjacentVertices(leftPixel, graph);
            auto leftIEdge =  *(temp | linq::intersect(GetAdjacentVertices(centerPixel,graph))).begin();
            temp = GetAdjacentVertices(rightPixel, graph);
            auto rightIEdge = *(temp | linq::intersect(GetAdjacentVertices(centerPixel,graph))).begin();

            add_edge(newPixel, leftIEdge, graph);
            add_edge(newPixel, rightIEdge, graph);

            auto newBEdge = add_vertex(*(new Pixel((graph[leftPixel].x+x)/2, (graph[leftPixel].y+y)/2, NODELABEL_B)), graph);
            graph[BEdge].x = (graph[rightPixel].x+x)/2;
            graph[BEdge].y = (graph[rightPixel].y+y)/2;
            
            add_edge(leftPixel, newBEdge,graph);
            add_edge(newPixel, newBEdge,graph);

            remove_edge(BEdge, leftPixel, graph);
            toBeAdded.push_back(newBEdge);
        }
    }
    listOfBEdges.insert(listOfBEdges.end(), toBeAdded.begin(), toBeAdded.end());
}

void P4(MyGraph& graph, std::vector<vertex_descriptor>& listOfFEdges, Image& image)
{
    auto interestingEdges = where(listOfFEdges, [&graph](vertex_descriptor v){return GetAdjacentVertices(v,graph).size()==2;});

    std::vector<vertex_descriptor> toBeAdded;
    for (auto e: interestingEdges)
    {
        auto adjacentPixels = GetAdjacentVertices(e, graph);
        bool isVertical = graph[adjacentPixels[0]].x==graph[adjacentPixels[1]].x;
        int bound1 = isVertical 
            ? std::min(graph[adjacentPixels[0]].y, graph[adjacentPixels[1]].y) 
            : std::min(graph[adjacentPixels[0]].x, graph[adjacentPixels[1]].x);
        int bound2 = isVertical 
            ? std::max(graph[adjacentPixels[0]].y, graph[adjacentPixels[1]].y) 
            : std::max(graph[adjacentPixels[0]].x, graph[adjacentPixels[1]].x); 
        auto isInVerticalBounds = [&graph, bound1, bound2](vertex_descriptor v){return graph[v].y>=bound1 and graph[v].y<=bound2;};
        auto isInHorizontalBounds = [&graph, bound1, bound2](vertex_descriptor v){return graph[v].x>=bound1 and graph[v].x<=bound2;};
        auto isInBounds = [isVertical, isInHorizontalBounds, isInVerticalBounds](vertex_descriptor v){return (isVertical and isInVerticalBounds(v))or(!isVertical and isInHorizontalBounds(v));};
        auto leftEdges = GetAdjacentVertices(adjacentPixels[0], graph);
        auto leftIEdges = where(leftEdges, [&graph, &bound1, &bound2, &isVertical](const vertex_descriptor& v){return graph[v].label==NODELABEL_I and ((isVertical and graph[v].y>=bound1 and graph[v].y<=bound2)or(!isVertical and graph[v].x>=bound1 and graph[v].x<=bound2));});
        auto rightEdges = GetAdjacentVertices(adjacentPixels[1], graph);
        auto rightIEdges = where(rightEdges, [&graph, &bound1, &bound2, &isVertical](const vertex_descriptor& v){return graph[v].label==NODELABEL_I and ((isVertical and graph[v].y>=bound1 and graph[v].y<=bound2)or(!isVertical and graph[v].x>=bound1 and graph[v].x<=bound2));});
        auto leftSecondPixels = GetAdjacentVertices(leftIEdges, graph);
        auto rightSecondPixels = GetAdjacentVertices(rightIEdges, graph);
        auto leftSecondPixelsInRange = where(leftSecondPixels, [&graph, &bound1, &bound2, &isVertical](vertex_descriptor v){return (isVertical and graph[v].y>=bound1 and graph[v].y<=bound2)or(!isVertical and graph[v].x>=bound1 and graph[v].x<=bound2);});
        auto rightSecondPixelsInRange = where(rightSecondPixels, [&graph, &bound1, &bound2, &isVertical](vertex_descriptor v){return (isVertical and graph[v].y>=bound1 and graph[v].y<=bound2)or(!isVertical and graph[v].x>=bound1 and graph[v].x<=bound2);});
        auto commonPixels = intersect(leftSecondPixelsInRange,rightSecondPixelsInRange);
        {
            int count=0;
            for(const auto p : commonPixels ){
                count++;
                //std::cout<<graph[p].x<<" "<<graph[p].y<<std::endl;
            }
            if(count!=2)
            {
                //std::cout<<"Wrong Number common pixels ("<<count<<")"<<std::endl;
                continue;
            }
        }
        auto commonPixelsAdjacent = GetAdjacentVertices(commonPixels, graph);
        auto firstCommonPixel = graph[*commonPixels.begin()];
        auto it = commonPixels.begin();
        auto second = ++it;
        auto lastCommonPixel = graph[*second];
        auto commonPixelsAdjacentFEdges = where(commonPixelsAdjacent ,
            [&graph, firstCommonPixel, lastCommonPixel](vertex_descriptor v)
                {
                    Pixel p = graph[v];
                    int xrange = std::abs(lastCommonPixel.x - firstCommonPixel.x);
                    int yrange = std::abs(lastCommonPixel.y - firstCommonPixel.y);
                    return true
                        and p.label==NODELABEL_F
                        and std::abs(p.x-firstCommonPixel.x)<=xrange
                        and std::abs(p.x-lastCommonPixel.x)<=xrange
                        and std::abs(p.y-firstCommonPixel.y)<=yrange
                        and std::abs(p.y-lastCommonPixel.y)<=yrange
                    ;   
                });
        int x = (firstCommonPixel.x + lastCommonPixel.x)/2;
        int y = (firstCommonPixel.y + lastCommonPixel.y)/2;
        int r,g,b;
        std::tie(r,g,b) = image.getPixel(x,y);
        auto newPixel = add_vertex(*(new Pixel(x, y, r, g, b)), graph);
        auto newFEdge = add_vertex(*(new Pixel((x+graph[adjacentPixels[1]].x)/2, (y+graph[adjacentPixels[1]].y)/2, NODELABEL_F)), graph);
        for(auto FEdge : commonPixelsAdjacentFEdges)
            add_edge(newPixel, FEdge, graph);
        for(auto IEdge : leftIEdges)
            add_edge(newPixel, IEdge, graph);
        for(auto IEdge : rightIEdges)
            add_edge(newPixel, IEdge, graph);
        add_edge(newPixel, e, graph);
        add_edge(newPixel, newFEdge, graph);
        add_edge(adjacentPixels[1], newFEdge, graph);

        remove_edge(adjacentPixels[1], e, graph);

        graph[e].x = (x+graph[adjacentPixels[0]].x)/2;
        graph[e].y = (y+graph[adjacentPixels[0]].y)/2;
        toBeAdded.push_back(newFEdge);
    }
    listOfFEdges.insert(listOfFEdges.end(), toBeAdded.begin(), toBeAdded.end());
}

std::map<vertex_descriptor,std::vector<vertex_descriptor>> vertexToNeighbours;
std::map<vertex_descriptor,long long> IEdgeToError;

void P5(
    MyGraph& graph, 
    std::vector<vertex_descriptor>& listOfIEdges,
    Image& image,
    double epsilon
)
{
    long long sumError=0;
    long long maxError=0;
    vertexToNeighbours.clear();
    int width = image.width();
    int height = image.height();
    auto IEdges = listOfIEdges 
        | linq::select([&graph](vertex_descriptor v){vertexToNeighbours[v] = GetAdjacentVertices(v, graph); return v;}) 
        | linq::where([](vertex_descriptor v){return vertexToNeighbours[v].size()==4;});
    for(auto IEdge : IEdges)
    {
        int minx = width;
        int maxx = 0;
        int miny = height;
        int maxy = 0;
        for(auto v :vertexToNeighbours[IEdge])
        {
            minx = std::min(minx, graph[v].x);
            maxx = std::max(maxx, graph[v].x);
            miny = std::min(miny, graph[v].y);
            maxy = std::max(maxy, graph[v].y);
        } 
        long long error = image.CompareWithInterpolation(minx, maxx, miny, maxy);
        IEdgeToError[IEdge] = error;
        sumError += error;
        maxError = std::max(maxError, error);
    }
    for(auto IEdge : IEdges)
        if(IEdgeToError[IEdge] > epsilon*maxError)
            {
                graph[IEdge]._break = 1;
            }
}

std::queue<vertex_descriptor> toBeVisited;

void P6(
    MyGraph& graph, 
    std::vector<vertex_descriptor>& listOfIEdges
)
{
    for(auto IEdge: listOfIEdges)
    {
        graph[IEdge].visited=0;
    }
    for(auto IEdge: listOfIEdges)
    {
        if(graph[IEdge]._break and !graph[IEdge].visited)
        {
            std::vector<vertex_descriptor> adjacentEdges;
            toBeVisited.push(IEdge);
            while(!toBeVisited.empty()) //BFS through IEdges with break==0
            {
                auto currentIEdge = toBeVisited.front();
                toBeVisited.pop();
                auto adjacentVertices = GetAdjacentVertices(currentIEdge, graph);
                for(auto v: adjacentVertices)
                {
                    auto adjacentEdgesTemp = GetAdjacentVertices(v, graph);
                    adjacentEdges.insert(adjacentEdges.end(),adjacentEdgesTemp.begin(), adjacentEdgesTemp.end());
                }
                auto adjacentIEdges = adjacentEdges 
                    | linq::where([&graph, currentIEdge](vertex_descriptor v){
                        return true
                            && graph[v].label==NODELABEL_I 
                            && graph[v]._break==0 
                            && graph[v].breakLevel<graph[currentIEdge].breakLevel;}
                            );
                for(auto a : adjacentIEdges)
                {
                    graph[a]._break=1;
                    toBeVisited.push(a);
                }
            }
        }
    }

}
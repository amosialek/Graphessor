#include <iostream>
#include "Productions.hpp"
#include "GraphessorConstants.hpp"
#include <linq.h>
#include <algorithm>
#include <queue>

class NotImplementedException : public std::logic_error
{
public:
    NotImplementedException() : std::logic_error("Function not yet implemented") { };
};

template<typename T, typename Pred>
std::vector<T> where(std::vector<T> inVec, Pred predicate)
{
  std::vector<T> result;
  std::copy_if(inVec.begin(), inVec.end(), std::back_inserter(result), predicate);
  return result;
}

template<typename T, typename Pred>
std::set<T> where(std::set<T> inVec, Pred predicate)
{
  std::set<T> result;
  std::copy_if(inVec.begin(), inVec.end(), std::inserter(result, result.begin()), predicate);
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



template <class T>
std::vector<vertex_descriptor> GetAdjacentVertices(const T &vertices, IGraph& g)
{
    std::vector<vertex_descriptor> result;
    for(auto v : vertices)
    {
        auto currentNeighbours = g.GetAdjacentVertices(v);
        result.insert(result.end(),currentNeighbours.begin(), currentNeighbours.end());
    }
    return result;
}

P1::P1(std::shared_ptr<IGraph> graph,
        vertex_descriptor S,
        std::shared_ptr<std::vector<vertex_descriptor>> listOfIEdges,
        std::shared_ptr<std::vector<vertex_descriptor>> listOfBEdges,
        std::shared_ptr<Image> image) 
        :
             graph(graph), 
             S(S),
             listOfIEdges(listOfIEdges),
             listOfBEdges(listOfBEdges),
             image(image)
    {};

void P1::Perform()
{
    int y = image->height()-1,
        x = image->width()-1,
        r,g,b;
    auto I = S;
    (*graph)[I].x=x/2;
    (*graph)[I].y=y/2;
    (*graph)[I].label=NODELABEL_I;
    listOfIEdges->push_back(I);

    auto BTop = graph -> AddVertex(*(new Pixel(x/2, 0, NODELABEL_B)));
    auto BLeft = graph -> AddVertex(*(new Pixel(0, y/2, NODELABEL_B)));
    auto BBot = graph -> AddVertex(*(new Pixel(x/2, y, NODELABEL_B)));
    auto BRight = graph -> AddVertex(*(new Pixel(x, y/2, NODELABEL_B)));
    
    listOfBEdges->push_back(BTop);
    listOfBEdges->push_back(BBot);
    listOfBEdges->push_back(BLeft);
    listOfBEdges->push_back(BRight);
    
    std::tie(r,g,b) = image->getPixel(0,0);
    auto PTopLeft = graph -> AddVertex(*(new Pixel(0, 0, r,g,b)));
    std::tie(r,g,b) = image->getPixel(x, 0);
    auto PTopRight = graph -> AddVertex(*(new Pixel(x, 0, r,g,b)));
    std::tie(r,g,b) = image->getPixel(x, y);
    auto PBotRight = graph -> AddVertex(*(new Pixel(x, y, r,g,b)));
    std::tie(r,g,b) = image->getPixel(0, y);
    auto PBotLeft = graph -> AddVertex(*(new Pixel(0, y, r,g,b)));

    graph -> AddEdge(PBotLeft, I);
    graph -> AddEdge(PBotRight, I);
    graph -> AddEdge(PTopLeft, I);
    graph -> AddEdge(PTopRight, I);

    graph -> AddEdge(PBotLeft, BBot );
    graph -> AddEdge(PBotRight, BBot);
    graph -> AddEdge(PTopLeft, BTop);
    graph -> AddEdge(PTopRight, BTop);

    graph -> AddEdge(PBotLeft, BLeft);
    graph -> AddEdge(PTopLeft, BLeft);
    graph -> AddEdge(PBotRight, BRight);
    graph -> AddEdge(PTopRight, BRight);
}

std::vector<uint8_t> P1::Serialize()
{
    throw NotImplementedException();
}
std::vector<uint8_t> P2::Serialize()
{
    throw NotImplementedException();
}
std::vector<uint8_t> P3::Serialize()
{
    throw NotImplementedException();
}
std::vector<uint8_t> P4::Serialize()
{
    throw NotImplementedException();
}
std::vector<uint8_t> P5::Serialize()
{
    throw NotImplementedException();
}
std::vector<uint8_t> P6::Serialize()
{
    throw NotImplementedException();
}


P2::P2(
    std::shared_ptr<IGraph> graph, 
    vertex_descriptor IEdge,
    std::shared_ptr<Image> image):
        graph(graph),
        IEdge(IEdge),
        image(image){};

std::vector<P2> P2::FindAllMatches(std::shared_ptr<CachedGraph> graph,
    std::shared_ptr<Image> image)
{
    std::vector<P2> p2s;

    std::vector<vertex_descriptor> neighbours;
    std::vector<vertex_descriptor> IEdgesToBeAdded;
    std::set<vertex_descriptor> IEdgesToBeDeleted;
    for(vertex_descriptor e : graph -> getCacheIterator(NODELABEL_F))
    {
        if((*graph)[e].label==NODELABEL_I and (*graph)[e]._break==1)
        {
            neighbours = graph -> GetAdjacentVertices(e);
            if(neighbours.size()==4)
            {
                IEdgesToBeDeleted.insert(e);
                int x = (*graph)[e].x;
                int y = (*graph)[e].y;
                int r,g,b;
                std::tie(r,g,b) = image->getPixel(x,y);
                int dx = abs(x - (*graph)[neighbours[0]].x);
                int dy = abs(y - (*graph)[neighbours[0]].y);
                if(dx<=2 || dy<=2)
                    continue; //don't want to have too many pixels
                p2s.emplace_back(graph, e, image);
            }
        }
    }
    return p2s;
}

void P2::Perform()
{
    std::vector<vertex_descriptor> neighbours;
    auto e = IEdge;
    {
        if((*graph)[e].label==NODELABEL_I and (*graph)[e]._break==1)
        {
            neighbours = graph -> GetAdjacentVertices(e);
            int x = (*graph)[e].x;
            int y = (*graph)[e].y;
            int r,g,b;
            std::tie(r,g,b) = image->getPixel(x,y);
            int dx = abs(x - (*graph)[neighbours[0]].x);
            int dy = abs(y - (*graph)[neighbours[0]].y);
            auto newPixel = e;
            (*graph)[e].r = r;
            (*graph)[e].g = g;
            (*graph)[e].b = b;
            graph->ChangeVertexType(e,NODELABEL_P);
            (*graph)[e]._break = 0;
            for(int i=0;i<=3;i++)
            {
                auto newIEdge = graph->AddVertex(*(new Pixel((x+(*graph)[neighbours[i]].x)/2,(y+(*graph)[neighbours[i]].y)/2, NODELABEL_I)));
                (*graph)[newIEdge].breakLevel = (*graph)[e].breakLevel+1;
                graph->AddEdge(neighbours[i], newIEdge);
                graph->AddEdge(newPixel, newIEdge);
                graph->RemoveEdge(neighbours[i],e);
            }
            auto F11 = graph -> AddVertex(*(new Pixel(x, y - dy/2, NODELABEL_F)));
            graph -> AddEdge(newPixel, F11);
            auto F12 = graph -> AddVertex(*(new Pixel(x, y + dy/2, NODELABEL_F)));
            graph -> AddEdge(newPixel, F12);
            auto F21 = graph -> AddVertex(*(new Pixel(x - dx/2, y, NODELABEL_F)));
            graph -> AddEdge(newPixel, F21);
            auto F22 = graph -> AddVertex(*(new Pixel(x + dx/2, y, NODELABEL_F)));              
            graph -> AddEdge(newPixel, F22);
        }
    }
}

const double GetDistance(const int x1,const int x2,const int y1,const int y2)//TODO: Consider long longs (for very large images)?
{
    return (x1-x2)*(x1-x2)+(y1-y2)*(y1-y2);
}

const double GetDistance(const Pixel p1, const Pixel p2)
{
    return GetDistance(p1.x, p2.x, p1.y, p2.y);
}

P3::P3(
    std::shared_ptr<IGraph> graph, 
    vertex_descriptor BEdge,
    std::shared_ptr<Image> image
):
    graph(graph),
    BEdge(BEdge),
    image(image)
{}

std::unique_ptr<std::vector<P3>> P3::FindAllMatches(std::shared_ptr<CachedGraph> graph,
    std::shared_ptr<Image> image)
{
    auto result = std::make_unique<std::vector<P3>>();
    std::vector<vertex_descriptor> neighbours;
    std::vector<vertex_descriptor> leftPixels;
    std::vector<vertex_descriptor> rightPixels;
    for(vertex_descriptor BEdge : graph->getCacheIterator(NODELABEL_B))
    {
        if((*graph)[BEdge].label==NODELABEL_B)
        {
            leftPixels.clear();
            rightPixels.clear();
            neighbours.clear();
            neighbours = graph->GetAdjacentVertices(BEdge);
            if(neighbours.size()!=2)
            {
                std::cerr<<"incorrect B-edge containing "<<neighbours.size()<<" neighbours"<<std::endl;
                continue;
            }
            auto leftAdjacent = graph -> GetAdjacentVertices(neighbours[0]);
            auto rightAdjacent = graph -> GetAdjacentVertices(neighbours[1]);
            auto leftIEdges = where(leftAdjacent,[graph](const vertex_descriptor& v){return (*graph)[v].label==NODELABEL_I;});
            auto rightIEdges = where(rightAdjacent, [graph](const vertex_descriptor &v){return (*graph)[v].label==NODELABEL_I;});

            auto common2 = intersect(leftIEdges,rightIEdges);
            if(!common2.empty())
            {
                continue; //there is common IEdge between neighbour pixels, so this B cannot be used in P3 production
            }
            result -> emplace_back(graph, BEdge, image);
        }
    }
    return result;
}

void P3::Perform()
{
    std::vector<vertex_descriptor> neighbours;
    std::vector<vertex_descriptor> leftPixels;
    std::vector<vertex_descriptor> rightPixels;
    neighbours = graph -> GetAdjacentVertices(BEdge);
    auto leftAdjacent = graph -> GetAdjacentVertices(neighbours[0]);
    auto rightAdjacent = graph -> GetAdjacentVertices(neighbours[1]);
    auto leftIEdges = where(leftAdjacent,[this](const vertex_descriptor& v){return (*graph)[v].label==NODELABEL_I;});
    auto rightIEdges = where(rightAdjacent, [this](const vertex_descriptor &v){return (*graph)[v].label==NODELABEL_I;});
    for(const auto &edge : leftIEdges)
    {
        auto adjacentElems = graph -> GetAdjacentVertices(edge);
        leftPixels.insert(leftPixels.end(), adjacentElems.begin(), adjacentElems.end()); 
    }
    for(const auto &edge : rightIEdges)
    {
        auto adjacentElems = graph -> GetAdjacentVertices(edge);
        rightPixels.insert(rightPixels.end(), adjacentElems.begin(), adjacentElems.end()); 
    }
    auto common = intersect(leftPixels,rightPixels);
    if(common.empty())
    {
        std::cerr<<"P3: no common pixel found. Probably a bug."<<std::endl;
        throw;
    }
    vertex_descriptor centerPixel = *common.begin();
    auto adjacentVertices = graph -> GetAdjacentVertices(centerPixel);
    vertex_descriptor leftI = *where(leftIEdges, [adjacentVertices](vertex_descriptor v){return adjacentVertices | linq::contains(v);}).begin();
    vertex_descriptor rightI = *where(rightIEdges, [adjacentVertices](vertex_descriptor v){return adjacentVertices | linq::contains(v);}).begin();
    auto FEdges =  where(adjacentVertices, [this](vertex_descriptor v){return (*graph)[v].label==NODELABEL_F;});
    auto FEdgesOrdered = LINQ(from(v, FEdges) orderby(GetDistance((*graph)[v], (*graph)[BEdge])));
    auto FEdge =*(FEdgesOrdered.begin());
    vertex_descriptor leftPixel = neighbours[0];
    vertex_descriptor rightPixel = neighbours[1];
    int x=((*graph)[leftPixel].x + (*graph)[rightPixel].x)/2,
        y=((*graph)[leftPixel].y + (*graph)[rightPixel].y)/2;
    auto newPixel = graph -> AddVertex(*(new Pixel(x, y, NODELABEL_P)));
    int r,g,b;
    std::tie(r,g,b) = image -> getPixel(x,y);
    (*graph)[newPixel].r = r;
    (*graph)[newPixel].g = g;
    (*graph)[newPixel].b = b;
    graph -> AddEdge(newPixel, BEdge);
    graph -> AddEdge(newPixel, FEdge);

    auto temp = graph -> GetAdjacentVertices(leftPixel);
    auto leftIEdge =  *(temp | linq::intersect(graph -> GetAdjacentVertices(centerPixel))).begin();
    temp = graph -> GetAdjacentVertices(rightPixel);
    auto rightIEdge = *(temp | linq::intersect(graph -> GetAdjacentVertices(centerPixel))).begin();

    graph -> AddEdge(newPixel, leftIEdge);
    graph -> AddEdge(newPixel, rightIEdge);

    auto newBEdge = graph -> AddVertex(*(new Pixel(((*graph)[leftPixel].x+x)/2, ((*graph)[leftPixel].y+y)/2, NODELABEL_B)));
    (*graph)[BEdge].x = ((*graph)[rightPixel].x+x)/2;
    (*graph)[BEdge].y = ((*graph)[rightPixel].y+y)/2;
    
    graph -> AddEdge(leftPixel, newBEdge);
    graph -> AddEdge(newPixel, newBEdge);

    graph -> RemoveEdge(BEdge, leftPixel);
}

P4::P4(std::shared_ptr<IGraph> graph,
 vertex_descriptor FEdge, 
 std::shared_ptr<Image> image):
    graph(graph),
    FEdge(FEdge),
    image(image)
 {};

std::unique_ptr<std::vector<P4>> P4::FindAllMatches(std::shared_ptr<CachedGraph> graph,
            std::shared_ptr<Image> image)
{
    auto result = std::make_unique<std::vector<P4>>();
    auto interestingEdges = where(graph -> getCacheIterator(NODELABEL_F), [graph](vertex_descriptor v){return graph -> GetAdjacentVertices(v).size()==2;});

    std::vector<vertex_descriptor> toBeAdded;
    for (auto e: interestingEdges)
    {
        auto adjacentPixels = graph -> GetAdjacentVertices(e);
        bool isVertical = (*graph)[adjacentPixels[0]].x==(*graph)[adjacentPixels[1]].x;
        int bound1 = isVertical 
            ? std::min((*graph)[adjacentPixels[0]].y, (*graph)[adjacentPixels[1]].y) 
            : std::min((*graph)[adjacentPixels[0]].x, (*graph)[adjacentPixels[1]].x);
        int bound2 = isVertical 
            ? std::max((*graph)[adjacentPixels[0]].y, (*graph)[adjacentPixels[1]].y) 
            : std::max((*graph)[adjacentPixels[0]].x, (*graph)[adjacentPixels[1]].x); 
        auto isInVerticalBounds = [graph, bound1, bound2](vertex_descriptor v){return (*graph)[v].y>=bound1 and (*graph)[v].y<=bound2;};
        auto isInHorizontalBounds = [graph, bound1, bound2](vertex_descriptor v){return (*graph)[v].x>=bound1 and (*graph)[v].x<=bound2;};
        auto isInBounds = [isVertical, isInHorizontalBounds, isInVerticalBounds](vertex_descriptor v){return (isVertical and isInVerticalBounds(v))or(!isVertical and isInHorizontalBounds(v));};
        auto leftEdges = graph -> GetAdjacentVertices(adjacentPixels[0]);
        auto leftIEdges = where(leftEdges, [graph, &bound1, &bound2, &isVertical](const vertex_descriptor& v){return (*graph)[v].label==NODELABEL_I and ((isVertical and (*graph)[v].y>=bound1 and (*graph)[v].y<=bound2)or(!isVertical and (*graph)[v].x>=bound1 and (*graph)[v].x<=bound2));});
        auto rightEdges = graph -> GetAdjacentVertices(adjacentPixels[1]);
        auto rightIEdges = where(rightEdges, [graph, &bound1, &bound2, &isVertical](const vertex_descriptor& v){return (*graph)[v].label==NODELABEL_I and ((isVertical and (*graph)[v].y>=bound1 and (*graph)[v].y<=bound2)or(!isVertical and (*graph)[v].x>=bound1 and (*graph)[v].x<=bound2));});
        auto leftSecondPixels = GetAdjacentVertices(leftIEdges, *graph);
        auto rightSecondPixels = GetAdjacentVertices(rightIEdges, *graph);
        auto leftSecondPixelsInRange = where(leftSecondPixels, [graph, &bound1, &bound2, &isVertical](vertex_descriptor v){return (isVertical and (*graph)[v].y>=bound1 and (*graph)[v].y<=bound2)or(!isVertical and (*graph)[v].x>=bound1 and (*graph)[v].x<=bound2);});
        auto rightSecondPixelsInRange = where(rightSecondPixels, [graph, &bound1, &bound2, &isVertical](vertex_descriptor v){return (isVertical and (*graph)[v].y>=bound1 and (*graph)[v].y<=bound2)or(!isVertical and (*graph)[v].x>=bound1 and (*graph)[v].x<=bound2);});
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
        result -> emplace_back(graph, e, image);
    }
    return result;
}

void P4::Perform()
{
        auto adjacentPixels = graph -> GetAdjacentVertices(FEdge);
        bool isVertical = (*graph)[adjacentPixels[0]].x==(*graph)[adjacentPixels[1]].x;
        int bound1 = isVertical 
            ? std::min((*graph)[adjacentPixels[0]].y, (*graph)[adjacentPixels[1]].y) 
            : std::min((*graph)[adjacentPixels[0]].x, (*graph)[adjacentPixels[1]].x);
        int bound2 = isVertical 
            ? std::max((*graph)[adjacentPixels[0]].y, (*graph)[adjacentPixels[1]].y) 
            : std::max((*graph)[adjacentPixels[0]].x, (*graph)[adjacentPixels[1]].x); 
        
        auto leftEdges = graph -> GetAdjacentVertices(adjacentPixels[0]);
        auto leftIEdges = where(leftEdges, [this, &bound1, &bound2, &isVertical](const vertex_descriptor& v){return (*graph)[v].label==NODELABEL_I and ((isVertical and (*graph)[v].y>=bound1 and (*graph)[v].y<=bound2)or(!isVertical and (*graph)[v].x>=bound1 and (*graph)[v].x<=bound2));});
        auto rightEdges = graph -> GetAdjacentVertices(adjacentPixels[1]);
        auto rightIEdges = where(rightEdges, [this, &bound1, &bound2, &isVertical](const vertex_descriptor& v){return (*graph)[v].label==NODELABEL_I and ((isVertical and (*graph)[v].y>=bound1 and (*graph)[v].y<=bound2)or(!isVertical and (*graph)[v].x>=bound1 and (*graph)[v].x<=bound2));});
        auto leftSecondPixels = GetAdjacentVertices(leftIEdges, *graph);
        auto rightSecondPixels = GetAdjacentVertices(rightIEdges, *graph);
        auto leftSecondPixelsInRange = where(leftSecondPixels, [this, &bound1, &bound2, &isVertical](vertex_descriptor v){return (isVertical and (*graph)[v].y>=bound1 and (*graph)[v].y<=bound2)or(!isVertical and (*graph)[v].x>=bound1 and (*graph)[v].x<=bound2);});
        auto rightSecondPixelsInRange = where(rightSecondPixels, [this, &bound1, &bound2, &isVertical](vertex_descriptor v){return (isVertical and (*graph)[v].y>=bound1 and (*graph)[v].y<=bound2)or(!isVertical and (*graph)[v].x>=bound1 and (*graph)[v].x<=bound2);});
        auto commonPixels = intersect(leftSecondPixelsInRange,rightSecondPixelsInRange);
        auto commonPixelsAdjacent = GetAdjacentVertices(commonPixels, *graph);
        auto firstCommonPixel = (*graph)[*commonPixels.begin()];
        auto it = commonPixels.begin();
        auto second = ++it;
        auto lastCommonPixel = (*graph)[*second];
        auto commonPixelsAdjacentFEdges = where(commonPixelsAdjacent ,
            [this, firstCommonPixel, lastCommonPixel](vertex_descriptor v)
                {
                    Pixel p = (*graph)[v];
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
        std::tie(r,g,b) = image->getPixel(x,y);
        auto newPixel = graph -> AddVertex(*(new Pixel(x, y, r, g, b)));
        auto newFEdge = graph -> AddVertex(*(new Pixel((x+(*graph)[adjacentPixels[1]].x)/2, (y+(*graph)[adjacentPixels[1]].y)/2, NODELABEL_F)));
        for(auto FEdge : commonPixelsAdjacentFEdges)
            graph -> AddEdge(newPixel, FEdge);
        for(auto IEdge : leftIEdges)
            graph -> AddEdge(newPixel, IEdge);
        for(auto IEdge : rightIEdges)
            graph -> AddEdge(newPixel, IEdge);
        graph -> AddEdge(newPixel, FEdge);
        graph -> AddEdge(newPixel, newFEdge);
        graph -> AddEdge(adjacentPixels[1], newFEdge);

        graph -> RemoveEdge(adjacentPixels[1], FEdge);

        (*graph)[FEdge].x = (x+(*graph)[adjacentPixels[0]].x)/2;
        (*graph)[FEdge].y = (y+(*graph)[adjacentPixels[0]].y)/2;
    
}

std::map<vertex_descriptor,std::vector<vertex_descriptor>> vertexToNeighbours;
std::map<vertex_descriptor, double> IEdgeToError;

P5::P5(
    std::shared_ptr<IGraph> graph, 
    vertex_descriptor IEdge):
        graph(graph),
        IEdge(IEdge)
    {}

void P5::Perform()
{
    (*graph)[IEdge]._break = 1;
}

std::unique_ptr<std::vector<P5>> P5::FindAllMatches(std::shared_ptr<CachedGraph> graph,
    std::shared_ptr<Image> image,
    int channel,
    double epsilon)
{
    auto result = std::make_unique<std::vector<P5>>();
    double sumError = 0;
    double maxError = 0;
    vertexToNeighbours.clear();
    int width = image->width();
    int height = image->height();
    graph -> getCacheIterator(NODELABEL_I) | linq::select([graph](vertex_descriptor v){vertexToNeighbours[v] = graph -> GetAdjacentVertices(v); return v;});
    auto IEdges = where(graph -> getCacheIterator(NODELABEL_I), [](vertex_descriptor v){return vertexToNeighbours[v].size()==4;});
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
        double error = image->CompareWithInterpolation(minx, maxx, miny, maxy, channel);
        IEdgeToError[IEdge] = error;
        sumError += error;
        maxError = std::max(maxError, error);
    //std::cout<<"error: "<<error<<" "<<minx<<" "<<maxx<<" "<<miny<<" "<<maxy<<std::endl;
    }
    if(maxError > epsilon)
        for(auto IEdge : IEdges)
        {
            if(IEdgeToError[IEdge] > 0.33 * maxError && IEdgeToError[IEdge] > epsilon)
                {
                    result -> emplace_back(graph, IEdge);
                }
        }
}

std::queue<vertex_descriptor> toBeVisited;

template <typename T> int signum(T val) {
    return (T(0) < val) - (val < T(0));
}

bool haveCommonEdge(IGraph& graph, vertex_descriptor v1, vertex_descriptor v2, vertex_descriptor corner)
{
return !(true
&& signum(graph[v1].x - graph[corner].x) == signum(graph[corner].x - graph[v2].x)
&& signum(graph[v1].y - graph[corner].y) == signum(graph[corner].y - graph[v2].y) );

}

P6::P6(std::shared_ptr<IGraph> graph, 
    vertex_descriptor IEdge):
        graph(graph),
        IEdge(IEdge)
        {};

auto P6::PerformAndGetAdjacentEdges()
{
    std::vector<vertex_descriptor> adjacentEdges;
    auto adjacentVertices = graph -> GetAdjacentVertices(IEdge);
    for(auto v: adjacentVertices)
    {
        auto adjacentEdgesTemp = graph -> GetAdjacentVertices(v);
        auto adjacentEdgesTemp2 = adjacentEdgesTemp 
        | linq::where([this, v](vertex_descriptor IEdge)
        {
            return haveCommonEdge(*graph, IEdge, IEdge, v);
        });
        adjacentEdges.insert(adjacentEdges.end(), adjacentEdgesTemp2.begin(), adjacentEdgesTemp2.end());
    }
    auto adjacentIEdges = adjacentEdges 
        | linq::where([this](vertex_descriptor v){
            return true
                && (*graph)[v].label==NODELABEL_I 
                && (*graph)[v]._break==0 
                && (*graph)[v].breakLevel<(*graph)[IEdge].breakLevel;}
                );
    for(auto a : adjacentIEdges)
    {
        (*graph)[a]._break=1;
    }
    return adjacentIEdges;
}

std::unique_ptr<std::vector<P6>> P6::PerformAllMatches(std::shared_ptr<CachedGraph> graph)
{
    auto result = std::make_unique<std::vector<P6>>();
    for(auto IEdge: graph->getCacheIterator(NODELABEL_I))
    {
        (*graph)[IEdge].visited = 0;
    }
    for(auto IEdge: graph->getCacheIterator(NODELABEL_I))
    {
        if((*graph)[IEdge]._break and !(*graph)[IEdge].visited)
        {
            std::vector<vertex_descriptor> adjacentEdges;
            toBeVisited.push(IEdge);
            while(!toBeVisited.empty()) //BFS through IEdges with break==0
            {
                auto currentIEdge = toBeVisited.front();
                toBeVisited.pop();
                result->emplace_back(graph, currentIEdge);
                for(auto a : result->back().PerformAndGetAdjacentEdges())
                {
                    toBeVisited.push(a);
                }
            }
        }
    }
    return result;
}

void P6::Perform()
{
    PerformAndGetAdjacentEdges();
}


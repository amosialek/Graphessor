#include "P4.hpp"

std::vector<uint8_t> P4::Serialize()
{
    throw NotImplementedException();
}

P4::P4(std::shared_ptr<IGraph> graph,
 vertex_descriptor FEdge, 
 std::shared_ptr<Image> image):
    graph(graph),
    FEdge(FEdge),
    image(image)
 {}

std::unique_ptr<std::vector<P4>> P4::FindAllMatches(std::shared_ptr<CachedGraph> graph,
            std::shared_ptr<Image> image)
{
    auto result = std::make_unique<std::vector<P4>>();
    auto interestingEdges = where(graph -> GetCacheIterator(NODELABEL_F), [graph](vertex_descriptor v){return graph -> GetAdjacentVertices(v).size()==2;});

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
            if(commonPixels.size()!=2)
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
        //spdlog::debug("P4 {}", FEdge);    
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
        int x = isVertical ? ((*graph)[adjacentPixels[0]].x +(*graph)[adjacentPixels[1]].x)/2 : (firstCommonPixel.x + lastCommonPixel.x)/2;
        int y = isVertical ? (firstCommonPixel.y + lastCommonPixel.y)/2 : ((*graph)[adjacentPixels[0]].y +(*graph)[adjacentPixels[1]].y)/2;
        std::tie(x,y) = image -> GetNearestPixelCoords(x,y);
        spdlog::debug("P4 {} x={} y={} y1={} y2={}", FEdge,x,y,firstCommonPixel.y, lastCommonPixel.y);    
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

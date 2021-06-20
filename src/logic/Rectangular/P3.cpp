#include "P3.hpp"
#include "cassert"

std::vector<uint8_t> P3::Serialize()
{
    throw NotImplementedException();
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
    for(vertex_descriptor BEdge : graph->GetCacheIterator(NODELABEL_B))
    {
        if((*graph)[BEdge].label==NODELABEL_B)
        {
            leftPixels.clear();
            rightPixels.clear();
            neighbours.clear();
            neighbours = graph->GetAdjacentVertices(BEdge);
            assert(neighbours.size()==2 and "P3::FindAllMatches: incorrect B-edge containing wrong number of neighbours");
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
    //spdlog::debug("P3 {}", BEdge);    
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
    auto FEdges =  where(adjacentVertices, [this](vertex_descriptor v){return (*graph)[v].label==NODELABEL_F;});
    std::sort(FEdges.begin(),FEdges.end(),[this](vertex_descriptor& v1, vertex_descriptor& v2){return GetDistance((*graph)[v1], (*graph)[BEdge])<GetDistance((*graph)[v2], (*graph)[BEdge]);});
    auto FEdge =*(FEdges.begin());
    vertex_descriptor leftPixel = neighbours[0];
    vertex_descriptor rightPixel = neighbours[1];
    int x=((*graph)[leftPixel].x + (*graph)[rightPixel].x)/2,
        y=((*graph)[leftPixel].y + (*graph)[rightPixel].y)/2;
    //std::tie(x,y) = image->GetNearestPixelCoords(x,y);
    spdlog::debug("P3 {} x={} y={} y1={} y2={}", BEdge,x,y,(*graph)[leftPixel].y , (*graph)[rightPixel].y);    
    auto newPixel = graph -> AddVertex(*(new Pixel(x, y, NODELABEL_P)));
    int r,g,b;
    std::tie(r,g,b) = image -> getPixel(x,y);
    (*graph)[newPixel].r = r;
    (*graph)[newPixel].g = g;
    (*graph)[newPixel].b = b;
    graph -> AddEdge(newPixel, BEdge);
    graph -> AddEdge(newPixel, FEdge);

    auto temp = graph -> GetAdjacentVertices(leftPixel);
    std::vector<vertex_descriptor> IEdges;
    auto centerPixelAdjacencies = graph -> GetAdjacentVertices(centerPixel);
    auto leftIEdge =  (*intersect(temp, centerPixelAdjacencies).begin());
    temp = graph -> GetAdjacentVertices(rightPixel);
    auto rightIEdge = (*intersect(temp, centerPixelAdjacencies).begin());

    graph -> AddEdge(newPixel, leftIEdge);
    graph -> AddEdge(newPixel, rightIEdge);

    auto newBEdge = graph -> AddVertex(*(new Pixel(((*graph)[leftPixel].x+x)/2, ((*graph)[leftPixel].y+y)/2, NODELABEL_B)));
    (*graph)[BEdge].x = ((*graph)[rightPixel].x+x)/2;
    (*graph)[BEdge].y = ((*graph)[rightPixel].y+y)/2;
    
    graph -> AddEdge(leftPixel, newBEdge);
    graph -> AddEdge(newPixel, newBEdge);

    graph -> RemoveEdge(BEdge, leftPixel);
}

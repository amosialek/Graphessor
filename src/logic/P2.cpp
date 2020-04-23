#include "P2.hpp"

std::vector<uint8_t> P2::Serialize()
{
    throw NotImplementedException();
}

P2::P2(
    std::shared_ptr<IGraph> graph, 
    vertex_descriptor IEdge,
    std::shared_ptr<Image> image):
        graph(graph),
        IEdge(IEdge),
        image(image){}

std::unique_ptr<std::vector<P2>> P2::FindAllMatches(std::shared_ptr<CachedGraph> graph,
    std::shared_ptr<Image> image)
{
    auto result = std::make_unique<std::vector<P2>>();

    std::vector<vertex_descriptor> neighbours;
    std::vector<vertex_descriptor> IEdgesToBeAdded;
    std::set<vertex_descriptor> IEdgesToBeDeleted;
    for(vertex_descriptor e : graph -> GetCacheIterator(NODELABEL_I))
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
                result -> emplace_back(graph, e, image);
            }
        }
    }
    spdlog::debug("Found {} P2 Matches", result->size());  
    return result;
}

void P2::Perform()
{
    
    std::vector<vertex_descriptor> neighbours;
    auto e = IEdge;
    {
        if((*graph)[e].label==NODELABEL_I and (*graph)[e]._break==1)
        {
            neighbours = graph -> GetAdjacentVertices(e);
            int x = 0;
            int y = 0;
            for(auto p : neighbours)
            {
                x+=graph->operator[](p).x;
                y+=graph->operator[](p).y;
            }
            x /= 4;
            y /= 4;
            std::tie(x,y) = image->GetNearestPixelCoords(x,y);
            spdlog::debug("P2 {} x={} y={}", IEdge,x,y);    
            graph->operator[](e).x = x;
            graph->operator[](e).y = y;
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
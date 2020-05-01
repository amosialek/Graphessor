#include "RivaraP1.hpp"
#include <algorithm>
#include "RivaraUtils.hpp"

namespace Rivara
{
    RivaraP1::RivaraP1(std::shared_ptr<IGraph> graph,
            vertex_descriptor EEdge,
            vertex_descriptor TEdge,
            std::shared_ptr<Image> image) 
            :
                graph(graph), 
                EEdge(EEdge),
                TEdge(TEdge),
                image(image)
        {}

    void RivaraP1::Perform()
    {
        
        auto nodes = graph -> GetAdjacentVertices(EEdge);
        auto lastNodes = graph -> GetAdjacentVertices(TEdge);
        auto lastNode = where(lastNodes,[&nodes](vertex_descriptor v){return nodes[0]!=v and nodes[1]!=v;}).front();

        auto newTNode = GetNewTNode();
        auto newNNode = GetNewNNode(nodes);
        auto newMiddleENode = GetNewEMiddleNode(newNNode, lastNode);
        auto newENode = GetNewENode(newNNode, nodes);

        vertex_descriptor newNNodeVertex = graph -> AddVertex(newNNode);
        vertex_descriptor newENodeVertex = graph -> AddVertex(newENode);
        vertex_descriptor newTNodeVertex = graph -> AddVertex(newTNode);
        vertex_descriptor newMiddleENodeVertex = graph -> AddVertex(newMiddleENode);

        graph -> RemoveEdge(EEdge, nodes[0]);
        graph -> AddEdge(EEdge, newNNodeVertex);

        graph -> AddEdge(nodes[0], newENodeVertex);
        graph -> AddEdge(newNNodeVertex, newENodeVertex);

        graph -> RemoveEdge(TEdge, nodes[0]);
        graph -> AddEdge(TEdge, newNNodeVertex);

        graph -> AddEdge(nodes[0], newTNodeVertex);
        graph -> AddEdge(newNNodeVertex, newTNodeVertex);
        graph -> AddEdge(lastNode, newTNodeVertex);
        
        graph -> AddEdge(newMiddleENodeVertex, lastNode);
        graph -> AddEdge(newMiddleENodeVertex, newNNodeVertex);
    }

    Pixel RivaraP1::GetNewENode(Pixel& newNNode, std::vector<vertex_descriptor>& nodes)
    {
        Pixel newENode;
        newENode.attributes = std::make_shared<RivaraAttributes>();
        newENode.label = NODELABEL_E;
        newENode.attributes -> SetDouble(RIVARA_ATTRIBUTE_L, NL(newNNode, (*graph)[nodes[0]]));
        newENode.attributes -> SetBool(RIVARA_ATTRIBUTE_B, (*graph)[EEdge].attributes -> GetBool(RIVARA_ATTRIBUTE_B));
        return newENode;
    }

    Pixel RivaraP1::GetNewEMiddleNode(Pixel& newNNode, vertex_descriptor lastNode)
    {
        Pixel newMiddleENode;
        newMiddleENode.attributes = std::make_shared<RivaraAttributes>();
        newMiddleENode.label = NODELABEL_E;
        newMiddleENode.attributes -> SetDouble(RIVARA_ATTRIBUTE_L, NL(newNNode, (*graph)[lastNode]));
        newMiddleENode.attributes -> SetBool(RIVARA_ATTRIBUTE_B, false);
        return newMiddleENode;
    }

    Pixel RivaraP1::GetNewTNode()
    {
        Pixel newTNode;
        newTNode.attributes = std::make_shared<RivaraAttributes>();
        newTNode.label = NODELABEL_T;
        newTNode.attributes -> SetBool(RIVARA_ATTRIBUTE_R, false);
        return newTNode;
    }

    Pixel RivaraP1::GetNewNNode(std::vector<vertex_descriptor> nodes)
    {
        Pixel newNNode;
        newNNode.attributes = std::make_shared<RivaraAttributes>();
        newNNode.attributes -> SetDouble(RIVARA_ATTRIBUTE_X, ((*graph)[nodes[0]].attributes -> GetDouble(RIVARA_ATTRIBUTE_X) + (*graph)[nodes[1]].attributes -> GetDouble(RIVARA_ATTRIBUTE_X))/2);
        newNNode.attributes -> SetDouble(RIVARA_ATTRIBUTE_Y, ((*graph)[nodes[0]].attributes -> GetDouble(RIVARA_ATTRIBUTE_Y) + (*graph)[nodes[1]].attributes -> GetDouble(RIVARA_ATTRIBUTE_Y))/2); 
        std::tie(newNNode.r, newNNode.g, newNNode.b) = image->getPixel(newNNode.attributes -> GetDouble(RIVARA_ATTRIBUTE_X), newNNode.attributes -> GetDouble(RIVARA_ATTRIBUTE_Y));
        newNNode.label = NODELABEL_N;
        return newNNode;
    }

    std::unique_ptr<std::vector<RivaraP1>> RivaraP1::FindAllMatches(std::shared_ptr<CachedGraph> g, std::shared_ptr<Image> image) 
    {
        auto result = std::make_unique<std::vector<RivaraP1>>();
        auto triangles = g -> GetCacheIterator(NODELABEL_T);
        for(auto t : triangles)
        {
            if(!(*g)[t].attributes -> GetBool(RIVARA_ATTRIBUTE_R)) //if not marked
                continue;
            auto vertices = g -> GetAdjacentVertices(t);
            std::vector<vertex_descriptor> secondEEdges;
            for(auto v : vertices)
            {
                auto temp = g -> GetAdjacentVertices(v, NODELABEL_E);
                std::copy(temp.begin(), temp.end(), back_inserter(secondEEdges));
            } 
            std::sort(secondEEdges.begin(), secondEEdges.end());
            std::vector<vertex_descriptor> commonEEdges;
            for(size_t i = 0; i < secondEEdges.size() - 1; i++)
            {
                if(secondEEdges[i] == secondEEdges[i+1])
                {
                    commonEEdges.push_back(secondEEdges[i]);
                }
            }

            if(commonEEdges.size() == 3) //checks whether there is any hanging node (3 means none)
            {
                auto maybeBestEdge = RivaraP1::GetBestEdge(g, commonEEdges);
                if(maybeBestEdge.has_value())
                {
                    result->emplace_back(g, t, maybeBestEdge.value(), image);
                }
            }
        }
        return result;
    }

    std::optional<vertex_descriptor> RivaraP1::GetBestEdge(std::shared_ptr<CachedGraph> g, const std::vector<vertex_descriptor>& EEdges)
    {
        std::vector<Pixel> commonEEdgesPixels; 
        for(auto EEdge : EEdges)
        {
            commonEEdgesPixels.push_back((*g)[EEdge]);
        }
        std::sort(commonEEdgesPixels.begin(), commonEEdgesPixels.end(), EdgeComparator);
        double maxLength = commonEEdgesPixels.back().attributes->GetDouble(RIVARA_ATTRIBUTE_L);
        for(auto e : EEdges)
        {
            Pixel p = (*g)[e];
            if(p.attributes->GetBool(RIVARA_ATTRIBUTE_B) 
                and p.attributes->GetDouble(RIVARA_ATTRIBUTE_L)==maxLength)
            {    
                return e; //longest edge at the border
            }
        }
        for(auto e : EEdges)
        {
            Pixel p = (*g)[e];
            if(p.attributes->GetDouble(RIVARA_ATTRIBUTE_L)==maxLength)
            {
                auto commonNodes = g -> GetAdjacentVertices(e, NODELABEL_N);
                if(true
                    and !(*g)[commonNodes[0]].attributes->GetBool(RIVARA_ATTRIBUTE_HN)
                    and !(*g)[commonNodes[1]].attributes->GetBool(RIVARA_ATTRIBUTE_HN))
                {    
                    return e; //longest edge without hanging nodes
                }
            }
        }
        return {}; //suitable edge not found
    }

    std::vector<uint8_t> RivaraP1::Serialize()
    {
        throw NotImplementedException();
    }
}
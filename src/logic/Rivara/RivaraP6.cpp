#include "RivaraP6.hpp"
#include "RivaraUtils.hpp"

namespace Rivara
{
    RivaraP6::RivaraP6(std::shared_ptr<IGraph> graph,
            vertex_descriptor hangingNode,
            vertex_descriptor TEdge) 
            :
                graph(graph), 
                hangingNode(hangingNode),
                TEdge(TEdge),
        {}

    void RivaraP6::Perform()
    {
        auto vertices = graph -> GetAdjacentVertices(TEdge);

        auto hangingNodeEEdges = graph -> GetAdjacentVertices(hangingNode);;
        std::vector<vertex_descriptor> hangingNodeNeighbors;

        hangingNodeNeighbors.insert(hangingNodeNeighbors.end(), graph->GetAdjacentVertices(hangingNodeNeighbors[0]).begin(), graph->GetAdjacentVertices(hangingNodeNeighbors[0]).end());

        std::vector<vertex_descriptor> lastNodeSet;

        std::set_difference(
            vertices.begin(), 
            vertices.end(), 
            hangingNodeNeighbors.begin(), 
            hangingNodeNeighbors.end(),
            std::back_inserter(lastNodeSet));

        Pixel newEMiddleNode = GetNewEMiddleNode((*graph)[hangingNode], lastNodeSet[0]);
        Pixel newTNode = GetNewTNode();

        auto newTVertex = graph -> AddVertex(newTNode);
        auto newEMiddleVertex = graph -> AddVertex(newEMiddleNode);
        
        graph -> AddEdge(hangingNode, newEMiddleVertex);
        graph -> AddEdge(lastNodeSet[0], newEMiddleVertex);

        graph -> RemoveEdge(TEdge, hangingNodeNeighbors[0]);
        graph -> AddEdge(TEdge, hangingNode);

        graph -> AddEdge(newTVertex, hangingNode);
        graph -> AddEdge(newTVertex, lastNodeSet[0]);
        graph -> AddEdge(newTVertex, hangingNodeNeighbors[0]);

        (*graph)[TEdge].attributes->SetBool(RIVARA_ATTRIBUTE_R, false);
    }

    std::unique_ptr<std::vector<RivaraP6>> RivaraP6::FindAllMatches(std::shared_ptr<CachedGraph> g, std::shared_ptr<Image> image)
    {
       std::unique_ptr<std::vector<RivaraP6>> result = std::make_unique<std::vector<RivaraP6>>();
        auto triangles = g -> GetCacheIterator(NODELABEL_T);
        for(auto triangle : triangles)
        {
            auto vertices = g -> GetAdjacentVertices(triangle);
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
            if(commonEEdges.size()==0)
            {
                std::vector<vertex_descriptor> hangingNodes;
                for(auto e : secondEEdges)
                {
                    auto tmp = g->GetAdjacentVertices(e);
                    hangingNodes.insert(hangingNodes.end(),tmp.begin(),tmp.end());
                }
                double hangingNodePoint1X = ((*g)[vertices[0]].attributes->GetDouble(RIVARA_ATTRIBUTE_X) + (*g)[vertices[1]].attributes->GetDouble(RIVARA_ATTRIBUTE_X))/2;
                double hangingNodePoint1Y = ((*g)[vertices[0]].attributes->GetDouble(RIVARA_ATTRIBUTE_Y) + (*g)[vertices[1]].attributes->GetDouble(RIVARA_ATTRIBUTE_Y))/2;
                double hangingNodePoint2X = ((*g)[vertices[1]].attributes->GetDouble(RIVARA_ATTRIBUTE_X) + (*g)[vertices[2]].attributes->GetDouble(RIVARA_ATTRIBUTE_X))/2;
                double hangingNodePoint2Y = ((*g)[vertices[1]].attributes->GetDouble(RIVARA_ATTRIBUTE_Y) + (*g)[vertices[2]].attributes->GetDouble(RIVARA_ATTRIBUTE_Y))/2;
                double hangingNodePoint3X = ((*g)[vertices[2]].attributes->GetDouble(RIVARA_ATTRIBUTE_X) + (*g)[vertices[0]].attributes->GetDouble(RIVARA_ATTRIBUTE_X))/2;
                double hangingNodePoint3Y = ((*g)[vertices[2]].attributes->GetDouble(RIVARA_ATTRIBUTE_Y) + (*g)[vertices[0]].attributes->GetDouble(RIVARA_ATTRIBUTE_Y))/2;
                hangingNodes = where(hangingNodes, 
                [
                    &g,
                    &vertices,
                    hangingNodePoint1X,
                    hangingNodePoint1Y,
                    hangingNodePoint2X,
                    hangingNodePoint2Y,
                    hangingNodePoint3X,
                    hangingNodePoint3Y]
                    (vertex_descriptor v){
                        auto vNode = (*g)[v];
                        return v!=vertices[0]and v!=vertices[1] and v!=vertices[2]
                        and ((abs(vNode.attributes->GetDouble(RIVARA_ATTRIBUTE_X)-hangingNodePoint1X)<0.1
                        and abs(vNode.attributes->GetDouble(RIVARA_ATTRIBUTE_Y)-hangingNodePoint1Y)<0.1)
                        or (abs(vNode.attributes->GetDouble(RIVARA_ATTRIBUTE_X)-hangingNodePoint2X)<0.1
                        and abs(vNode.attributes->GetDouble(RIVARA_ATTRIBUTE_Y)-hangingNodePoint2Y)<0.1)
                        or (abs(vNode.attributes->GetDouble(RIVARA_ATTRIBUTE_X)-hangingNodePoint3X)<0.1
                        and abs(vNode.attributes->GetDouble(RIVARA_ATTRIBUTE_Y)-hangingNodePoint3Y)<0.1))
                        ;});
                auto hangingNode0EEdges = g -> GetAdjacentVertices(hangingNodes[0]);
                auto hangingNode1EEdges = g -> GetAdjacentVertices(hangingNodes[1]);
                auto hangingNode2EEdges = g -> GetAdjacentVertices(hangingNodes[2]);

                double L4 = (*g)[hangingNode0EEdges[0]].attributes -> GetDouble(RIVARA_ATTRIBUTE_L);
                double L5 = (*g)[hangingNode0EEdges[1]].attributes -> GetDouble(RIVARA_ATTRIBUTE_L);

                double L6 = (*g)[hangingNode1EEdges[0]].attributes -> GetDouble(RIVARA_ATTRIBUTE_L);
                double L7 = (*g)[hangingNode1EEdges[1]].attributes -> GetDouble(RIVARA_ATTRIBUTE_L);

                double L8 = (*g)[hangingNode2EEdges[0]].attributes -> GetDouble(RIVARA_ATTRIBUTE_L);
                double L9 = (*g)[hangingNode2EEdges[1]].attributes -> GetDouble(RIVARA_ATTRIBUTE_L);

                if((L4+L5)>=(L6+L7) and (L4+L5)>=(L8+L9))
                {
                    result->emplace_back(g,hangingNodes[0], triangle);
                }
                else if((L4+L5)>=(L6+L7) and (L4+L5)>=(L8+L9))
                {
                    result->emplace_back(g,hangingNodes[1], triangle);
                }
                else 
                {
                    result->emplace_back(g,hangingNodes[2], triangle);   
                }
            }
        }
        return result; 
    }
    
    bool RivaraP6::EdgeHasNoHangingNodes(IGraph& graph, vertex_descriptor v)
    {
        auto Nnodes = graph.GetAdjacentVertices(v);
        return ( !(graph)[Nnodes[0]].attributes->GetBool(RIVARA_ATTRIBUTE_HN) 
        and  !(graph)[Nnodes[1]].attributes->GetBool(RIVARA_ATTRIBUTE_HN));
    }

        Pixel RivaraP6::GetNewENode(Pixel& newNNode, std::vector<vertex_descriptor>& nodes)
    {
        Pixel newENode;
        newENode.attributes = std::make_shared<RivaraAttributes>();
        newENode.label = NODELABEL_E;
        newENode.attributes -> SetDouble(RIVARA_ATTRIBUTE_L, NL(newNNode, (*graph)[nodes[0]]));
        newENode.attributes -> SetBool(RIVARA_ATTRIBUTE_B, (*graph)[EEdge].attributes -> GetBool(RIVARA_ATTRIBUTE_B));
        return newENode;
    }

    Pixel RivaraP6::GetNewEMiddleNode(Pixel& newNNode, vertex_descriptor lastNode)
    {
        Pixel newMiddleENode;
        newMiddleENode.attributes = std::make_shared<RivaraAttributes>();
        newMiddleENode.label = NODELABEL_E;
        newMiddleENode.attributes -> SetDouble(RIVARA_ATTRIBUTE_L, NL(newNNode, (*graph)[lastNode]));
        newMiddleENode.attributes -> SetBool(RIVARA_ATTRIBUTE_B, false);
        return newMiddleENode;
    }

    Pixel RivaraP6::GetNewTNode()
    {
        Pixel newTNode;
        newTNode.attributes = std::make_shared<RivaraAttributes>();
        newTNode.label = NODELABEL_T;
        newTNode.attributes -> SetBool(RIVARA_ATTRIBUTE_R, false);
        return newTNode;
    }

    Pixel RivaraP6::GetNewNNode(std::vector<vertex_descriptor> nodes)
    {
        Pixel newNNode;
        newNNode.attributes = std::make_shared<RivaraAttributes>();
        newNNode.attributes -> SetDouble(RIVARA_ATTRIBUTE_X, ((*graph)[nodes[0]].attributes -> GetDouble(RIVARA_ATTRIBUTE_X) + (*graph)[nodes[1]].attributes -> GetDouble(RIVARA_ATTRIBUTE_X))/2);
        newNNode.attributes -> SetDouble(RIVARA_ATTRIBUTE_Y, ((*graph)[nodes[0]].attributes -> GetDouble(RIVARA_ATTRIBUTE_Y) + (*graph)[nodes[1]].attributes -> GetDouble(RIVARA_ATTRIBUTE_Y))/2); 
        std::tie(newNNode.r, newNNode.g, newNNode.b) = image->getPixel(newNNode.attributes -> GetDouble(RIVARA_ATTRIBUTE_X), newNNode.attributes -> GetDouble(RIVARA_ATTRIBUTE_Y));
        newNNode.label = NODELABEL_N;
        return newNNode;
    }
}
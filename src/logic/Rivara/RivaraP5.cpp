#include "RivaraP5.hpp"
#include "RivaraUtils.hpp"

namespace Rivara
{
    RivaraP5::RivaraP5(std::shared_ptr<IGraph> graph,
            vertex_descriptor NNode,
            vertex_descriptor EEdge,
            vertex_descriptor TEdge,
            std::shared_ptr<Image> image) 
            :
                graph(graph), 
                NNode(NNode),
                EEdge(EEdge),
                TEdge(TEdge),
                image(image)
        {}

    void RivaraP5::Perform()
    {
        auto nodes = graph -> GetAdjacentVertices(EEdge);

        auto vertices = graph -> GetAdjacentVertices(TEdge);

        Pixel newNNode = GetNewNNode(nodes);
        Pixel newEMiddleNode = GetNewEMiddleNode(newNNode, NNode);
        Pixel newTNode = GetNewTNode(); 
        Pixel newENode = GetNewENode(newNNode, nodes);

        auto newTVertex = graph -> AddVertex(newTNode);
        auto newEVertex = graph -> AddVertex(newENode);
        auto newNVertex = graph -> AddVertex(newNNode);
        auto newEMiddleVertex = graph -> AddVertex(newEMiddleNode);
        
        graph -> AddEdge(NNode, newEMiddleVertex);
        graph -> AddEdge(newNVertex, newEMiddleVertex);

        graph -> AddEdge(nodes[0], newEVertex);
        graph -> AddEdge(newNVertex, newEVertex);

        graph -> RemoveEdge(TEdge, nodes[0]);
        graph -> AddEdge(TEdge, newNVertex);

        graph -> RemoveEdge(EEdge, nodes[0]);
        graph -> AddEdge(EEdge, newNVertex);

        graph -> AddEdge(newTVertex, NNode);
        graph -> AddEdge(newTVertex, newNVertex);
        graph -> AddEdge(newTVertex, nodes[0]);

        (*graph)[EEdge].attributes->SetDouble(RIVARA_ATTRIBUTE_L, NL(newNNode, (*graph)[NNode]));
        (*graph)[TEdge].attributes->SetBool(RIVARA_ATTRIBUTE_R, false);
    }

    std::unique_ptr<std::vector<RivaraP5>> RivaraP5::FindAllMatches(std::shared_ptr<CachedGraph> g, std::shared_ptr<Image> image)
    {
       std::unique_ptr<std::vector<RivaraP5>> result = std::make_unique<std::vector<RivaraP5>>();
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
            if(commonEEdges.size()==1)
            {
                auto notBrokenEdgeNodes = g -> GetAdjacentVertices(commonEEdges[0]);
                std::vector<vertex_descriptor> lastNodeSet;
                std::set_difference(
                    vertices.begin(),
                    vertices.end(), 
                    notBrokenEdgeNodes.begin(), 
                    notBrokenEdgeNodes.end(),
                    std::back_inserter(lastNodeSet));
                auto EEdges = g -> GetAdjacentVertices(lastNodeSet[0]);
                std::vector<vertex_descriptor> hangingNodes;
                for(auto e: EEdges)
                {
                    auto nodes = g -> GetAdjacentVertices(e);
                    hangingNodes.insert(hangingNodes.end(), nodes.begin(), nodes.end());
                }
                double hangingNodePoint1X = ((*g)[notBrokenEdgeNodes[0]].attributes->GetDouble(RIVARA_ATTRIBUTE_X) + (*g)[lastNodeSet[0]].attributes->GetDouble(RIVARA_ATTRIBUTE_X))/2;
                double hangingNodePoint1Y = ((*g)[notBrokenEdgeNodes[0]].attributes->GetDouble(RIVARA_ATTRIBUTE_Y) + (*g)[lastNodeSet[0]].attributes->GetDouble(RIVARA_ATTRIBUTE_Y))/2;
                double hangingNodePoint2X = ((*g)[notBrokenEdgeNodes[1]].attributes->GetDouble(RIVARA_ATTRIBUTE_X) + (*g)[lastNodeSet[0]].attributes->GetDouble(RIVARA_ATTRIBUTE_X))/2;
                double hangingNodePoint2Y = ((*g)[notBrokenEdgeNodes[1]].attributes->GetDouble(RIVARA_ATTRIBUTE_Y) + (*g)[lastNodeSet[0]].attributes->GetDouble(RIVARA_ATTRIBUTE_Y))/2;
                hangingNodes = where(hangingNodes, 
                [
                    &lastNodeSet,
                    &g,
                    hangingNodePoint1X,
                    hangingNodePoint1Y,
                    hangingNodePoint2X,
                    hangingNodePoint2Y]
                    (vertex_descriptor v){
                        auto vNode = (*g)[v];
                        return v!=lastNodeSet[0]
                        and ((abs(vNode.attributes->GetDouble(RIVARA_ATTRIBUTE_X)-hangingNodePoint1X)<0.1
                        and abs(vNode.attributes->GetDouble(RIVARA_ATTRIBUTE_Y)-hangingNodePoint1Y)<0.1)
                        or (abs(vNode.attributes->GetDouble(RIVARA_ATTRIBUTE_X)-hangingNodePoint2X)<0.1
                        and abs(vNode.attributes->GetDouble(RIVARA_ATTRIBUTE_Y)-hangingNodePoint2Y)<0.1))
                        ;});
                auto hangingNode0EEdges = g -> GetAdjacentVertices(hangingNodes[0]);
                auto hangingNode1EEdges = g -> GetAdjacentVertices(hangingNodes[1]);

                double L4 = (*g)[hangingNode0EEdges[0]].attributes -> GetDouble(RIVARA_ATTRIBUTE_L);
                double L5 = (*g)[hangingNode0EEdges[1]].attributes -> GetDouble(RIVARA_ATTRIBUTE_L);

                double L6 = (*g)[hangingNode1EEdges[0]].attributes -> GetDouble(RIVARA_ATTRIBUTE_L);
                double L7 = (*g)[hangingNode1EEdges[1]].attributes -> GetDouble(RIVARA_ATTRIBUTE_L);

                double L3 = (*g)[commonEEdges[0]].attributes -> GetDouble(RIVARA_ATTRIBUTE_L);

                if(L3>(L6+L7) and (L4+L5)<L3 and EdgeHasNoHangingNodes(*g, commonEEdges[0]))
                {
                    result -> emplace_back(g, lastNodeSet[0], commonEEdges[0], triangle, image);
                }
            }
        }
        return result; 
    }
    
    bool RivaraP5::EdgeHasNoHangingNodes(IGraph& graph, vertex_descriptor v)
    {
        auto Nnodes = graph.GetAdjacentVertices(v);
        return ( !(graph)[Nnodes[0]].attributes->GetBool(RIVARA_ATTRIBUTE_HN) 
        and  !(graph)[Nnodes[1]].attributes->GetBool(RIVARA_ATTRIBUTE_HN));
    }

        Pixel RivaraP5::GetNewENode(Pixel& newNNode, std::vector<vertex_descriptor>& nodes)
    {
        Pixel newENode;
        newENode.attributes = std::make_shared<RivaraAttributes>();
        newENode.label = NODELABEL_E;
        newENode.attributes -> SetDouble(RIVARA_ATTRIBUTE_L, NL(newNNode, (*graph)[nodes[0]]));
        newENode.attributes -> SetBool(RIVARA_ATTRIBUTE_B, (*graph)[EEdge].attributes -> GetBool(RIVARA_ATTRIBUTE_B));
        return newENode;
    }

    Pixel RivaraP5::GetNewEMiddleNode(Pixel& newNNode, vertex_descriptor lastNode)
    {
        Pixel newMiddleENode;
        newMiddleENode.attributes = std::make_shared<RivaraAttributes>();
        newMiddleENode.label = NODELABEL_E;
        newMiddleENode.attributes -> SetDouble(RIVARA_ATTRIBUTE_L, NL(newNNode, (*graph)[lastNode]));
        newMiddleENode.attributes -> SetBool(RIVARA_ATTRIBUTE_B, false);
        return newMiddleENode;
    }

    Pixel RivaraP5::GetNewTNode()
    {
        Pixel newTNode;
        newTNode.attributes = std::make_shared<RivaraAttributes>();
        newTNode.label = NODELABEL_T;
        newTNode.attributes -> SetBool(RIVARA_ATTRIBUTE_R, false);
        return newTNode;
    }

    Pixel RivaraP5::GetNewNNode(std::vector<vertex_descriptor> nodes)
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
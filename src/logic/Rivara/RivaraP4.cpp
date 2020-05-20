#include "RivaraP4.hpp"

namespace Rivara
{
    RivaraP4::RivaraP4(std::shared_ptr<IGraph> graph,
            vertex_descriptor NNode,
            vertex_descriptor TEdge) 
            :
                graph(graph), 
                NNode(NNode),
                TEdge(TEdge)
        {}

    void RivaraP4::Perform()
    {
        spdlog::debug("Rivara P4");
        auto EEdges = graph -> GetAdjacentVertices(NNode, NODELABEL_E); 
        std::vector<vertex_descriptor> nodes;
        for(auto e:EEdges)
        {
            auto tmp = where(graph -> GetAdjacentVertices(e), [this](vertex_descriptor v){return v!=NNode;});
            nodes.insert(nodes.end(), tmp.begin(), tmp.end());
        }

        auto vertices = graph -> GetAdjacentVertices(TEdge);

        std::vector<vertex_descriptor> lastNodeSet;

        Rivara::RelativeComplementOfBInA(vertices, nodes, lastNodeSet);

        Pixel newEMiddleNode = GetNewEMiddleNode(graph, (*graph)[NNode], lastNodeSet[0]);
        Pixel newTNode = GetNewTNode();

        auto newTVertex = graph -> AddVertex(newTNode);
        auto newEVertex = graph -> AddVertex(newEMiddleNode);

        graph -> AddEdge(lastNodeSet[0], newEVertex);
        graph -> AddEdge(NNode, newEVertex);

        graph -> RemoveEdge(TEdge, nodes[0]);
        graph -> AddEdge(TEdge, NNode);

        graph -> AddEdge(newTVertex, NNode);
        graph -> AddEdge(newTVertex, lastNodeSet[0]);
        graph -> AddEdge(newTVertex, nodes[0]);

        (*graph)[TEdge].x = ((*graph)[nodes[1]].x+(*graph)[lastNodeSet[0]].x+(*graph)[NNode].x)/3;
        (*graph)[TEdge].y = ((*graph)[nodes[1]].y+(*graph)[lastNodeSet[0]].y+(*graph)[NNode].y)/3;
        (*graph)[newTVertex].x = ((*graph)[nodes[0]].x+(*graph)[lastNodeSet[0]].x+(*graph)[NNode].x)/3;
        (*graph)[newTVertex].y = ((*graph)[nodes[0]].y+(*graph)[lastNodeSet[0]].y+(*graph)[NNode].y)/3;


        (*graph)[NNode].attributes -> SetBool(RIVARA_ATTRIBUTE_HN, false);
        (*graph)[TEdge].attributes->SetBool(RIVARA_ATTRIBUTE_R, false);
        (*graph)[TEdge].error = (*graph)[newTVertex].error = -1;
    }

    std::unique_ptr<std::vector<RivaraP4>> RivaraP4::FindAllMatches(std::shared_ptr<CachedGraph> g)
    {
       std::unique_ptr<std::vector<RivaraP4>> result = std::make_unique<std::vector<RivaraP4>>();
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
                Rivara::RelativeComplementOfBInA(
                    vertices, notBrokenEdgeNodes, lastNodeSet);
                auto EEdges = g -> GetAdjacentVertices(lastNodeSet[0], NODELABEL_E);
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
                        and (abs(vNode.attributes->GetDouble(RIVARA_ATTRIBUTE_X)-hangingNodePoint1X)<0.1
                        and abs(vNode.attributes->GetDouble(RIVARA_ATTRIBUTE_Y)-hangingNodePoint1Y)<0.1)
                        or (abs(vNode.attributes->GetDouble(RIVARA_ATTRIBUTE_X)-hangingNodePoint2X)<0.1
                        and abs(vNode.attributes->GetDouble(RIVARA_ATTRIBUTE_Y)-hangingNodePoint2Y)<0.1)
                        ;});
                auto hangingNode0EEdges = g -> GetAdjacentVertices(hangingNodes[0], NODELABEL_E);
                auto hangingNode1EEdges = g -> GetAdjacentVertices(hangingNodes[1], NODELABEL_E);

                std::set<vertex_descriptor> verticesEEdges;
                verticesEEdges.insert(EEdges.begin(), EEdges.end());
                auto leftEEdges = g-> GetAdjacentVertices(notBrokenEdgeNodes[0], NODELABEL_E);
                auto rightEEdges = g-> GetAdjacentVertices(notBrokenEdgeNodes[0], NODELABEL_E);
                verticesEEdges.insert(leftEEdges.begin(), leftEEdges.end());
                verticesEEdges.insert(rightEEdges.begin(), rightEEdges.end());
                std::vector<vertex_descriptor> hangingNode0EEdgesInThisTriangle;
                std::vector<vertex_descriptor> hangingNode1EEdgesInThisTriangle;
                Intersection(hangingNode0EEdges, verticesEEdges, hangingNode0EEdgesInThisTriangle);
                Intersection(hangingNode1EEdges, verticesEEdges, hangingNode1EEdgesInThisTriangle);
                
                double L4 = (*g)[hangingNode0EEdgesInThisTriangle[0]].attributes -> GetDouble(RIVARA_ATTRIBUTE_L);
                double L5 = (*g)[hangingNode0EEdgesInThisTriangle[1]].attributes -> GetDouble(RIVARA_ATTRIBUTE_L);

                double L6 = (*g)[hangingNode1EEdgesInThisTriangle[0]].attributes -> GetDouble(RIVARA_ATTRIBUTE_L);
                double L7 = (*g)[hangingNode1EEdgesInThisTriangle[1]].attributes -> GetDouble(RIVARA_ATTRIBUTE_L);

                double L3 = (*g)[commonEEdges[0]].attributes -> GetDouble(RIVARA_ATTRIBUTE_L);

                if((L4+L5)>=(L6+L7) and (L4+L5)>=L3)
                {
                    result -> emplace_back(g, hangingNodes[0], triangle);
                }
                else if((L6+L7)>=(L4+L5) and (L6+L7)>=L3)
                {
                    result -> emplace_back(g, hangingNodes[1], triangle);
                }
            }
        }
        return result; 
    }
}
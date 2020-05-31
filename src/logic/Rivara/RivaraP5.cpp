#include "RivaraP5.hpp"

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
        spdlog::debug("Rivara P5");
        auto nodes = graph -> GetAdjacentVertices(EEdge);

        auto vertices = graph -> GetAdjacentVertices(TEdge);

        Pixel newNNode = GetNewNNode(graph, nodes, image);
        Pixel newEMiddleNode = GetNewEMiddleNode(graph, newNNode, NNode);
        Pixel newTNode = GetNewTNode(); 
        Pixel newENode = GetNewENode(graph, EEdge);

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

        (*graph)[EEdge].x = ((*graph)[nodes[1]].x+newNNode.x)/2;
        (*graph)[EEdge].y = ((*graph)[nodes[1]].y+newNNode.y)/2;
        (*graph)[newEVertex].x = ((*graph)[nodes[0]].x+newNNode.x)/2;
        (*graph)[newEVertex].y = ((*graph)[nodes[0]].y+newNNode.y)/2;
        (*graph)[TEdge].x = ((*graph)[nodes[1]].x+newNNode.x+(*graph)[NNode].x)/3;
        (*graph)[TEdge].y = ((*graph)[nodes[1]].y+newNNode.y+(*graph)[NNode].y)/3;
        (*graph)[newTVertex].x = ((*graph)[nodes[0]].x+newNNode.x+(*graph)[NNode].x)/3;
        (*graph)[newTVertex].y = ((*graph)[nodes[0]].y+newNNode.y+(*graph)[NNode].y)/3;

        (*graph)[EEdge].attributes->SetDouble(RIVARA_ATTRIBUTE_L, NL(newNNode, (*graph)[NNode]));
        (*graph)[TEdge].attributes->SetBool(RIVARA_ATTRIBUTE_R, false);
        (*graph)[TEdge].error = (*graph)[newTVertex].error = -1;
    }

    std::unique_ptr<std::vector<RivaraP5>> RivaraP5::FindAllMatches(std::shared_ptr<RivaraCachedGraph> g, std::shared_ptr<Image> image)
    {
       std::unique_ptr<std::vector<RivaraP5>> result = std::make_unique<std::vector<RivaraP5>>();
        //auto triangles = g -> GetCacheIterator(NODELABEL_T);
        auto triangles = TrianglesWhichMightHaveHangingNode(*g);
        for(auto triangle : triangles)
        {
            std::vector<vertex_descriptor> secondEEdges;
            std::vector<vertex_descriptor> commonEEdges;
            std::vector<vertex_descriptor> vertices;
            GetCommonEEdges(g, triangle, secondEEdges, commonEEdges, vertices);
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
                        and ((abs(vNode.attributes->GetDouble(RIVARA_ATTRIBUTE_X)-hangingNodePoint1X)<0.1
                        and abs(vNode.attributes->GetDouble(RIVARA_ATTRIBUTE_Y)-hangingNodePoint1Y)<0.1)
                        or (abs(vNode.attributes->GetDouble(RIVARA_ATTRIBUTE_X)-hangingNodePoint2X)<0.1
                        and abs(vNode.attributes->GetDouble(RIVARA_ATTRIBUTE_Y)-hangingNodePoint2Y)<0.1))
                        ;});
                auto hangingNode0EEdges = g -> GetAdjacentVertices(hangingNodes[0]);
                auto hangingNode1EEdges = g -> GetAdjacentVertices(hangingNodes[1]);

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

                if(L3>(L6+L7) and (L4+L5)<L3 and EdgeHasNoHangingNodes(*g, commonEEdges[0]))
                {
                    result -> emplace_back(g, lastNodeSet[0], commonEEdges[0], triangle, image);
                }
            }
        }
        return result; 
    }
    


}
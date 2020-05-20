#include "RivaraP6.hpp"

namespace Rivara
{
    RivaraP6::RivaraP6(std::shared_ptr<IGraph> graph,
            vertex_descriptor hangingNode,
            vertex_descriptor TEdge) 
            :
                graph(graph), 
                hangingNode(hangingNode),
                TEdge(TEdge)
        {}

    void RivaraP6::Perform()
    {
        spdlog::debug("Rivara P6");
        auto vertices = graph -> GetAdjacentVertices(TEdge);

        auto hangingNodeEEdges = graph -> GetAdjacentVertices(hangingNode);;
        std::vector<vertex_descriptor> hangingNodeNeighbors;
        auto xd = graph->GetAdjacentVertices(hangingNodeEEdges[0]);
        hangingNodeNeighbors.insert(hangingNodeNeighbors.end(), xd.begin(), xd.end());
        graph->GetAdjacentVertices(hangingNodeEEdges[1]);
        hangingNodeNeighbors.insert(hangingNodeNeighbors.end(), xd.begin(), xd.end());

        std::vector<vertex_descriptor> lastNodeSet;

        Rivara::RelativeComplementOfBInA(
            vertices, 
            hangingNodeNeighbors,
            lastNodeSet);

        Pixel newEMiddleNode = GetNewEMiddleNode(graph, (*graph)[hangingNode], lastNodeSet[0]);
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

        (*graph)[TEdge].x = ((*graph)[hangingNodeNeighbors[1]].x+(*graph)[hangingNode].x+(*graph)[lastNodeSet[0]].x)/3;
        (*graph)[TEdge].y = ((*graph)[hangingNodeNeighbors[1]].y+(*graph)[hangingNode].y+(*graph)[lastNodeSet[0]].y)/3;
        (*graph)[newTVertex].x = ((*graph)[hangingNodeNeighbors[0]].x+(*graph)[hangingNode].x+(*graph)[lastNodeSet[0]].x)/3;
        (*graph)[newTVertex].y = ((*graph)[hangingNodeNeighbors[0]].y+(*graph)[hangingNode].y+(*graph)[lastNodeSet[0]].y)/3;

        (*graph)[TEdge].attributes->SetBool(RIVARA_ATTRIBUTE_R, false);

        (*graph)[TEdge].error = (*graph)[newTVertex].error = -1;
    }

    std::unique_ptr<std::vector<RivaraP6>> RivaraP6::FindAllMatches(std::shared_ptr<CachedGraph> g)
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
    

}
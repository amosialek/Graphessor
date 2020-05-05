#include "RivaraP2.hpp"

namespace Rivara
{
    RivaraP2::RivaraP2(std::shared_ptr<IGraph> graph,
            vertex_descriptor hangingNode,
            vertex_descriptor TEdge) 
            :
                graph(graph), 
                hangingNode(hangingNode),
                TEdge(TEdge)
        {}

    void RivaraP2::Perform()
    {
        auto EEdges = graph -> GetAdjacentVertices(hangingNode);
        std::vector<vertex_descriptor> HNNeighbors;
        for(auto e : EEdges)
        {
            auto nodes = where(graph->GetAdjacentVertices(e), [this](vertex_descriptor v){return v!=hangingNode;});
            HNNeighbors.insert(HNNeighbors.end(), EEdges.begin(), EEdges.end());
        }
        vertex_descriptor lastNode = where(graph->GetAdjacentVertices(TEdge), [&HNNeighbors](vertex_descriptor v){return v!=HNNeighbors[0] and v!=HNNeighbors[1];})[0];

        Pixel newENode = GetNewEMiddleNode(graph, (*graph)[hangingNode], lastNode);
        Pixel newTNode = GetNewTNode();
        vertex_descriptor newENodeVertex = graph -> AddVertex(newENode);
        vertex_descriptor newTNodeVertex = graph -> AddVertex(newTNode);

        (*graph)[hangingNode].attributes -> SetBool(RIVARA_ATTRIBUTE_HN, false);
        (*graph)[TEdge].attributes -> SetBool(RIVARA_ATTRIBUTE_R, false);

        //oldTEdge neighbors
        graph -> RemoveEdge(HNNeighbors[0], TEdge);
        graph -> AddEdge(hangingNode, TEdge);

        //newEEdge neighbors
        graph -> AddEdge(hangingNode, newENodeVertex);
        graph -> AddEdge(lastNode, newENodeVertex);

        //newTEdge neighbors
        graph -> AddEdge(newTNodeVertex, HNNeighbors[0]);
        graph -> AddEdge(newTNodeVertex, hangingNode);
        graph -> AddEdge(newTNodeVertex, lastNode);
        
        (*graph)[TEdge].attributes->SetBool(RIVARA_ATTRIBUTE_R, false);
    }


    std::unique_ptr<std::vector<RivaraP2>> RivaraP2::FindAllMatches(std::shared_ptr<CachedGraph> g)
    {
        std::unique_ptr<std::vector<RivaraP2>> result = std::make_unique<std::vector<RivaraP2>>();
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
            if(commonEEdges.size()==2)
            {
                auto tempNNodes1 = g -> GetAdjacentVertices(commonEEdges[0]);
                auto tempNNodes2 = g -> GetAdjacentVertices(commonEEdges[1]);
                std::vector<vertex_descriptor> intersection;
                std::set_intersection(
                    tempNNodes1.begin(),
                    tempNNodes1.end(),
                    tempNNodes2.begin(), 
                    tempNNodes2.end(),
                    std::back_inserter(intersection));
                std::vector<vertex_descriptor> hangingNodeNeighbors;
                hangingNodeNeighbors = where(vertices, [&intersection](vertex_descriptor v){return v!=intersection[0];});
                auto tempENodes1 = g -> GetAdjacentVertices(hangingNodeNeighbors[0]);
                auto x = g -> GetAdjacentVertices(hangingNodeNeighbors[1]);
                tempENodes1.insert(tempENodes1.end(), x.begin(), x.end());
                std::vector<vertex_descriptor> possibleHangingNodes;
                for(auto EEdge : tempENodes1)
                {
                    auto tmp = g -> GetAdjacentVertices(EEdge);
                    possibleHangingNodes.insert(possibleHangingNodes.end(), tmp.begin(), tmp.end());
                }
                double expectedX =
                    (*g)[hangingNodeNeighbors[0]].attributes->GetDouble(RIVARA_ATTRIBUTE_X)
                    + (*g)[hangingNodeNeighbors[1]].attributes->GetDouble(RIVARA_ATTRIBUTE_X);  
                double expectedY =
                    (*g)[hangingNodeNeighbors[0]].attributes->GetDouble(RIVARA_ATTRIBUTE_Y)
                    + (*g)[hangingNodeNeighbors[1]].attributes->GetDouble(RIVARA_ATTRIBUTE_Y);
                auto hangingNode = where(possibleHangingNodes, [&g, expectedX, expectedY](vertex_descriptor v){
                    return true
                        and std::abs((*g)[v].attributes->GetDouble(RIVARA_ATTRIBUTE_X) - expectedX) < 0.1
                        and std::abs((*g)[v].attributes->GetDouble(RIVARA_ATTRIBUTE_Y) - expectedY) < 0.1;
                    })[0];
                auto hangingNodeENeighbors = g -> GetAdjacentVertices(hangingNode);
                double lengthOfSplitEdge = (*g)[hangingNodeENeighbors[0]].attributes->GetDouble(RIVARA_ATTRIBUTE_L)
                 + (*g)[hangingNodeENeighbors[1]].attributes->GetDouble(RIVARA_ATTRIBUTE_L);
                if(true
                and lengthOfSplitEdge>=(*g)[commonEEdges[0]].attributes -> GetDouble(RIVARA_ATTRIBUTE_L)
                and lengthOfSplitEdge>=(*g)[commonEEdges[1]].attributes -> GetDouble(RIVARA_ATTRIBUTE_L)
                )
                result -> emplace_back(g, hangingNode, triangle);
            }
        }
        return result;
    }
}
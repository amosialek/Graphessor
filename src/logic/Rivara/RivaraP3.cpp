#include "RivaraP3.hpp"

namespace Rivara
{
    RivaraP3::RivaraP3(std::shared_ptr<IGraph> graph,
            vertex_descriptor EEdgeToBreak,
            vertex_descriptor TEdge,
            std::shared_ptr<Image> image) 
            :
                graph(graph), 
                EEdgeToBreak(EEdgeToBreak),
                TEdge(TEdge),
                image(image)
        {}

    void RivaraP3::Perform()
    {
        spdlog::debug("Rivara P3");
        auto nodes = graph -> GetAdjacentVertices(EEdgeToBreak);
        auto vertices = graph -> GetAdjacentVertices(TEdge);
        std::vector<vertex_descriptor> lastNodeSet;
        std::set_difference(
            vertices.begin(),
            vertices.end(),
            nodes.begin(), 
            nodes.end(),
            std::back_inserter(lastNodeSet));

        auto lastNode = lastNodeSet[0];

        auto newTNode = GetNewTNode();
        auto newNNode = GetNewNNode(graph, nodes, image);
        auto newMiddleENode = GetNewEMiddleNode(graph, newNNode, lastNode);
        auto newENode = GetNewENode(graph, EEdgeToBreak);

        vertex_descriptor newNNodeVertex = graph -> AddVertex(newNNode);
        vertex_descriptor newENodeVertex = graph -> AddVertex(newENode);
        vertex_descriptor newTNodeVertex = graph -> AddVertex(newTNode);
        vertex_descriptor newMiddleENodeVertex = graph -> AddVertex(newMiddleENode);

        graph -> RemoveEdge(EEdgeToBreak, nodes[0]);
        graph -> AddEdge(EEdgeToBreak, newNNodeVertex);

        graph -> AddEdge(nodes[0], newENodeVertex);
        graph -> AddEdge(newNNodeVertex, newENodeVertex);

        graph -> RemoveEdge(TEdge, nodes[0]);
        graph -> AddEdge(TEdge, newNNodeVertex);

        graph -> AddEdge(nodes[0], newTNodeVertex);
        graph -> AddEdge(newNNodeVertex, newTNodeVertex);
        graph -> AddEdge(lastNode, newTNodeVertex);
        
        graph -> AddEdge(newMiddleENodeVertex, lastNode);
        graph -> AddEdge(newMiddleENodeVertex, newNNodeVertex);

        (*graph)[EEdgeToBreak].x = ((*graph)[nodes[1]].x+newNNode.x)/2;
        (*graph)[EEdgeToBreak].y = ((*graph)[nodes[1]].y+newNNode.y)/2;
        (*graph)[newENodeVertex].x = ((*graph)[nodes[0]].x+newNNode.x)/2;
        (*graph)[newENodeVertex].y = ((*graph)[nodes[0]].y+newNNode.y)/2;
        (*graph)[TEdge].x = ((*graph)[nodes[1]].x+newNNode.x+(*graph)[lastNode].x)/3;
        (*graph)[TEdge].y = ((*graph)[nodes[1]].y+newNNode.y+(*graph)[lastNode].y)/3;
        (*graph)[newTNodeVertex].x = ((*graph)[nodes[0]].x+newNNode.x+(*graph)[lastNode].x)/3;
        (*graph)[newTNodeVertex].y = ((*graph)[nodes[0]].y+newNNode.y+(*graph)[lastNode].y)/3;
        (*graph)[TEdge].attributes->SetBool(RIVARA_ATTRIBUTE_R, false);

        
    }

    std::unique_ptr<std::vector<RivaraP3>> RivaraP3::FindAllMatches(std::shared_ptr<CachedGraph> g, std::shared_ptr<Image> image)
    {
       std::unique_ptr<std::vector<RivaraP3>> result = std::make_unique<std::vector<RivaraP3>>();
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
                    ((*g)[hangingNodeNeighbors[0]].attributes->GetDouble(RIVARA_ATTRIBUTE_X)
                    + (*g)[hangingNodeNeighbors[1]].attributes->GetDouble(RIVARA_ATTRIBUTE_X))/2;  
                double expectedY =
                    ((*g)[hangingNodeNeighbors[0]].attributes->GetDouble(RIVARA_ATTRIBUTE_Y)
                    + (*g)[hangingNodeNeighbors[1]].attributes->GetDouble(RIVARA_ATTRIBUTE_Y))/2;
                auto hangingNode = where(possibleHangingNodes, [&g, expectedX, expectedY](vertex_descriptor v){
                    return true
                        and std::abs((*g)[v].attributes->GetDouble(RIVARA_ATTRIBUTE_X) - expectedX) < 0.1
                        and std::abs((*g)[v].attributes->GetDouble(RIVARA_ATTRIBUTE_Y) - expectedY) < 0.1;
                    })[0];
                auto hangingNodeENeighbors = g -> GetAdjacentVertices(hangingNode);
                double lengthOfSplitEdge = (*g)[hangingNodeENeighbors[0]].attributes->GetDouble(RIVARA_ATTRIBUTE_L)
                 + (*g)[hangingNodeENeighbors[1]].attributes->GetDouble(RIVARA_ATTRIBUTE_L);
                if(true
                    and lengthOfSplitEdge<(*g)[commonEEdges[0]].attributes -> GetDouble(RIVARA_ATTRIBUTE_L)
                    and (*g)[commonEEdges[0]].attributes -> GetDouble(RIVARA_ATTRIBUTE_L)>=(*g)[commonEEdges[1]].attributes -> GetDouble(RIVARA_ATTRIBUTE_L)
                    and 
                    (
                        (*g)[commonEEdges[0]].attributes -> GetBool(RIVARA_ATTRIBUTE_B)
                        or 
                        (
                            EdgeHasNoHangingNodes(*g, commonEEdges[0])
                            and 
                            !(
                                (*g)[commonEEdges[1]].attributes -> GetBool(RIVARA_ATTRIBUTE_B)
                                and 
                                (*g)[commonEEdges[0]].attributes -> GetBool(RIVARA_ATTRIBUTE_L)!=(*g)[commonEEdges[1]].attributes -> GetBool(RIVARA_ATTRIBUTE_L)
                            )
                        )
                    )
                )
                {
                    result -> emplace_back(g, commonEEdges[0], triangle, image);
                }
                if(true
                    and lengthOfSplitEdge<(*g)[commonEEdges[1]].attributes -> GetDouble(RIVARA_ATTRIBUTE_L)
                    and (*g)[commonEEdges[1]].attributes -> GetDouble(RIVARA_ATTRIBUTE_L)>=(*g)[commonEEdges[0]].attributes -> GetDouble(RIVARA_ATTRIBUTE_L)
                    and 
                    (
                        (*g)[commonEEdges[1]].attributes -> GetBool(RIVARA_ATTRIBUTE_B)
                        or 
                        (
                            EdgeHasNoHangingNodes(*g, commonEEdges[1])
                            and 
                            !(
                                (*g)[commonEEdges[0]].attributes -> GetBool(RIVARA_ATTRIBUTE_B)
                                and 
                                (*g)[commonEEdges[1]].attributes -> GetBool(RIVARA_ATTRIBUTE_L)!=(*g)[commonEEdges[0]].attributes -> GetBool(RIVARA_ATTRIBUTE_L)
                            )
                        )
                    )
                )
                {
                    result -> emplace_back(g, commonEEdges[1], triangle, image);
                }
            }
        }
        return result; 
    }




}
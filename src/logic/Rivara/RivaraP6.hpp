#ifndef Rivara_P6_HPP
#define Rivara_P6_HPP

#include "Productions.hpp"
#include "RivaraAttributes.hpp"
#include <optional>

namespace Rivara
{
    class RivaraP6 {
        private:
            std::shared_ptr<IGraph> graph;
            vertex_descriptor hangingNode;
            vertex_descriptor TEdge;
        public:
            RivaraP6(std::shared_ptr<IGraph> graph,
                vertex_descriptor hangingNode,
                vertex_descriptor TEdge,
                );
            Pixel GetNewENode(Pixel& newNNode, std::vector<vertex_descriptor>& nodes);
            Pixel GetNewNNode(std::vector<vertex_descriptor> nodes);
            Pixel GetNewTNode();
            Pixel GetNewEMiddleNode(Pixel& newNNode, vertex_descriptor lastNode);
            void Perform();
            static std::unique_ptr<std::vector<RivaraP6>> FindAllMatches(std::shared_ptr<CachedGraph> g, std::shared_ptr<Image> image);
            std::vector<uint8_t> Serialize();   
            static bool EdgeHasNoHangingNodes(IGraph& graph, vertex_descriptor v);
    }; 
}

#endif /* Rivara_P4_HPP */

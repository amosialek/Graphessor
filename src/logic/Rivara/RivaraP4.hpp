#ifndef Rivara_P4_HPP
#define Rivara_P4_HPP

#include "Productions.hpp"
#include "RivaraAttributes.hpp"
#include <optional>

namespace Rivara
{
    class RivaraP4 {
        private:
            std::shared_ptr<IGraph> graph;
            vertex_descriptor NNode;
            vertex_descriptor TEdge;
        public:
            RivaraP4(std::shared_ptr<IGraph> graph,
                vertex_descriptor NNode,
                vertex_descriptor TEdge);
            Pixel GetNewTNode();
            Pixel GetNewEMiddleNode(Pixel& newNNode, vertex_descriptor lastNode);
            void Perform();
            static std::unique_ptr<std::vector<RivaraP4>> FindAllMatches(std::shared_ptr<CachedGraph> g, std::shared_ptr<Image> image);
            std::vector<uint8_t> Serialize();   
            static bool EdgeHasNoHangingNodes(vertex_descriptor v);
    }; 
}

#endif /* Rivara_P4_HPP */

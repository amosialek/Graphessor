#ifndef Rivara_P2_HPP
#define Rivara_P2_HPP

#include "Productions.hpp"
#include "RivaraAttributes.hpp"
#include <optional>

namespace Rivara
{
    class RivaraP2 {
        private:
            std::shared_ptr<IGraph> graph;
            vertex_descriptor hangingNode;
            vertex_descriptor TEdge;
        public:
            RivaraP2(std::shared_ptr<IGraph> graph,
                vertex_descriptor EEdge,
                vertex_descriptor TEdge);
            Pixel GetNewENode(Pixel& newNNode, vertex_descriptor hangingNode);
            Pixel GetNewTNode();
            void Perform();
            static std::unique_ptr<std::vector<RivaraP2>> FindAllMatches(std::shared_ptr<CachedGraph> g);
            std::vector<uint8_t> Serialize();   
    }; 
}

#endif /* Rivara_P1_HPP */

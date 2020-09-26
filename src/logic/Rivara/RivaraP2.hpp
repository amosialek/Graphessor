#ifndef Rivara_P2_HPP
#define Rivara_P2_HPP

#include "Productions.hpp"
#include "RivaraUtils.hpp"
#include <optional>

namespace Rivara
{
    class RivaraP2 {
        private:
            std::shared_ptr<RivaraCachedGraph> graph;
            vertex_descriptor hangingNode;
            vertex_descriptor TEdge;
        public:
            RivaraP2(std::shared_ptr<RivaraCachedGraph> graph,
                vertex_descriptor EEdge,
                vertex_descriptor TEdge);
            void Perform();
            static std::unique_ptr<std::vector<RivaraP2>> FindAllMatches(std::shared_ptr<RivaraCachedGraph> g);
            std::vector<uint8_t> Serialize();   
    }; 
}

#endif /* Rivara_P1_HPP */

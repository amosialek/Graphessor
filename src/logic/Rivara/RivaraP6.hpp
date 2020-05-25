#ifndef Rivara_P6_HPP
#define Rivara_P6_HPP

#include "Productions.hpp"
#include "RivaraUtils.hpp"
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
                vertex_descriptor TEdge
                );
            void Perform();
            static std::unique_ptr<std::vector<RivaraP6>> FindAllMatches(std::shared_ptr<RivaraCachedGraph> g);
            std::vector<uint8_t> Serialize();   
    }; 
}

#endif /* Rivara_P4_HPP */

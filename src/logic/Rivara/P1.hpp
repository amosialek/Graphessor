#ifndef Rivara_P1_HPP
#define Rivara_P1_HPP

#include "Productions.hpp"
#include "RivaraAttributes.hpp"
#include <optional>

namespace Rivara
{
    class P1 {
        private:
            std::shared_ptr<IGraph> graph;
            vertex_descriptor EEdge;
            vertex_descriptor TEdge;
            std::shared_ptr<Image> image;
        public:
            P1(std::shared_ptr<IGraph> graph,
            vertex_descriptor EEdge,
            vertex_descriptor TEdge,
            std::shared_ptr<Image> image);
            void Perform();
            static std::unique_ptr<std::vector<P1>> FindAllMatches(std::shared_ptr<CachedGraph> g, std::shared_ptr<Image> image);
            std::vector<uint8_t> Serialize();

            ///
            /// Returns Best Edge to break in P1 production. g: graph, EEdges
            /// @param g - graph
            /// @param EEdges - edges to choose from. It works properly for 3 edges only
            ///
            static std::optional<vertex_descriptor> GetBestEdge(std::shared_ptr<CachedGraph> g, std::vector<vertex_descriptor> EEdges);
    }; 
}

#endif /* Rivara_P1_HPP */

#ifndef Rivara_P1_HPP
#define Rivara_P1_HPP

#include "Productions.hpp"
#include "RivaraAttributes.hpp"
#include <optional>

namespace Rivara
{
    class RivaraP1 {
        private:
            std::shared_ptr<IGraph> graph;
            vertex_descriptor EEdge;
            vertex_descriptor TEdge;
            std::shared_ptr<Image> image;
        public:
            RivaraP1(std::shared_ptr<IGraph> graph,
            vertex_descriptor EEdge,
            vertex_descriptor TEdge,
            std::shared_ptr<Image> image);
            Pixel GetNewENode(Pixel& newNNode, std::vector<vertex_descriptor>& nodes);
            Pixel GetNewNNode(std::vector<vertex_descriptor> nodes);
            Pixel GetNewTNode();
            Pixel GetNewEMiddleNode(Pixel& newNNode, vertex_descriptor lastNode);
            void Perform();
            static std::unique_ptr<std::vector<RivaraP1>> FindAllMatches(std::shared_ptr<CachedGraph> g, std::shared_ptr<Image> image);
            std::vector<uint8_t> Serialize();

            ///
            /// Returns Best Edge to break in RivaraP1 production. g: graph, EEdges
            /// @param g - graph
            /// @param EEdges - edges to choose from. It works properly for 3 edges only
            ///
            static std::optional<vertex_descriptor> GetBestEdge(std::shared_ptr<CachedGraph> g, const std::vector<vertex_descriptor>& EEdges);
    }; 
}

#endif /* Rivara_P1_HPP */

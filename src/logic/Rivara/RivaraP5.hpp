#ifndef Rivara_P5_HPP
#define Rivara_P5_HPP

#include "Productions.hpp"
#include "RivaraUtils.hpp"
#include <optional>

namespace Rivara
{
    class RivaraP5 {
        private:
            std::shared_ptr<IGraph> graph;
            vertex_descriptor NNode;
            vertex_descriptor EEdge;
            vertex_descriptor TEdge;
            std::shared_ptr<Image> image;
        public:
            RivaraP5(std::shared_ptr<IGraph> graph,
                vertex_descriptor NNode,
                vertex_descriptor TEdge,
                vertex_descriptor EEdge,
                std::shared_ptr<Image> image
                );
            void Perform();
            static std::unique_ptr<std::vector<RivaraP5>> FindAllMatches(std::shared_ptr<RivaraCachedGraph> g, std::shared_ptr<Image> image);
            std::vector<uint8_t> Serialize();   
    }; 
}

#endif /* Rivara_P4_HPP */

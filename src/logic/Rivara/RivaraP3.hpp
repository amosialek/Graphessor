#ifndef Rivara_P3_HPP
#define Rivara_P3_HPP

#include "Productions.hpp"
#include "RivaraUtils.hpp"
#include <optional>

namespace Rivara
{
    class RivaraP3 {
        private:
            std::shared_ptr<IGraph> graph;
            vertex_descriptor EEdgeToBreak;
            vertex_descriptor TEdge;
            std::shared_ptr<Image> image;
        public:
            RivaraP3(std::shared_ptr<IGraph> graph,
                vertex_descriptor EEdge,
                vertex_descriptor TEdge,
                std::shared_ptr<Image> image);
            void Perform();
            static std::unique_ptr<std::vector<RivaraP3>> FindAllMatches(std::shared_ptr<CachedGraph> g, std::shared_ptr<Image> image);
            std::vector<uint8_t> Serialize();   
    }; 
}

#endif /* Rivara_P3_HPP */

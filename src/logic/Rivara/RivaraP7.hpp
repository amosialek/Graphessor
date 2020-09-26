#ifndef Rivara_P7_HPP
#define Rivara_P7_HPP

#include "Productions.hpp"
#include "RivaraUtils.hpp"
#include <optional>

namespace Rivara
{
    class RivaraP7 {
        private:
            std::shared_ptr<RivaraCachedGraph> graph;
            vertex_descriptor TEdge;
        public:
            RivaraP7(std::shared_ptr<RivaraCachedGraph> graph,
                vertex_descriptor TEdge
                );
            void Perform();
            static std::unique_ptr<std::vector<RivaraP7>> FindAllMatches(std::shared_ptr<RivaraCachedGraph> g, 
                                                                        std::shared_ptr<Image> image,
                                                                        int channel,
                                                                        double epsilon);
            std::vector<uint8_t> Serialize();   
    }; 
}

#endif /* Rivara_P4_HPP */

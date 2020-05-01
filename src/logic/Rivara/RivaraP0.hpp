#ifndef Rivara_P0_HPP
#define Rivara_P0_HPP

#include "Productions.hpp"
#include "RivaraUtils.hpp"

namespace Rivara
{
    ///
    /// Starting production for Rivara grammar
    ///
    class RivaraP0 {
        private:
            std::shared_ptr<IGraph> graph;
            vertex_descriptor S;
            std::shared_ptr<Image> image;
        public:
            RivaraP0(std::shared_ptr<IGraph> graph,
            vertex_descriptor S,
            std::shared_ptr<Image> image);
            void Perform() ;
            std::vector<uint8_t> Serialize() ;
    }; 
}
#endif /* Rivara_P0_HPP */

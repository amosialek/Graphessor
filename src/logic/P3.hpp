#ifndef P3_HPP
#define P3_HPP

#include "Productions.hpp"
#include "linq.h"

class P3 : public Production{
    private:
        std::shared_ptr<IGraph> graph;
        vertex_descriptor BEdge;
        std::shared_ptr<Image> image;
    public:
        P3(
            std::shared_ptr<IGraph> graph,
            vertex_descriptor BEdge,
            std::shared_ptr<Image> image);
        void Perform() override;
        std::vector<uint8_t> Serialize() override;
        static std::unique_ptr<std::vector<P3>> FindAllMatches(
            std::shared_ptr<CachedGraph> graph,
            std::shared_ptr<Image> image);
}; 

#endif /* P3_HPP */

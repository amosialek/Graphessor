#ifndef P6_HPP
#define P6_HPP

#include "Productions.hpp"
#include "linq.h"

class P6 : public Production{
    private:
        std::shared_ptr<IGraph> graph;
        vertex_descriptor IEdge;
    public:
        P6(std::shared_ptr<IGraph> graph, vertex_descriptor IEdge);
        void Perform() override;
        static std::unique_ptr<std::vector<P6>> PerformAllMatches(std::shared_ptr<CachedGraph> graph);
        std::vector<uint8_t> Serialize() override;
        auto PerformAndGetAdjacentEdges();
}; 

#endif /* P6_HPP */

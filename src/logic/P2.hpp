#ifndef P2_HPP
#define P2_HPP

#include "Productions.hpp"

class P2 : public Production{
    private:
        std::shared_ptr<IGraph> graph;
        vertex_descriptor IEdge;
        std::shared_ptr<Image> image;
    public:
        P2(std::shared_ptr<IGraph> graph,
            vertex_descriptor IEdge,
            std::shared_ptr<Image> image);
        void Perform() override;
        std::vector<uint8_t> Serialize() override;
        static std::unique_ptr<std::vector<P2>> FindAllMatches(std::shared_ptr<CachedGraph> graph,
            std::shared_ptr<Image> image);
}; 

#endif /* P2_HPP */

#ifndef P4_HPP
#define P4_HPP

#include "Productions.hpp"

class P4 : public Production{
    private:
        std::shared_ptr<IGraph> graph;
        vertex_descriptor FEdge;
        std::shared_ptr<Image> image;
    public:
        P4(std::shared_ptr<IGraph> graph,
        vertex_descriptor FEdge,
        std::shared_ptr<Image> image);
        void Perform() override;
        std::vector<uint8_t> Serialize() override;
        static std::unique_ptr<std::vector<P4>> FindAllMatches(std::shared_ptr<CachedGraph> graph,
            std::shared_ptr<Image> image);
}; 

#endif /* P4_HPP */

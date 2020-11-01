#ifndef P5_HPP
#define P5_HPP

#include "Productions.hpp"
#include "Array2D.hpp"

class P5 : public Production{
    private:
        std::shared_ptr<IGraph> graph;
        vertex_descriptor IEdge;
    public:
        P5(std::shared_ptr<IGraph> graph,
            vertex_descriptor IEdge);
        void Perform() override;
        std::vector<uint8_t> Serialize() override;
        static std::unique_ptr<std::vector<P5>> FindAllMatches(std::shared_ptr<CachedGraph> graph,
            std::shared_ptr<Array2D> image,
            std::shared_ptr<Array2D> interpolation,
            int channel,
            double epsilon);
}; 

#endif /* P5_HPP */

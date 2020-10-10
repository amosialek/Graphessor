#ifndef P1_HPP
#define P1_HPP

#include "Productions.hpp"

class P1 : public Production{
    private:
        std::shared_ptr<IGraph> graph;
        vertex_descriptor S;
        std::shared_ptr<Image> image;
    public:
        P1(std::shared_ptr<IGraph> graph,
        vertex_descriptor S,
        std::shared_ptr<Image> image);
        void Perform() override;
        std::vector<uint8_t> Serialize() override;
}; 

#endif /* P1_HPP */

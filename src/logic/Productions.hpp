#ifndef __PRODUCTIONS_HPP__
#define __PRODUCTIONS_HPP__

#include <vector>
#include "mygraph.hpp"
#include "Image.hpp"
#include "Production.hpp"

class P1 : public Production{
    private:
        std::shared_ptr<IGraph> graph;
        std::shared_ptr<std::vector<vertex_descriptor>> listOfIEdges;
        std::shared_ptr<std::vector<vertex_descriptor>> listOfBEdges;
        vertex_descriptor S;
        std::shared_ptr<Image> image;
    public:
        P1(std::shared_ptr<IGraph> graph,
        vertex_descriptor S,
        std::shared_ptr<std::vector<vertex_descriptor>> listOfIEdges,
        std::shared_ptr<std::vector<vertex_descriptor>> listOfBEdges,
        std::shared_ptr<Image> image);
        void Perform() override;
        std::vector<uint8_t> Serialize() override;
}; 

class P2 : Production{
    private:
        std::shared_ptr<IGraph> graph;
        vertex_descriptor IEdge;
        std::shared_ptr<std::vector<vertex_descriptor>> listOfFEdges;
        std::shared_ptr<Image> image;
    public:
        P2(std::shared_ptr<IGraph> graph,
            vertex_descriptor IEdge,
            std::shared_ptr<std::vector<vertex_descriptor>> listOfFEdges,
            std::shared_ptr<Image> image);
        void Perform() override;
        std::vector<uint8_t> Serialize() override;
        static std::vector<P2> FindAllMatches(std::shared_ptr<IGraph> graph,
            std::shared_ptr<std::vector<vertex_descriptor>> listOfIEdges,
            std::shared_ptr<std::vector<vertex_descriptor>> listOfFEdges,
            std::shared_ptr<Image> image);
}; 

class P3 : Production{
    private:
        std::shared_ptr<IGraph> graph;
        std::shared_ptr<std::vector<vertex_descriptor>> listOfBEdges;
        std::shared_ptr<Image> image;
    public:
        P3(
            std::shared_ptr<IGraph> graph,
            std::shared_ptr<std::vector<vertex_descriptor>> listOfBEdges,
            std::shared_ptr<Image> image);
        void Perform() override;
        std::vector<uint8_t> Serialize() override;
}; 

class P4 : Production{
    private:
        std::shared_ptr<IGraph> graph;
        std::shared_ptr<std::vector<vertex_descriptor>> listOfFEdges;
        std::shared_ptr<Image> image;
    public:
        P4(std::shared_ptr<IGraph> graph,
        std::shared_ptr<std::vector<vertex_descriptor>> listOfFEdges, 
        std::shared_ptr<Image> image);
        void Perform() override;
        std::vector<uint8_t> Serialize() override;
}; 

class P5 : Production{
    private:
        std::shared_ptr<IGraph> graph;
        std::shared_ptr<std::vector<vertex_descriptor>> listOfIEdges;
        std::shared_ptr<Image> image;
        int channel;
        double epsilon;
    public:
        P5(std::shared_ptr<IGraph> graph,
            std::shared_ptr<std::vector<vertex_descriptor>> listOfIEdges,
            std::shared_ptr<Image> image,
            int channel,
            double epsilon);
        void Perform() override;
        std::vector<uint8_t> Serialize() override;
}; 

class P6 : Production{
    private:
        std::shared_ptr<IGraph> graph;
        std::shared_ptr<std::vector<vertex_descriptor>> listOfIEdges;
    public:
        P6(std::shared_ptr<IGraph> graph,
        std::shared_ptr<std::vector<vertex_descriptor>>  listOfIEdges);
        void Perform() override;
        std::vector<uint8_t> Serialize() override;
}; 

#endif // __PRODUCTIONS_HPP__
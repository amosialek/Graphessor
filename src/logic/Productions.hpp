#ifndef __PRODUCTIONS_HPP__
#define __PRODUCTIONS_HPP__

#include <vector>
#include "mygraph.hpp"
#include "Image.hpp"
#include "Production.hpp"
#include "CachedGraph.hpp"

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
        std::shared_ptr<Image> image;
    public:
        P2(std::shared_ptr<IGraph> graph,
            vertex_descriptor IEdge,
            std::shared_ptr<Image> image);
        void Perform() override;
        std::vector<uint8_t> Serialize() override;
        static std::vector<P2> FindAllMatches(std::shared_ptr<CachedGraph> graph,
            std::shared_ptr<Image> image);
}; 

class P3 : Production{
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

class P4 : Production{
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

class P5 : Production{
    private:
        std::shared_ptr<IGraph> graph;
        vertex_descriptor IEdge;
    public:
        P5(std::shared_ptr<IGraph> graph,
            vertex_descriptor IEdge);
        void Perform() override;
        std::vector<uint8_t> Serialize() override;
        static std::unique_ptr<std::vector<P5>> FindAllMatches(std::shared_ptr<CachedGraph> graph,
            std::shared_ptr<Image> image,
            int channel,
            double epsilon);
}; 

class P6 : Production{
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

#endif // __PRODUCTIONS_HPP__
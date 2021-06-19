#ifndef CACHEDGRAPH_HPP
#define CACHEDGRAPH_HPP

#include "mygraph.hpp"
#include "Pixel.hpp"

class CachedGraph : public IGraph
{
    protected:
        std::map<std::string, std::shared_ptr<std::set<vertex_descriptor>>> typeToVerticesCache;
        std::unique_ptr<PixelGraph> graph;
        void EnsureCacheExists(std::string type);
        void InsertCacheElement(std::string type, vertex_descriptor v);
        void ChangeCachedElementType(vertex_descriptor v, std::string type1, std::string type2);
    public:
        CachedGraph();
        CachedGraph(const CachedGraph& other);
        void AddEdge(vertex_descriptor v1,vertex_descriptor v2) override;
        virtual vertex_descriptor AddVertex(Pixel p) override;
        void RemoveEdge(vertex_descriptor v1,vertex_descriptor v2) override;
        void ChangeVertexType(vertex_descriptor v, std::string type) override;
        Pixel& operator[](vertex_descriptor v) override;
        std::set<vertex_descriptor>& GetCacheIterator(std::string type);
        std::vector<vertex_descriptor> GetAdjacentVertices(vertex_descriptor v) override;
        std::vector<vertex_descriptor> GetAdjacentVertices(vertex_descriptor v, std::string type) override;
        PixelGraph GetGraph() override;
        virtual std::vector<std::vector<Pixel>> GetPixelsForBilinearInterpolation();
        virtual std::vector<std::vector<Pixel>> GetPixelsForBaricentricInterpolation();
        virtual std::vector<std::vector<Pixel>> GetPixelsForSVDInterpolation();
        virtual const std::set<vertex_descriptor>& GetPixels();
        virtual void Serialize(std::ostream& stream) override;  
        void Deserialize(std::istream& stream);  
        void DecreaseXAndYByRatio(int ratio);
};

#endif /* CACHEDGRAPH_HPP */

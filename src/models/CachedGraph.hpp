#ifndef CACHEDGRAPH_HPP
#define CACHEDGRAPH_HPP

#include "mygraph.hpp"

#include "mygraph.hpp"

class CachedGraph : public IGraph
{
    private:
        std::map<std::string, std::unique_ptr<std::set<vertex_descriptor>>> typeToVerticesCache;
        std::unique_ptr<PixelGraph> graph;
        void EnsureCacheExists(std::string type)
        {
            std::map<std::string, std::unique_ptr<std::set<vertex_descriptor>>>::iterator it = typeToVerticesCache.find(type);
            if(it==typeToVerticesCache.end())
            {
                typeToVerticesCache[type] = std::make_unique<std::set<vertex_descriptor>>();
            }
        }
        void InsertCacheElement(std::string type, vertex_descriptor v)
        {
            EnsureCacheExists(type);
            {
                typeToVerticesCache[type] -> emplace(v);
            }
        }
        void ChangeCachedElementType(vertex_descriptor v, std::string type1, std::string type2)
        {
            EnsureCacheExists(type1);
            EnsureCacheExists(type2);
            typeToVerticesCache[type1] -> erase(v);
            typeToVerticesCache[type2] -> emplace(v);
        }
    public:
        CachedGraph()
        {
            graph = std::make_unique<PixelGraph>();
        };
        edge_descriptor AddEdge(vertex_descriptor v1,vertex_descriptor v2) override
        {
            add_edge(v1,v2,*graph);
        };

        vertex_descriptor AddVertex(Pixel p) override
        {
            vertex_descriptor v = add_vertex(p, *graph);
            InsertCacheElement(p.label, v);
            return v;
        };

        void RemoveEdge(vertex_descriptor v1,vertex_descriptor v2) override
        {
            remove_edge(v1,v2,*graph);
        };

        void ChangeVertexType(vertex_descriptor v, std::string type) override
        {
            ChangeCachedElementType(v, (*graph)[v].label, type);
            (*graph)[v].label = type;
        };

        Pixel& operator[](vertex_descriptor v) override
        {
            return (*graph)[v];
        };

        const std::set<vertex_descriptor>& getCacheIterator(std::string type)
        {
            EnsureCacheExists(type);
            return *(typeToVerticesCache[type]);
        };

        std::vector<vertex_descriptor> GetAdjacentVertices(vertex_descriptor v)
        {
            std::vector<vertex_descriptor> result;
            
            adjacency_iterator currentNeighbour, endOfNeighbours;
            std::tie(currentNeighbour, endOfNeighbours) = adjacent_vertices(v, *graph);
            for(;currentNeighbour!=endOfNeighbours;++currentNeighbour)
            {
                result.push_back(*currentNeighbour);
            }
            return result;
        }

        PixelGraph GetGraph()
        {
            return *graph;
        }
};

#endif /* CACHEDGRAPH_HPP */

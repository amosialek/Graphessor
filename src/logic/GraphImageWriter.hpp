#ifndef __GRAPHIMAGEWRITER_HPP__
#define __GRAPHIMAGEWRITER_HPP__

#include "CachedGraph.hpp"
#include <string>

class GraphImageWriter
{
    public:
        static void DrawPixels(std::shared_ptr<CachedGraph> graph, std::string filename);
        static void DrawFullGraph(std::shared_ptr<CachedGraph> graph, std::string filename);
};

#endif // __GRAPHIMAGEWRITER_HPP__

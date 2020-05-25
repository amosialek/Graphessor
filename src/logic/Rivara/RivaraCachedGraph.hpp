#ifndef __RIVARACACHEDGRAPH_HPP__
#define __RIVARACACHEDGRAPH_HPP__

#include "CachedGraph.hpp"

namespace Rivara
{
    class RivaraCachedGraph : public CachedGraph
    {
        public:
            RivaraCachedGraph() : CachedGraph(){};
            virtual std::vector<std::vector<Pixel>> GetPixelsForBilinearInterpolation() override;
            virtual std::vector<std::vector<Pixel>> GetPixelsForBaricentricInterpolation() override;
            virtual const std::set<vertex_descriptor>& GetPixels() override;
            void RegisterHangingNode(vertex_descriptor v);
            std::set<vertex_descriptor> GetHangingNodes();
            void DeregisterHangingNode(vertex_descriptor v);
            vertex_descriptor AddVertex(Pixel p) override;
    };
}
#endif // __RIVARACACHEDGRAPH_HPP__

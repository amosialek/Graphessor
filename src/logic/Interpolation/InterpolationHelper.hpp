#ifndef __INTERPOLATIONHELPER_HPP__
#define __INTERPOLATIONHELPER_HPP__

#include <vector>
#include "mygraph.hpp"
#include "Image.hpp"
#include "CachedGraph.hpp"
#include "GraphessorConstants.hpp"
#include "spdlog/spdlog.h"
#include <cassert>

std::vector<double> GetSingleVertexFEdgeCoefficients(Array2D & image, IGraph& graph, vertex_descriptor edge, vertex_descriptor vertex, int minx, int miny, int maxx, int maxy, double secondVertexValue, int orders);

#endif // __INTERPOLATIONHELPER_HPP__

#ifndef __INTERPOLATIONHELPER_HPP__
#define __INTERPOLATIONHELPER_HPP__

#include <vector>
#include "mygraph.hpp"
#include "Image.hpp"
#include "CachedGraph.hpp"
#include "GraphessorConstants.hpp"
#include "spdlog/spdlog.h"
#include <cassert>

std::set<vertex_descriptor> GetEdges(IGraph& graph, vertex_descriptor iEdge, int minx, int maxx, int miny, int maxy);
std::vector<double> GetSingleVertexFEdgeCoefficients(Array2D & image, IGraph& graph, vertex_descriptor edge, vertex_descriptor vertex, int minx, int miny, int maxx, int maxy, double secondVertexValue, int orders);
std::tuple<std::map<int, std::map<int, double>>,std::map<std::string, std::vector<double>>> GetFEdgeWithTwoVerticesCoefficients(Array2D& image, Pixel& v0, Pixel& v1,int minx, int maxx, int miny, int maxy, int orders);
#endif // __INTERPOLATIONHELPER_HPP__

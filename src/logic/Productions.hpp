#ifndef __PRODUCTIONS_HPP__
#define __PRODUCTIONS_HPP__

#include <vector>
#include "mygraph.hpp"
#include "Image.hpp"

void P1(MyGraph& graph, vertex_descriptor S, std::vector<vertex_descriptor>& listOfIEdges, std::vector<vertex_descriptor>& listOfBEdges, Image image);
void P2(MyGraph& graph, std::vector<vertex_descriptor>& listOfIEdges, Image image);
void P3(MyGraph& graph, std::vector<vertex_descriptor>& listOfBEdges, Image image);

#endif // __PRODUCTIONS_HPP__
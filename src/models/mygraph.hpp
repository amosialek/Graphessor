#ifndef __MYGRAPH_HPP__
#define __MYGRAPH_HPP__

#include "Pixel.hpp"
#include <boost/graph/adjacency_list.hpp>
typedef boost::adjacency_list<
   boost::vecS,         //outEdge
   boost::vecS,         //vertex
   boost::undirectedS,  //directionaltype
   Pixel,               //Vertex type
   boost::no_property,  //Edge type
   boost::no_property,  //graph type (properties)
   boost::listS         //Edgelist
> MyGraph;
typedef boost::graph_traits<MyGraph>::edge_iterator edge_iterator;
typedef boost::graph_traits<MyGraph>::vertex_iterator vertex_iterator;
typedef boost::graph_traits<MyGraph>::adjacency_iterator adjacency_iterator;
typedef size_t vertex_descriptor;
typedef size_t edge_descriptor;
#endif // __MYGRAPH_HPP__
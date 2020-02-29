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

template <class Name>
class myEdgeWriter {
public:
     myEdgeWriter(Name _name) : name(_name) {}
     template <class VertexOrEdge>
     void operator()(std::ostream& out, const VertexOrEdge& v) const {
        char buffer[8];
        std::string s = "[";
        if(name[v].r!=-1){
            sprintf(buffer,"#%02X%02X%02X",name[v].r,name[v].g,name[v].b);
            s+= "fillcolor=\"";
            s+=buffer;
            s+="\",style=filled\n";
        }
        if(name[v].x>=0 && name[v].y>=0 )
            s+= "pos=\""+std::to_string(name[v].x/4)+','+std::to_string(name[v].y/4)+"!\"\n";
        s+="label=\""+ name[v].label +"\"";
        s+=']';
        out<<s<<'\n';
     }
private:
     Name name;
};
#endif // __MYGRAPH_HPP__
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
> PixelGraph;
typedef boost::graph_traits<PixelGraph>::edge_iterator edge_iterator;
typedef boost::graph_traits<PixelGraph>::vertex_iterator vertex_iterator;
typedef boost::graph_traits<PixelGraph>::adjacency_iterator adjacency_iterator;
typedef size_t vertex_descriptor;
typedef size_t edge_descriptor;

class IGraph
{
    public:
        virtual ~IGraph(){};
        virtual void AddEdge(vertex_descriptor v1,vertex_descriptor v2)=0;
        virtual vertex_descriptor AddVertex(Pixel p)=0;
        virtual void RemoveEdge(vertex_descriptor v1,vertex_descriptor v2)=0;
        virtual void ChangeVertexType(vertex_descriptor v1, std::string type)=0;
        virtual Pixel& operator[](vertex_descriptor v)=0;
        virtual std::vector<vertex_descriptor> GetAdjacentVertices(vertex_descriptor v)=0;
        virtual PixelGraph GetGraph()=0;
};


template <class Name>
class myEdgeWriter {
public:
    myEdgeWriter(Name& _name) : name(_name) {}
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
     Name& name;
};
#endif // __MYGRAPH_HPP__
#include<bits/stdc++.h>
#include <boost/config.hpp>
#include <boost/graph/graphviz.hpp>
#include <boost/graph/adjacency_list.hpp>
#include <boost/tuple/tuple.hpp>
using namespace std;

class BasePixel
{
    public: string colour;
    int x=-1,y=-1;
} ;

struct InterPixel
{
    string type;
};

typedef boost::adjacency_list<
    boost::vecS, boost::vecS, boost::undirectedS, BasePixel, InterPixel, boost::no_property, boost::vecS
> MyGraph;

typedef boost::graph_traits<MyGraph>::edge_iterator edge_iterator;

template <class Name>
class myEdgeWriter {
public:
     myEdgeWriter(Name _name) : name(_name) {}
     template <class VertexOrEdge>
     void operator()(std::ostream& out, const VertexOrEdge& v) const {
        string s = "[";
        if(name[v].colour!="")
            s+= "fillcolor=\""+name[v].colour+"\",style=filled\n";
        if(name[v].x>=0 && name[v].y>=0 )
            s+= "pos=\""+to_string(name[v].x%2)+','+to_string(name[v].y/2%2)+"!\"\n";
        s+=']';
        cout<<s<<endl;
     }
private:
     Name name;
};

int main()
{
    int b;
  MyGraph g;
  MyGraph::vertex_descriptor v1 = boost::add_vertex(g);
  MyGraph::vertex_descriptor v2 = boost::add_vertex(g);
  MyGraph::vertex_descriptor v3 = boost::add_vertex(g);
  MyGraph::vertex_descriptor v4 = boost::add_vertex(g);
  boost::add_edge(v1,v2,g);

      std::pair<edge_iterator, edge_iterator> ei = edges(g);
//       auto print = [](const InterPixel& n) { std::cout << " " << n.name; };
//   for_each(ei.first,ei.second,print);
//   std::copy( ei.first, ei.second,
//                 std::ostream_iterator<MyGraph::edge_descriptor>{
//                     std::cout, "\n"});
    g[0].colour = "#0000ff";  
    for(int i=0;i<4;i++)
        g[i].x=g[i].y=i;
    myEdgeWriter<MyGraph> w(g);
    // cout<<g[0].colour<<endl;
    boost::write_graphviz(cout, g, w);
}
#ifndef PRODUCTIONS_CPP
#define PRODUCTIONS_CPP
#include "Productions.hpp"

Production::~Production(){}

double GetDistance(const int x1, const int x2, const int y1, const int y2) //TODO: Consider long longs (for very large images)?
{
    return (x1-x2)*(x1-x2)+(y1-y2)*(y1-y2);
}

double GetDistance(const Pixel p1, const Pixel p2)
{
    return GetDistance(p1.x, p2.x, p1.y, p2.y);
}

bool haveCommonEdge(IGraph& graph, vertex_descriptor v1, vertex_descriptor v2, vertex_descriptor corner)
{
return !(true
&& signum(graph[v1].x - graph[corner].x) == signum(graph[corner].x - graph[v2].x)
&& signum(graph[v1].y - graph[corner].y) == signum(graph[corner].y - graph[v2].y));
}

#endif /* PRODUCTIONS_CPP */

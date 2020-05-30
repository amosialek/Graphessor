#ifndef __PRODUCTIONS_HPP__
#define __PRODUCTIONS_HPP__

#include <vector>
#include "mygraph.hpp"
#include "Image.hpp"
#include "Production.hpp"
#include "CachedGraph.hpp"
#include "GraphessorConstants.hpp"
#include <iosfwd>
#include <algorithm>
#include <queue>
#include "spdlog/spdlog.h"
#include <cassert>

class NotImplementedException : public std::logic_error
{
public:
    NotImplementedException() : std::logic_error("Function not yet implemented") { };
};

template<typename T, typename Pred>
std::vector<T> where(std::vector<T> inVec, Pred predicate)
{
  std::vector<T> result;
  std::copy_if(inVec.begin(), inVec.end(), std::back_inserter(result), predicate);
  return result;
}

template<typename T, typename Pred>
std::set<T> where(std::set<T> inVec, Pred predicate)
{
  std::set<T> result;
  std::copy_if(inVec.begin(), inVec.end(), std::inserter(result, result.begin()), predicate);
  return result;
}

template<typename T, typename Pred>
T first(std::vector<T> inVec, Pred predicate)
{
  for(auto element : inVec)
    if(predicate(element))
      return element;
  assert(false && "Sequence contains no matching elements");
}

template<typename T, typename Pred>
T first(std::set<T> inVec, Pred predicate)
{
  for(auto element : inVec)
    if(predicate(element))
      return element;
  assert(false && "Sequence contains no matching elements");
}

template<typename T>
std::vector<T> intersect(std::vector<T>& inVec1, std::vector<T>& inVec2)
{
  std::vector<T> result;
  if(inVec1.size()<inVec2.size())
  {
    std::set<T> tempset(inVec1.begin(), inVec1.end());
    std::copy_if(inVec2.begin(), inVec2.end(), std::back_inserter(result), [tempset](T t){return tempset.find(t)!=tempset.end();});
  }
  else
  {
    std::set<T> tempset(inVec2.begin(), inVec2.end());
    std::copy_if(inVec1.begin(), inVec1.end(), std::back_inserter(result), [tempset](T t){return tempset.find(t)!=tempset.end();});  
  }
  return result;
}

template <class T>
std::vector<vertex_descriptor> GetAdjacentVertices(const T &vertices, IGraph& g)
{
    std::vector<vertex_descriptor> result;
    for(auto v : vertices)
    {
        auto currentNeighbours = g.GetAdjacentVertices(v);
        result.insert(result.end(),currentNeighbours.begin(), currentNeighbours.end());
    }
    return result;
}

double GetDistance(const int x1, const int x2, const int y1, const int y2) ;

double GetDistance(const Pixel p1, const Pixel p2);


template <typename T> int signum(T val) {
    return (T(0) < val) - (val < T(0));
}

bool haveCommonEdge(IGraph& graph, vertex_descriptor v1, vertex_descriptor v2, vertex_descriptor corner);

#endif // __PRODUCTIONS_HPP__
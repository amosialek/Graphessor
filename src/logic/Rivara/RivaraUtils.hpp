#ifndef __RIVARAUTILS_HPP__
#define __RIVARAUTILS_HPP__
#include <math.h>
#include "Pixel.hpp"
#include "RivaraAttributes.hpp"
#include "mygraph.hpp"
#include "Image.hpp"

namespace Rivara
{
    inline double NL(double x1, double y1, double x2, double y2)
    {
        return std::sqrt((x2-x1)*(x2-x1)+(y2-y1)*(y2-y1));
    }

    inline double NL(Pixel p1, Pixel p2)
    {
        return NL(
            p1.attributes -> GetDouble(RIVARA_ATTRIBUTE_X),
            p1.attributes -> GetDouble(RIVARA_ATTRIBUTE_Y),
            p2.attributes -> GetDouble(RIVARA_ATTRIBUTE_X),
            p2.attributes -> GetDouble(RIVARA_ATTRIBUTE_Y));
    }

    inline double NL(double x1, double y1, double x2, double y2, double x3, double y3)
    {
        return std::sqrt(((x1+x2)/2.0-x3)*((x1+x2)/2.0-x3) + ((y1+y2)/2.0-y3)*((y1+y2)/2.0-y3));
    }

    inline double NL(Pixel p1, Pixel p2, Pixel p3)
    {
        return NL(
            p1.attributes -> GetDouble(RIVARA_ATTRIBUTE_X),
            p1.attributes -> GetDouble(RIVARA_ATTRIBUTE_Y),
            p2.attributes -> GetDouble(RIVARA_ATTRIBUTE_X),
            p2.attributes -> GetDouble(RIVARA_ATTRIBUTE_Y), 
            p3.attributes -> GetDouble(RIVARA_ATTRIBUTE_X), 
            p3.attributes -> GetDouble(RIVARA_ATTRIBUTE_Y));
    }

    inline bool EdgeComparator(const Pixel& lhs, const Pixel& rhs)
    {
        return lhs.attributes->GetDouble(RIVARA_ATTRIBUTE_L)<rhs.attributes->GetDouble(RIVARA_ATTRIBUTE_L);
    }

    inline Pixel GetNewENode(std::shared_ptr<IGraph> graph, vertex_descriptor EEdge)
    {
        Pixel newENode;
        newENode.attributes = std::make_shared<RivaraAttributes>();
        newENode.label = NODELABEL_E;
        newENode.attributes -> SetDouble(RIVARA_ATTRIBUTE_L, (*graph)[EEdge].attributes -> GetDouble(RIVARA_ATTRIBUTE_L)/2);
        newENode.attributes -> SetBool(RIVARA_ATTRIBUTE_B, (*graph)[EEdge].attributes -> GetBool(RIVARA_ATTRIBUTE_B));
        return newENode;
    }

    inline Pixel GetNewEMiddleNode(std::shared_ptr<IGraph> graph, Pixel& newNNode, vertex_descriptor lastNode)
    {
        Pixel newMiddleENode;
        newMiddleENode.attributes = std::make_shared<RivaraAttributes>();
        newMiddleENode.label = NODELABEL_E;
        newMiddleENode.attributes -> SetDouble(RIVARA_ATTRIBUTE_L, NL(newNNode, (*graph)[lastNode]));
        newMiddleENode.attributes -> SetBool(RIVARA_ATTRIBUTE_B, false);
        newMiddleENode.x = (newNNode.x+(*graph)[lastNode].x)/2;
        newMiddleENode.y = (newNNode.y+(*graph)[lastNode].y)/2;
        return newMiddleENode;
    }

    inline Pixel GetNewTNode()
    {
        Pixel newTNode;
        newTNode.attributes = std::make_shared<RivaraAttributes>();
        newTNode.label = NODELABEL_T;
        newTNode.attributes -> SetBool(RIVARA_ATTRIBUTE_R, false);
        return newTNode;
    }
    
    inline Pixel GetNewNNode(std::shared_ptr<IGraph> graph,  std::vector<vertex_descriptor> nodes, std::shared_ptr<Image> image)
    {
        Pixel newNNode;
        newNNode.attributes = std::make_shared<RivaraAttributes>();
        newNNode.attributes -> SetDouble(RIVARA_ATTRIBUTE_X, ((*graph)[nodes[0]].attributes -> GetDouble(RIVARA_ATTRIBUTE_X) + (*graph)[nodes[1]].attributes -> GetDouble(RIVARA_ATTRIBUTE_X))/2);
        newNNode.attributes -> SetDouble(RIVARA_ATTRIBUTE_Y, ((*graph)[nodes[0]].attributes -> GetDouble(RIVARA_ATTRIBUTE_Y) + (*graph)[nodes[1]].attributes -> GetDouble(RIVARA_ATTRIBUTE_Y))/2); 
        newNNode.x = newNNode.attributes -> GetDouble(RIVARA_ATTRIBUTE_X);
        newNNode.y = newNNode.attributes -> GetDouble(RIVARA_ATTRIBUTE_Y);
        std::tie(newNNode.r, newNNode.g, newNNode.b) = image->getPixel(newNNode.attributes -> GetDouble(RIVARA_ATTRIBUTE_X), newNNode.attributes -> GetDouble(RIVARA_ATTRIBUTE_Y));
        newNNode.label = NODELABEL_N;
        return newNNode;
    }

    inline bool EdgeHasNoHangingNodes(IGraph& graph, vertex_descriptor v)
    {
        auto Nnodes = graph.GetAdjacentVertices(v);
        return ( !(graph)[Nnodes[0]].attributes->GetBool(RIVARA_ATTRIBUTE_HN) 
        and  !(graph)[Nnodes[1]].attributes->GetBool(RIVARA_ATTRIBUTE_HN));
    }
    
    template<typename T>
    inline void Intersection(std::set<T>& set1, std::set<T>& set2, std::set<T>& output)
    {
        std::set_intersection(
            set1.begin(), set1.end(),
            set2.begin(), set2.end(),
            std::inserter(output)
        );
    }

    template<typename T>
    inline void Intersection(std::vector<T>& set1, std::vector<T>& set2, std::vector<T>& output)
    {
        std::sort(set1.begin(), set1.end());
        std::sort(set2.begin(), set2.end());
        std::set_intersection(
            set1.begin(), set1.end(),
            set2.begin(), set2.end(),
            std::back_inserter(output)
        );
    }

    template<typename T>
    inline void Intersection(std::set<T>& set1, std::vector<T>& set2, std::vector<T>& output)
    {
        std::sort(set2.begin(), set2.end());
        std::set_intersection(
            set1.begin(), set1.end(),
            set2.begin(), set2.end(),
            std::back_inserter(output)
        );
    }

    template<typename T>
    inline void Intersection(std::vector<T>& set1, std::set<T>& set2, std::vector<T>& output)
    {
        Intersection(set2, set1, output);
    }

    template<typename T>
    inline void RelativeComplementOfBInA(std::vector<T>& A, std::set<T>& B, std::vector<T>& output)
    {
        std::sort(A.begin(), A.end());
        std::set_difference(
        A.begin(),
        A.end(),
        B.begin(), 
        B.end(),
        std::back_inserter(output));
    }

    template<typename T>
    inline void RelativeComplementOfBInA(std::set<T>& A, std::vector<T>& B, std::vector<T>& output)
    {
        std::sort(B.begin(), B.end());
        std::set_difference(
        A.begin(),
        A.end(),
        B.begin(), 
        B.end(),
        std::back_inserter(output));
    }

    template<typename T>
    inline void RelativeComplementOfBInA(std::vector<T>& A, std::vector<T>& B, std::vector<T>& output)
    {
        std::sort(A.begin(), A.end());
        std::sort(B.begin(), B.end());
        std::set_difference(
        A.begin(),
        A.end(),
        B.begin(), 
        B.end(),
        std::back_inserter(output));
    }

    template<typename T>
    inline void RelativeComplementOfBInA(std::set<T>& A, std::set<T>& B, std::set<T>& output)
    {
        std::set_difference(
        A.begin(),
        A.end(),
        B.begin(), 
        B.end(),
        std::inserter(output));
    }
}
#endif // __RIVARAUTILS_HPP__

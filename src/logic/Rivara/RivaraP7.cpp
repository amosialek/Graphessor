#include "RivaraP7.hpp"

namespace Rivara
{
    RivaraP7::RivaraP7(std::shared_ptr<RivaraCachedGraph> graph,
            vertex_descriptor TEdge) 
            :
                graph(graph), 
                TEdge(TEdge)
        {}

    void RivaraP7::Perform()
    {
        (*graph)[TEdge].attributes->SetBool(RIVARA_ATTRIBUTE_R, true);
        graph->RegisterMarkedElement(TEdge);
    }

    std::unique_ptr<std::vector<RivaraP7>> RivaraP7::FindAllMatches(std::shared_ptr<RivaraCachedGraph> g,
        std::shared_ptr<Array2D> image,
        std::shared_ptr<Array2D> interpolation,
        int channel,
        double epsilon)
    {
        spdlog::debug("Rivara P7");
        std::unique_ptr<std::vector<RivaraP7>> result = std::make_unique<std::vector<RivaraP7>>();
        std::set<vertex_descriptor>& triangles = g -> GetNotComputedElements();
        for(auto triangle : triangles)
        {
            if((*g)[triangle].error == -1 || ((*g)[triangle].error>epsilon && !(*g)[triangle].attributes->GetBool(RIVARA_ATTRIBUTE_R)))
            {
                auto vertices = g -> GetAdjacentVertices(triangle);
                double x1 = (*g)[vertices[0]].attributes->GetDouble(RIVARA_ATTRIBUTE_X);
                double x2 = (*g)[vertices[1]].attributes->GetDouble(RIVARA_ATTRIBUTE_X);
                double x3 = (*g)[vertices[2]].attributes->GetDouble(RIVARA_ATTRIBUTE_X);
                double y1 = (*g)[vertices[0]].attributes->GetDouble(RIVARA_ATTRIBUTE_Y);
                double y2 = (*g)[vertices[1]].attributes->GetDouble(RIVARA_ATTRIBUTE_Y);
                double y3 = (*g)[vertices[2]].attributes->GetDouble(RIVARA_ATTRIBUTE_Y);
                if(abs(x1*y2+x2*y3+x3*y1-x1*y3-x2*y1-x3*y2)>16)
                {    
                    if((*g)[triangle].error == -1)
                    {
                        interpolation->BaricentricInterpolation(*image,x1,x2,x3,y1,y2,y3);
                        (*g)[triangle].error = image -> CompareWith(*interpolation, x1, x2, x3, y1, y2, y3);
                    }
                    double error = (*g)[triangle].error;
                    if(error>epsilon) //TODO: highly efficient dirty hax. remove asap
                    {
                        g->DeregisterNotComputedElement(triangle);
                        result->emplace_back(g, triangle);
                        return result;
                    }
                }
                g->DeregisterNotComputedElement(triangle);
            }
        }
        
        triangles = g -> GetCacheIterator(NODELABEL_T);
        double maxError = 0;
        std::map<vertex_descriptor, double> triangleErrors;
        
        for(auto triangle : triangles)
        {
            if((*g)[triangle].error == -1 || ((*g)[triangle].error>epsilon && !(*g)[triangle].attributes->GetBool(RIVARA_ATTRIBUTE_R)))
            {
                auto vertices = g -> GetAdjacentVertices(triangle);
                double x1 = (*g)[vertices[0]].attributes->GetDouble(RIVARA_ATTRIBUTE_X);
                double x2 = (*g)[vertices[1]].attributes->GetDouble(RIVARA_ATTRIBUTE_X);
                double x3 = (*g)[vertices[2]].attributes->GetDouble(RIVARA_ATTRIBUTE_X);
                double y1 = (*g)[vertices[0]].attributes->GetDouble(RIVARA_ATTRIBUTE_Y);
                double y2 = (*g)[vertices[1]].attributes->GetDouble(RIVARA_ATTRIBUTE_Y);
                double y3 = (*g)[vertices[2]].attributes->GetDouble(RIVARA_ATTRIBUTE_Y);
                if(abs(x1*y2+x2*y3+x3*y1-x1*y3-x2*y1-x3*y2)>4)
                {    
                    if((*g)[triangle].error == -1)
                    {
                        interpolation->BaricentricInterpolation(*image,x1,x2,x3,y1,y2,y3);
                        (*g)[triangle].error = image -> CompareWith(*interpolation, x1, x2, x3, y1, y2, y3);
                    }
                    double error = (*g)[triangle].error;
                    if(error>epsilon) //TODO: highly efficient dirty hax. remove asap
                    {
                        result->emplace_back(g, triangle);
                        return result;
                    }
                    maxError = std::max(maxError, error);
                    triangleErrors[triangle] = error;
                }
            }
        }
        if(maxError>epsilon)
        {
            for(auto triangle : triangleErrors)
            {
                if(triangle.second>0.33 * maxError)
                {
                    result->emplace_back(g, triangle.first);
                }
            }
        }
        return result; 
    }
    

}
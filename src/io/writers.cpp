#include "writers.hpp"

DebugWriter::DebugWriter(std::string fileBaseName)
{
    this->fileBaseName = fileBaseName;
};
void DebugWriter::WriteItOut(std::string fileSuffix, PixelGraph &graph) 
{
    {
        myEdgeWriter<PixelGraph> w(graph);
        std::ofstream a(fileBaseName+fileSuffix);
        boost::write_graphviz(a, graph, w);
    }
};
#include "writers.hpp"

DebugWriter::DebugWriter(std::string fileBaseName)
{
    this->fileBaseName = fileBaseName;
};
void DebugWriter::WriteItOut(std::string fileSuffix, MyGraph &graph) 
{
    {
        myEdgeWriter<MyGraph> w(graph);
        std::ofstream a(fileBaseName+fileSuffix);
        boost::write_graphviz(a, graph, w);
    }
};
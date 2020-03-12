#include <iostream>
#include "Pixel.hpp"
#include "mygraph.hpp"
#include "Image.hpp"
#include "GraphessorConstants.hpp"
#include "Productions.hpp"
#include <boost/config.hpp>
#include <boost/graph/graphviz.hpp>
#include <boost/graph/adjacency_list.hpp>
#include <boost/tuple/tuple.hpp>
#include <boost/program_options.hpp>
#include "writers.hpp"
#include "CachedGraph.hpp"
#include <chrono>

std::map<std::string, int> functionTime;

void measure_time(std::function<void()> lambda, std::string functionName)
{
    std::chrono::steady_clock::time_point begin = std::chrono::steady_clock::now();
    lambda();
    std::chrono::steady_clock::time_point end = std::chrono::steady_clock::now();
    functionTime[functionName] += std::chrono::duration_cast<std::chrono::milliseconds>(end - begin).count();
}

namespace opt=boost::program_options;

int main(int argc, char** argv) {
    auto FEdges = std::make_shared<std::vector<vertex_descriptor>>();
    auto IEdges = std::make_shared<std::vector<vertex_descriptor>>();
    auto BEdges = std::make_shared<std::vector<vertex_descriptor>>();
    opt::options_description description("Allowed options");
    description.add_options()
    ("help", "produce help message")
    ("epsilon", opt::value<double>(), "set epsilon")
    ("input", opt::value<std::string>(), "input bitmap file")
    ("output", opt::value<std::string>(), "debug output file template")
    
;

    opt::variables_map vm;
    opt::store(opt::parse_command_line(argc, argv, description), vm);
    opt::notify(vm);    
    
    double epsilon = 0.33;
    if (vm.count("epsilon"))
        epsilon = vm["epsilon"].as<double>();

    std::string inputFileName;
    if (vm.count("input"))
        inputFileName = vm["input"].as<std::string>();

    std::string outputFileName;
    if (vm.count("output"))
        outputFileName = vm["output"].as<std::string>();

    AbstractOutputWriter* debugWriter = WriterFactory::GetDebugWriter(outputFileName);
    for(int channel=0;channel<3;channel++)
    {
        auto graph = std::make_shared<CachedGraph>();
        auto S = graph -> AddVertex(*(new Pixel(0,0, NODELABEL_S)));
        auto image = std::make_shared<ImageMagnifier>(inputFileName);
        P1(graph, S, IEdges, BEdges, image).Perform();
        
        long long lastICount = 0;
        int i=1;
        debugWriter->WriteItOut(std::to_string(i++), *graph);
        while(lastICount<IEdges->size())
        {
            lastICount = IEdges->size();
            std::cerr<<"iteration: "<<i<<std::endl;
            std::chrono::steady_clock::time_point end;
            std::chrono::steady_clock::time_point begin = std::chrono::steady_clock::now();
            P5(graph, IEdges, image, channel, i < 10 ? 0 : epsilon).Perform();
            end = std::chrono::steady_clock::now();
            functionTime["P5"] += std::chrono::duration_cast<std::chrono::milliseconds>(end - begin).count();
            begin = std::chrono::steady_clock::now();
            debugWriter->WriteItOut(std::to_string(i++), *graph);
            P6(graph, IEdges).Perform();
            end = std::chrono::steady_clock::now();
            functionTime["P6"] += std::chrono::duration_cast<std::chrono::milliseconds>(end - begin).count();
            begin = std::chrono::steady_clock::now();
            debugWriter->WriteItOut(std::to_string(i++), *graph);
            auto p2s = P2::FindAllMatches(graph, IEdges, FEdges, image);
            for(auto p2:p2s)
                p2.Perform();
            end = std::chrono::steady_clock::now();
            functionTime["P2"] += std::chrono::duration_cast<std::chrono::milliseconds>(end - begin).count();
            begin = std::chrono::steady_clock::now();
            debugWriter->WriteItOut(std::to_string(i++), *graph);
            P3(graph, BEdges, image).Perform();
            end = std::chrono::steady_clock::now();
            functionTime["P3"] += std::chrono::duration_cast<std::chrono::milliseconds>(end - begin).count();
            begin = std::chrono::steady_clock::now();
            debugWriter->WriteItOut(std::to_string(i++), *graph);
            P4(graph, FEdges, image).Perform();
            end = std::chrono::steady_clock::now();
            functionTime["P4"] += std::chrono::duration_cast<std::chrono::milliseconds>(end - begin).count();
            debugWriter->WriteItOut(std::to_string(i++), *graph);
        }
        std::cerr<<"P2 "<<functionTime["P2"]<<std::endl;
        std::cerr<<"P3 "<<functionTime["P3"]<<std::endl;
        std::cerr<<"P4 "<<functionTime["P4"]<<std::endl;
        std::cerr<<"P5 "<<functionTime["P5"]<<std::endl;
        std::cerr<<"P6 "<<functionTime["P6"]<<std::endl;
    }
}



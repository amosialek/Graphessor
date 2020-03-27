#include <iostream>
#include <boost/config.hpp>
#include <boost/graph/graphviz.hpp>
#include <boost/graph/adjacency_list.hpp>
#include <boost/tuple/tuple.hpp>
#include <boost/program_options.hpp>
#include <chrono>
#include "spdlog/spdlog.h"
#include "writers.hpp"
#include "P1.hpp"
#include "P2.hpp"
#include "P3.hpp"
#include "P4.hpp"
#include "P5.hpp"
#include "P6.hpp"

std::map<std::string, int> functionTime;

namespace opt=boost::program_options;

int main(int argc, char** argv) {

    spdlog::set_pattern("[%H:%M:%S %z] [%n] [%^---%L---%$] [thread %t] %v");
    spdlog::set_level(spdlog::level::debug); // Set global log level to debug

    opt::options_description description("Allowed options");
    description.add_options()
    ("help", "produce help message")
    ("epsilon", opt::value<double>(), "set epsilon")
    ("input", opt::value<std::string>(), "input bitmap file")
    ("output", opt::value<std::string>(), "debug output file template");

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
    for(int channel=0;channel<1;channel++)
    {
        auto graph = std::make_shared<CachedGraph>();
        auto S = graph -> AddVertex(*(new Pixel(0,0, NODELABEL_S)));
        auto image = std::make_shared<ImageMagnifier>(inputFileName);
        P1(graph, S, image).Perform();
        
        unsigned long long lastICount = 0;
        int i=1;
        debugWriter->WriteItOut(std::to_string(i++), *graph);
        while(lastICount < graph -> GetCacheIterator(NODELABEL_I).size())
        {
            spdlog::debug("Starting production loop");  
            lastICount = graph -> GetCacheIterator(NODELABEL_I).size();
            std::cerr<<"iteration: "<<i<<std::endl;
            std::chrono::steady_clock::time_point end;
            std::chrono::steady_clock::time_point begin = std::chrono::steady_clock::now();
            auto p5s = P5::FindAllMatches(graph, image, channel, i < 30 ? 0 : epsilon);
            for(auto p5 : *p5s)
                p5.Perform();
            end = std::chrono::steady_clock::now();
            functionTime["P5"] += std::chrono::duration_cast<std::chrono::milliseconds>(end - begin).count();
            begin = std::chrono::steady_clock::now();
            debugWriter->WriteItOut(std::to_string(i++), *graph);
            P6::PerformAllMatches(graph);
            end = std::chrono::steady_clock::now();
            functionTime["P6"] += std::chrono::duration_cast<std::chrono::milliseconds>(end - begin).count();
            begin = std::chrono::steady_clock::now();
            debugWriter->WriteItOut(std::to_string(i++), *graph);
            auto p2s = P2::FindAllMatches(graph, image);
            for(auto p2 : *p2s)
                p2.Perform();
            end = std::chrono::steady_clock::now();
            functionTime["P2"] += std::chrono::duration_cast<std::chrono::milliseconds>(end - begin).count();
            begin = std::chrono::steady_clock::now();
            debugWriter->WriteItOut(std::to_string(i++), *graph);
            auto p3s = P3::FindAllMatches(graph, image);
            for(auto p3 : *p3s)
                p3.Perform();
            end = std::chrono::steady_clock::now();
            functionTime["P3"] += std::chrono::duration_cast<std::chrono::milliseconds>(end - begin).count();
            begin = std::chrono::steady_clock::now();
            debugWriter->WriteItOut(std::to_string(i++), *graph);
            auto p4s = P4::FindAllMatches(graph, image);
            for(auto p4: *p4s)
                p4.Perform();
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



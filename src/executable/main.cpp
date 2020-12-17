#include <iostream>
#include <experimental/filesystem> //experimental is no longer needed in gcc 8+. It is left for gcc-7 compilers
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
#include "spdlog/sinks/basic_file_sink.h"
#include "GraphImageWriter.hpp"
#include "RivaraProductions.hpp"
#include "RivaraCachedGraph.hpp"
#include "ImageMagnifier.hpp"
#include "RectangularInterpolation.hpp"

std::map<std::string, int> functionTime;

namespace opt=boost::program_options;
using namespace Rivara;



void PerformQuadTree(std::vector<std::shared_ptr<CachedGraph>>& channel_graphs,
    std::shared_ptr<Image> image,
    AbstractOutputWriter* debugWriter,
    double epsilon,
    std::shared_ptr<Array2D> interpolationArray,
    int channel,
    std::vector<std::shared_ptr<Array2D>> imageArrays,
    std::shared_ptr<spdlog::logger> logger)
{
    
    auto graph = std::make_shared<CachedGraph>();
    channel_graphs.emplace_back(graph);
    auto S = graph -> AddVertex(*(new Pixel(0,0, NODELABEL_S)));
    P1(graph, S, image).Perform();
    
    unsigned long long lastICount = 0;
    int i=1;
    //debugWriter->WriteItOut(std::to_string(i++), *graph);
    while(i<10 && lastICount < graph -> GetCacheIterator(NODELABEL_I).size())
    {

        spdlog::debug("Starting production loop, channel={}, i={}",channel,i);  
        lastICount = graph -> GetCacheIterator(NODELABEL_I).size();
        if(i%50==0)
        std::cerr<<"iteration: "<<i<<std::endl;
        std::chrono::steady_clock::time_point end;
        std::chrono::steady_clock::time_point begin = std::chrono::steady_clock::now();
        auto p5s = P5::FindAllMatches(graph, imageArrays[channel], interpolationArray, channel, i < 2 ? 0 : epsilon);
        for(auto p5 : *p5s)
            p5.Perform();
        end = std::chrono::steady_clock::now();
        functionTime["P5"] += std::chrono::duration_cast<std::chrono::milliseconds>(end - begin).count();
        begin = std::chrono::steady_clock::now();
        //debugWriter->WriteItOut(std::to_string(i++), *graph);
        P6::PerformAllMatches(graph);
        end = std::chrono::steady_clock::now();
        functionTime["P6"] += std::chrono::duration_cast<std::chrono::milliseconds>(end - begin).count();
        begin = std::chrono::steady_clock::now();
        //debugWriter->WriteItOut(std::to_string(i++), *graph);
        auto p2s = P2::FindAllMatches(graph, image);
        for(auto p2 : *p2s)
            p2.Perform();
        end = std::chrono::steady_clock::now();
        functionTime["P2"] += std::chrono::duration_cast<std::chrono::milliseconds>(end - begin).count();
        begin = std::chrono::steady_clock::now();
        //debugWriter->WriteItOut(std::to_string(i++), *graph);
        auto p3s = P3::FindAllMatches(graph, image);
        for(auto p3 : *p3s)
            p3.Perform();
        end = std::chrono::steady_clock::now();
        functionTime["P3"] += std::chrono::duration_cast<std::chrono::milliseconds>(end - begin).count();
        begin = std::chrono::steady_clock::now();
        //debugWriter->WriteItOut(std::to_string(i++), *graph);
        auto p4s = P4::FindAllMatches(graph, image);
        for(auto p4: *p4s)
            p4.Perform();
        end = std::chrono::steady_clock::now();
        functionTime["P4"] += std::chrono::duration_cast<std::chrono::milliseconds>(end - begin).count();
        //debugWriter->WriteItOut(std::to_string(i++), *graph);
        i++;
    }
    std::cerr<<"P2 "<<functionTime["P2"]<<std::endl;
    std::cerr<<"P3 "<<functionTime["P3"]<<std::endl;
    std::cerr<<"P4 "<<functionTime["P4"]<<std::endl;
    std::cerr<<"P5 "<<functionTime["P5"]<<std::endl;
    std::cerr<<"P6 "<<functionTime["P6"]<<std::endl;
}

void PerformRivara(std::vector<std::shared_ptr<CachedGraph>>& channel_graphs,
    std::shared_ptr<Image> image,
    AbstractOutputWriter* debugWriter,
    double epsilon,
    std::shared_ptr<spdlog::logger> logger)
{
    for(int channel=0;channel<1;channel++)
    {
        int i=0;
        auto graph = std::make_shared<RivaraCachedGraph>();
        channel_graphs.emplace_back(graph);
        auto SPixel = new Pixel(0,0, NODELABEL_S);
        SPixel->attributes = std::make_unique<RivaraAttributes>();
        auto S = graph -> AddVertex(*(SPixel));
        (*graph)[S].attributes = std::make_shared<RivaraAttributes>();
        RivaraP0(graph, S, image).Perform();
        //debugWriter->WriteItOut(std::to_string(i++), *graph);

        i=1;
        unsigned long long lastICount = 0;
        //debugWriter->WriteItOut(std::to_string(i++), *graph);
        std::vector<std::shared_ptr<Array2D>> imageArrays = image->GetChannelsAsArrays();
        while(lastICount < graph -> GetCacheIterator(NODELABEL_T).size())
        {
            std::shared_ptr<Array2D> interpolationArray = std::make_shared<Array2D>(imageArrays[0]->width, imageArrays[0]->height);

            spdlog::debug("Starting production loop, channel={}, i={}",channel,i);  
            lastICount = graph -> GetCacheIterator(NODELABEL_T).size();
            if(i%50==0)
                std::cerr<<"iteration: "<<i<<std::endl;
            std::chrono::steady_clock::time_point end;
            std::chrono::steady_clock::time_point begin = std::chrono::steady_clock::now();
            auto p7s = RivaraP7::FindAllMatches(graph, imageArrays[channel], interpolationArray, channel, i < 10 ? 0 : epsilon);
            for(auto p7 : *p7s)
            {
                p7.Perform();
            }
            end = std::chrono::steady_clock::now();
            functionTime["P7"] += std::chrono::duration_cast<std::chrono::milliseconds>(end - begin).count();
            //std::cerr<<"iteration: "<<i<<std::endl;
            //debugWriter->WriteItOut(std::to_string(i++), *graph);
            begin = std::chrono::steady_clock::now();
            auto p1s = RivaraP1::FindAllMatches(graph, image);
            if(p1s->size()>0)
                p1s -> front().Perform();
            end = std::chrono::steady_clock::now();
            functionTime["P1"] += std::chrono::duration_cast<std::chrono::milliseconds>(end - begin).count();
            //std::cerr<<"iteration: "<<i<<std::endl;
            //debugWriter->WriteItOut(std::to_string(i++), *graph);
            i++;
            int productionsPerformed = 1;
            while(productionsPerformed>0)
            {
                logger->flush();
                //debugWriter->WriteItOut(std::to_string(i++), *graph);
                begin = std::chrono::steady_clock::now();
                auto p2s = RivaraP2::FindAllMatches(graph);
                spdlog::debug("found {} p2s", p2s -> size());
                for(auto p2 : *p2s)
                {
                    p2.Perform();
                    logger->flush();
                }
                end = std::chrono::steady_clock::now();
                functionTime["P2"] += std::chrono::duration_cast<std::chrono::milliseconds>(end - begin).count();
                //std::cerr<<"iteration: "<<i<<std::endl;
                logger->flush();
                //debugWriter->WriteItOut(std::to_string(i++), *graph);
                begin = std::chrono::steady_clock::now();
                auto p3s = RivaraP3::FindAllMatches(graph, image);
                spdlog::debug("found {} p3s", p3s -> size());
                for(auto p3 : *p3s)
                {
                    p3.Perform();      
                    logger->flush();
                }   
                end = std::chrono::steady_clock::now();
                functionTime["P3"] += std::chrono::duration_cast<std::chrono::milliseconds>(end - begin).count();
                //std::cerr<<"iteration: "<<i<<std::endl;

                begin = std::chrono::steady_clock::now();
                //debugWriter->WriteItOut(std::to_string(i++), *graph);
                auto p4s = RivaraP4::FindAllMatches(graph);
                for(auto p4: *p4s)
                    p4.Perform();
                end = std::chrono::steady_clock::now();
                functionTime["P4"] += std::chrono::duration_cast<std::chrono::milliseconds>(end - begin).count();
                //std::cerr<<"iteration: "<<i<<std::endl;

                begin = std::chrono::steady_clock::now();
                //debugWriter->WriteItOut(std::to_string(i++), *graph);
                auto p5s = RivaraP4::FindAllMatches(graph);
                for(auto p5: *p5s)
                    p5.Perform();
                end = std::chrono::steady_clock::now();
                functionTime["P5"] += std::chrono::duration_cast<std::chrono::milliseconds>(end - begin).count();
                //std::cerr<<"iteration: "<<i<<std::endl;

                begin = std::chrono::steady_clock::now();
                //debugWriter->WriteItOut(std::to_string(i++), *graph);
                auto p6s = RivaraP4::FindAllMatches(graph);
                for(auto p6: *p6s)
                    p6.Perform();
                end = std::chrono::steady_clock::now();
                functionTime["P6"] += std::chrono::duration_cast<std::chrono::milliseconds>(end - begin).count();
                //std::cerr<<"iteration: "<<i<<std::endl;

                productionsPerformed = p2s->size()+p3s->size()+p4s->size()+p5s->size()+p6s->size();
            }
        }
        std::cerr<<"P1 "<<functionTime["P1"]<<std::endl;
        std::cerr<<"P2 "<<functionTime["P2"]<<std::endl;
        std::cerr<<"P3 "<<functionTime["P3"]<<std::endl;
        std::cerr<<"P4 "<<functionTime["P4"]<<std::endl;
        std::cerr<<"P5 "<<functionTime["P5"]<<std::endl;
        std::cerr<<"P6 "<<functionTime["P6"]<<std::endl;
        std::cerr<<"P7 "<<functionTime["P7"]<<std::endl;
    }
}

void Deserialize(AbstractOutputWriter* debugWriter)
{
    std::ifstream serializedGraphFileStream("/media/albert/Nowy/poligon/test.txt");
    auto graph = std::make_shared<RivaraCachedGraph>();
    graph->Deserialize(serializedGraphFileStream);
    debugWriter->WriteItOut("test", *graph);
}

int main(int argc, char** argv) {

    spdlog::set_pattern("[%H:%M:%S %z] [%n] [%^---%L---%$] [thread %t] %v");
    spdlog::set_level(spdlog::level::debug); // Set global log level to debug
    bool isRivara;

    opt::options_description description("Allowed options");
    description.add_options()
    ("help", "produce help message")
    ("epsilon", opt::value<double>()->default_value(0.01), "set epsilon")
    ("input", opt::value<std::string>(), "input bitmap file")
    ("graph-output", opt::value<std::string>(), "debug output file template")
    ("output-directory", opt::value<std::string>(), "directory for output, debug output and log file")
    ("output-file-template", opt::value<std::string>()->default_value("output"), "output file template")
    ("log-file", opt::value<std::string>()->default_value("log.txt"), "log file name")
    ("rivara", opt::bool_switch(&isRivara), "use rivara productions instead of quadtrees");

    opt::variables_map vm;
    opt::store(opt::parse_command_line(argc, argv, description), vm);
    opt::notify(vm);    
    double epsilon = 0.33;
    if (vm.count("epsilon"))
        epsilon = vm["epsilon"].as<double>();

    std::string inputFileName;
    if (vm.count("input"))
        inputFileName = vm["input"].as<std::string>();
    if(!std::experimental::filesystem::exists(inputFileName))
    {    
        std::cerr<<"Input file '"<<inputFileName<<"' doesn't exist"<<std::endl;
        return 1;
    }
    std::string graphOutputFileName;
    if (vm.count("graph-output"))
        graphOutputFileName = vm["graph-output"].as<std::string>();

    std::string outputFileName;
    if (vm.count("output-file-template"))
        outputFileName = vm["output-file-template"].as<std::string>();

    std::string logFileName;
    if (vm.count("log-file"))
        logFileName = vm["log-file"].as<std::string>();

    std::string outputDirectory;
    if (vm.count("output-directory"))
        outputDirectory = vm["output-directory"].as<std::string>();

    if(outputDirectory!="")
        std::experimental::filesystem::create_directories(outputDirectory);
    std::experimental::filesystem::path outputDirectoryPath = outputDirectory;

    auto file_logger = spdlog::basic_logger_mt("basic_logger", (outputDirectoryPath/logFileName).c_str());
    spdlog::set_default_logger(file_logger); 

    for(int i=0;i<argc;i++)
    {
        spdlog::debug("P1 {}",argv[i]);    
    }

    CachedGraph g;
    std::ifstream s("/media/albert/Nowy/Albert/agh/doktorat/outputs/MPaszynski1/2020_12_17/1/serializedGraph");
    g.Deserialize(s);
    s.close();
    

    AbstractOutputWriter* debugWriter = WriterFactory::GetDebugWriter(graphOutputFileName);
    std::vector<std::shared_ptr<CachedGraph>> channel_graphs;
    auto image = std::make_shared<ImageMagnifier>(inputFileName);
    auto image3 = std::make_shared<Image>(inputFileName);
    //image -> Save3Colors("/media/albert/Nowy/poligon/bunny_orig");
    if(isRivara)
        PerformRivara(channel_graphs, image, debugWriter, epsilon, file_logger);
    else
    {
        std::vector<std::shared_ptr<Array2D>> vectorsForImage;
        std::vector<std::shared_ptr<Array2D>> imageArrays = image3->GetChannelsAsArrays();
        std::shared_ptr<Array2D> interpolationArray2 = std::make_shared<Array2D>(image3->width(), image3->height());
        RectangularInterpolation2(imageArrays[0], interpolationArray2, std::make_shared<CachedGraph>(g));
        std::vector<std::shared_ptr<Array2D>> vectorsForImage2;
        vectorsForImage2.push_back(std::make_shared<Array2D>(image3->width(), image3->height()));
        vectorsForImage2.push_back(interpolationArray2);
        vectorsForImage2.push_back(std::make_shared<Array2D>(image3->width(), image3->height()));
        Image interpolationImage2 = Image(vectorsForImage2);
        interpolationImage2.save((outputDirectoryPath/(outputFileName+"_interpolation3.bmp")).c_str());
        for(int channel=0;channel<3;channel++)
        {
            std::shared_ptr<Array2D> interpolationArray = std::make_shared<Array2D>(image->width(), image->height());
            PerformQuadTree(channel_graphs, image, debugWriter, epsilon, interpolationArray, channel, imageArrays, file_logger);
            vectorsForImage.push_back(interpolationArray);
        }
        //std::shared_ptr<Array2D> interpolationArray2 = std::make_shared<Array2D>(image->width(), image->height());
        
        //RectangularInterpolation2(imageArrays[0], interpolationArray2, channel_graphs[0]);
        //std::vector<std::shared_ptr<Array2D>> vectorsForImage2;
        //vectorsForImage2.push_back(std::make_shared<Array2D>(image->width(), image->height()));
        //vectorsForImage2.push_back(interpolationArray2);
        //vectorsForImage2.push_back(std::make_shared<Array2D>(image->width(), image->height()));

        Image interpolationImage = Image(vectorsForImage);
        //Image interpolationImage2 = Image(vectorsForImage2);
        interpolationImage.save((outputDirectoryPath/(outputFileName+"_interpolation.bmp")).c_str());
        //interpolationImage2.save((outputDirectoryPath/(outputFileName+"_interpolation2.bmp")).c_str());
        
    }
    //Deserialize(debugWriter);
    
    for(auto g : channel_graphs)
        g->DecreaseXAndYByRatio(4);

    std::ofstream graphOutput("/home/albert/Albert/agh/doktorat/outputs/MPaszynski1/2020_12_10/1/serializedGraph");
    channel_graphs[0]->Serialize(graphOutput);
    graphOutput.close();

    auto restoredImage = std::make_unique<Image>(channel_graphs);
    GraphImageWriter::DrawPixels(channel_graphs[0],(outputDirectoryPath/(outputFileName+"_red_graph.bmp")).c_str());
    // GraphImageWriter::DrawPixels(channel_graphs[1],(outputDirectoryPath/(outputFileName+"_green_graph.bmp")).c_str());
    // GraphImageWriter::DrawPixels(channel_graphs[2],(outputDirectoryPath/(outputFileName+"_blue_graph.bmp")).c_str());
    auto image2 = std::make_shared<Image>(inputFileName);
    restoredImage -> save((outputDirectoryPath/(outputFileName+".bmp")).c_str());
    restoredImage -> Save3Colors((outputDirectoryPath/outputFileName).c_str());
    double PSNR = restoredImage -> PSNR(image2.get());
    std::cout<<"PSNR: "<<PSNR<<std::endl; 
    spdlog::debug("PSNR = {}",PSNR);  
}
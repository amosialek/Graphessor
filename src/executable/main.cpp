#include <iostream>
#include <experimental/filesystem> //experimental is no longer needed in gcc 8+. It is left for gcc-7 compilers
#include <boost/config.hpp>
#include <boost/graph/graphviz.hpp>
#include <boost/graph/adjacency_list.hpp>
#include <boost/tuple/tuple.hpp>
#include <boost/program_options.hpp>
#include <chrono>
#include "spdlog/spdlog.h"
#include "Rectangular.hpp"
#include "spdlog/sinks/basic_file_sink.h"
#include "GraphImageWriter.hpp"
#include "Rivara.hpp"
#include "ImageMagnifier.hpp"
#include "RectangularInterpolation.hpp"

std::map<std::string, int> functionTime;

namespace opt=boost::program_options;
using namespace Rivara;
std::experimental::filesystem::path outputDirectoryPath;
std::string outputFileName;
std::shared_ptr<Image> image3;
std::vector<std::shared_ptr<Array2D>> imageArrays3;

double countL2Error(Array2D& interpolationArray, Array2D& original)
{
     auto interpolationCopy = interpolationArray.GetCopy();
     interpolationCopy.Subtract(original);
     
     double numerator = interpolationCopy.MultiplyElementWiseAndSum(interpolationCopy, 0, interpolationCopy.width-1, 0, interpolationCopy.height-1);
     double denumerator = original.MultiplyElementWiseAndSum(original, 0, original.width-1, 0, original.height-1);
     if(numerator==0 and denumerator==0)
        denumerator = 1;
     spdlog::debug("L2 error = {}", sqrt(numerator/denumerator));
     return numerator/denumerator;
}

double countL2Error(std::shared_ptr<Array2D> interpolationArray, std::vector<std::shared_ptr<Array2D>> imageArrays, std::vector<std::shared_ptr<CachedGraph>>& channel_graphs)
{
    std::shared_ptr<CachedGraph> g = std::make_shared<CachedGraph>(*channel_graphs[0]);

    g->DecreaseXAndYByRatio(4);
    std::shared_ptr<Array2D> interpolationArray2 = std::make_shared<Array2D>(interpolationArray->width/4, interpolationArray->height/4);
    
    RectangularInterpolation(imageArrays3[0], interpolationArray2, g);

    double L2 = imageArrays3[0] -> SquaredError(*interpolationArray2,0,imageArrays3[0]->width-1, 0, imageArrays3[0]->height-1);
    L2=L2/(imageArrays3[0]->width* imageArrays3[0]->height*255.0*255.0);    
    std::vector<std::shared_ptr<Array2D>> vectorsForImage;
    interpolationArray2 = std::make_shared<Array2D>(interpolationArray->width/4, interpolationArray->height/4);
    vectorsForImage.push_back(interpolationArray2);
    vectorsForImage.push_back(std::make_shared<Array2D>(interpolationArray2 -> width, interpolationArray2 -> height));
    vectorsForImage.push_back(std::make_shared<Array2D>(interpolationArray2 -> width, interpolationArray2 -> height));
    Image interpolationImage = Image(vectorsForImage);
    g = std::make_shared<CachedGraph>(*channel_graphs[0]);
    g->DecreaseXAndYByRatio(4);
    
    RectangularInterpolation(imageArrays3[0], interpolationArray2, g);

    L2 = imageArrays3[0] -> SquaredError(*interpolationArray2,0,imageArrays3[0]->width-1, 0, imageArrays3[0]->height-1);
    L2=L2 / (imageArrays3[0] -> width * imageArrays3[0] -> height * 255.0 * 255.0);
    double L2original = imageArrays[0] -> SquaredError(*interpolationArray,0,imageArrays[0]->width-1,0,imageArrays[0]->height-1)/255.0/255.0/imageArrays[0]->height/imageArrays[0]->width;
    vectorsForImage.clear();
    spdlog::debug("interpolated L2 error = {}",  L2); 
    spdlog::debug("original L2 error = {}",  L2original);  
    vectorsForImage.push_back(interpolationArray2);
    vectorsForImage.push_back(std::make_shared<Array2D>(interpolationArray2 -> width, interpolationArray2 -> height));
    vectorsForImage.push_back(std::make_shared<Array2D>(interpolationArray2 -> width, interpolationArray2 -> height));
    interpolationImage = Image(vectorsForImage);
    interpolationImage.save((outputDirectoryPath/(outputFileName+"_L2_interpolation.bmp")).c_str());
    GraphImageWriter::DrawPixels(g, (outputDirectoryPath/(outputFileName+"_L2_graph.bmp")).c_str());

    return L2;
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
    ("rivara", opt::bool_switch(&isRivara), "use rivara productions instead of quadtrees")
    ("order", opt::value<int>(), "test function max order");

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

    
    if (vm.count("output-file-template"))
        outputFileName = vm["output-file-template"].as<std::string>();

    std::string logFileName;
    if (vm.count("log-file"))
        logFileName = vm["log-file"].as<std::string>();

    int order;
    if (vm.count("order"))
        order = vm["order"].as<int>();


    std::string outputDirectory;
    if (vm.count("output-directory"))
        outputDirectory = vm["output-directory"].as<std::string>();

    if(outputDirectory!="")
        std::experimental::filesystem::create_directories(outputDirectory);
    outputDirectoryPath = outputDirectory;

    auto file_logger = spdlog::basic_logger_mt("basic_logger", (outputDirectoryPath/logFileName).c_str());
    spdlog::set_default_logger(file_logger); 

    for(int i=0;i<argc;i++)
    {
        spdlog::debug("args: {}",argv[i]);    
    }

    AbstractOutputWriter* debugWriter = WriterFactory::GetDebugWriter(graphOutputFileName);
    std::vector<std::shared_ptr<CachedGraph>> channel_graphs;
    auto image = std::make_shared<ImageMagnifier>(inputFileName);
    if(isRivara)
        PerformRivara(channel_graphs, image, debugWriter, epsilon, file_logger);
    else
    {
        std::vector<std::shared_ptr<Array2D>> vectorsForImage;
        std::vector<std::shared_ptr<Array2D>> imageArrays = image->GetChannelsAsArrays();
        for(int channel=0;channel<3;channel++)
        {
            std::shared_ptr<Array2D> interpolationArray = std::make_shared<Array2D>(image->width(), image->height());
            PerformQuadTree(channel_graphs, image, debugWriter, epsilon, interpolationArray, channel, imageArrays, file_logger, order);
            interpolationArray->Apply([](double value){return value<0 ? 0 : value;});
            vectorsForImage.push_back(interpolationArray);
        }
        Image interpolationImage = Image(vectorsForImage);
        interpolationImage.save((outputDirectoryPath/(outputFileName+"_interpolation.bmp")).c_str());   
    }
    for(auto g : channel_graphs)
        g->DecreaseXAndYByRatio(4);
    for(int channel=0;channel<3;channel++)
        for(auto IEdge : channel_graphs[channel]->GetCacheIterator(NODELABEL_I))
        {
            spdlog::debug("channel={} IEdge={} x={} y={}",channel, IEdge, (*channel_graphs[channel])[IEdge].x, (*channel_graphs[channel])[IEdge].y);
        }
    std::ofstream graphOutput(outputDirectory+"/serializedGraph");
    channel_graphs[0]->Serialize(graphOutput);
    graphOutput.close();

    auto restoredImage = std::make_unique<Image>(channel_graphs);
    GraphImageWriter::DrawPixels(channel_graphs[0],(outputDirectoryPath/(outputFileName+"_red_graph.bmp")).c_str());
    GraphImageWriter::DrawFullGraph(channel_graphs[0],(outputDirectoryPath/(outputFileName+"_red_full_graph.bmp")).c_str());
    // GraphImageWriter::DrawPixels(channel_graphs[1],(outputDirectoryPath/(outputFileName+"_green_graph.bmp")).c_str());
    // GraphImageWriter::DrawPixels(channel_graphs[2],(outputDirectoryPath/(outputFileName+"_blue_graph.bmp")).c_str());
    auto image2 = std::make_shared<Image>(inputFileName);
    restoredImage -> save((outputDirectoryPath/(outputFileName+".bmp")).c_str());
    restoredImage -> Save3Colors((outputDirectoryPath/outputFileName).c_str());
    double PSNR = restoredImage -> PSNR(image2.get());
    std::cout<<"PSNR: "<<PSNR<<std::endl; 
    spdlog::debug("PSNR = {}",PSNR);  
}
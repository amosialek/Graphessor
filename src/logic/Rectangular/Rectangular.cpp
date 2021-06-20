#include "Rectangular.hpp"

void PerformQuadTree(std::vector<std::shared_ptr<CachedGraph>>& channel_graphs,
    std::shared_ptr<Image> image,
    AbstractOutputWriter* debugWriter,
    double epsilon,
    std::shared_ptr<Array2D> interpolationArray,
    int channel,
    std::vector<std::shared_ptr<Array2D>> imageArrays,
    std::shared_ptr<spdlog::logger> logger,
    int order)
{
    std::map<std::string, int> functionTime;
    auto graph = std::make_shared<CachedGraph>();
    channel_graphs.emplace_back(graph);
    auto S = graph -> AddVertex(*(new Pixel(0,0, NODELABEL_S)));
    P1(graph, S, image).Perform();

    unsigned long long lastICount = 0;
    int i=1;
    //debugWriter->WriteItOut(std::to_string(i++), *graph);
    while(lastICount < graph -> GetCacheIterator(NODELABEL_I).size() and i<11)
    {

        spdlog::debug("Starting production loop, channel={}, i={}",channel,i);  
        lastICount = graph -> GetCacheIterator(NODELABEL_I).size();
        if(i%50==0)
        std::cerr<<"iteration: "<<i<<std::endl;
        std::chrono::steady_clock::time_point end;
        std::chrono::steady_clock::time_point begin = std::chrono::steady_clock::now();
        auto p5s = P5::FindAllMatches(graph, imageArrays[channel], interpolationArray, channel, i < 2 ? 0 : epsilon, order);
        if(channel==0 and i==4)debugWriter->Write("p5_failing", *graph);
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


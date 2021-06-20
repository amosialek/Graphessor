#include "Rivara.hpp"

using namespace Rivara;

void PerformRivara(std::vector<std::shared_ptr<CachedGraph>>& channel_graphs,
    std::shared_ptr<Image> image,
    AbstractOutputWriter* debugWriter,
    double epsilon,
    std::shared_ptr<spdlog::logger> logger)
{
    std::map<std::string, int> functionTime;
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

        i=1;
        unsigned long long lastICount = 0;
        std::vector<std::shared_ptr<Array2D>> imageArrays = image->GetChannelsAsArrays();
        while(lastICount < graph -> GetCacheIterator(NODELABEL_T).size())
        {
            std::shared_ptr<Array2D> interpolationArray = std::make_shared<Array2D>(imageArrays[0]->width, imageArrays[0]->height);

            spdlog::debug("Starting production loop, channel={}, i={}",channel,i);  
            lastICount = graph -> GetCacheIterator(NODELABEL_T).size();
            std::chrono::steady_clock::time_point end;
            std::chrono::steady_clock::time_point begin = std::chrono::steady_clock::now();
            auto p7s = RivaraP7::FindAllMatches(graph, imageArrays[channel], interpolationArray, channel, i < 10 ? 0 : epsilon);
            for(auto p7 : *p7s)
            {
                p7.Perform();
            }
            end = std::chrono::steady_clock::now();
            functionTime["P7"] += std::chrono::duration_cast<std::chrono::milliseconds>(end - begin).count();
            begin = std::chrono::steady_clock::now();
            auto p1s = RivaraP1::FindAllMatches(graph, image);
            if(p1s->size()>0)
                p1s -> front().Perform();
            end = std::chrono::steady_clock::now();
            functionTime["P1"] += std::chrono::duration_cast<std::chrono::milliseconds>(end - begin).count();
            i++;
            int productionsPerformed = 1;
            while(productionsPerformed>0)
            {
                logger->flush();
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
                logger->flush();
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
            
                begin = std::chrono::steady_clock::now();
                auto p4s = RivaraP4::FindAllMatches(graph);
                for(auto p4: *p4s)
                    p4.Perform();
                end = std::chrono::steady_clock::now();
                functionTime["P4"] += std::chrono::duration_cast<std::chrono::milliseconds>(end - begin).count();
            
                begin = std::chrono::steady_clock::now();
                auto p5s = RivaraP5::FindAllMatches(graph,image);
                for(auto p5: *p5s)
                    p5.Perform();
                end = std::chrono::steady_clock::now();
                functionTime["P5"] += std::chrono::duration_cast<std::chrono::milliseconds>(end - begin).count();
            
                begin = std::chrono::steady_clock::now();
                auto p6s = RivaraP6::FindAllMatches(graph);
                for(auto p6: *p6s)
                    p6.Perform();
                end = std::chrono::steady_clock::now();
                functionTime["P6"] += std::chrono::duration_cast<std::chrono::milliseconds>(end - begin).count();
            
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
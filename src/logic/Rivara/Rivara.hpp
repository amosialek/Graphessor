#ifndef RIVARA_HPP
#define RIVARA_HPP
#include "RivaraProductions.hpp"
#include "RivaraCachedGraph.hpp"
#include "RivaraAttributes.hpp"
#include "writers.hpp"

void PerformRivara(std::vector<std::shared_ptr<CachedGraph>>& channel_graphs,
    std::shared_ptr<Image> image,
    AbstractOutputWriter* debugWriter,
    double epsilon,
    std::shared_ptr<spdlog::logger> logger);


#endif /* RIVARA_HPP */

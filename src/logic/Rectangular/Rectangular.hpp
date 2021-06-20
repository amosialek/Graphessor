#ifndef RECTANGULAR_HPP
#define RECTANGULAR_HPP

#include "writers.hpp"
#include "P1.hpp"
#include "P2.hpp"
#include "P3.hpp"
#include "P4.hpp"
#include "P5.hpp"
#include "P6.hpp"


void PerformQuadTree(std::vector<std::shared_ptr<CachedGraph>>& channel_graphs,
    std::shared_ptr<Image> image,
    AbstractOutputWriter* debugWriter,
    double epsilon,
    std::shared_ptr<Array2D> interpolationArray,
    int channel,
    std::vector<std::shared_ptr<Array2D>> imageArrays,
    std::shared_ptr<spdlog::logger> logger,
    int order);


#endif /* RECTANGULAR_HPP */

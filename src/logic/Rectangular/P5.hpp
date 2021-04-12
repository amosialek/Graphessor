#ifndef P5_HPP
#define P5_HPP

#include "Productions.hpp"
#include "Array2D.hpp"

class P5 : public Production{
    private:
        std::shared_ptr<IGraph> graph;
        vertex_descriptor IEdge;
    public:
        P5(std::shared_ptr<IGraph> graph,
            vertex_descriptor IEdge);
        void Perform() override;
        std::vector<uint8_t> Serialize() override;
        static std::unique_ptr<std::vector<P5>> FindAllMatches(std::shared_ptr<CachedGraph> graph,
            std::shared_ptr<Array2D> image,
            std::shared_ptr<Array2D> interpolation,
            int channel,
            double epsilon,
            int orders);

};
/**
 * Resets interpolation on (minx, miny) - (maxx, maxy) rectangle. 
 * If bool flag of the wall is set to true, then the wall is split to two parts.
 * I.e. topWallSplit set to true, so instead of normal interpolation of whole rectangle, it will be split to (minx, miny)-((minx+maxx)/2, maxy) and (((minx+maxx)/2, miny)-(maxx, maxy)
 * @param image original bitmap to be interpolated
 * @param interpolation bitmap which will be filled during interpolation in given rectangle
 * @param minx left x coordinate of rectangle (minx<maxx)
 * @param maxx right x coordinate of rectangle (minx<maxx)
 * @param miny top y coordinate of rectangle (miny<maxy)
 * @param maxy bottom y coordinate of rectangle (miny<maxy)
 * @param topWallSplit indicates the top wall (y==miny) should be split
 * @param bottomWallSplit indicates the bottom wall (y==maxy) should be split
 * @param rightWallSplit indicates the left wall (x==minx) should be split
 * @param leftWallSplit indicates the right wall (x==maxx) should be split
 */
void HalfEdgesBilinearInterpolation(
    Array2D& image, 
    Array2D& interpolation, 
    int minx, 
    int maxx, 
    int miny, 
    int maxy, 
    bool topWallSplit, 
    bool bottomWallSplit, 
    bool rightWallSplit, 
    bool leftWallSplit); 
#endif /* P5_HPP */

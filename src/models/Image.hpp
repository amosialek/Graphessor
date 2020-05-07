#ifndef __IMAGE_HPP__
#define __IMAGE_HPP__

#include <string>
#include <boost/gil/extension/io/bmp.hpp>
#include "CachedGraph.hpp"


class Image
{
    private:
        std::tuple<int, int, int> getPixelInternal(int x, int y);
        int getPixelInternal(int x, int y, int channel);
        void FillMissingSpacesBasedOnLargerBlocks(std::shared_ptr<IGraph> graph, std::set<vertex_descriptor>& pixels, int channel, int width);
        // void FillMissingSpacesBasedOnBaricentricInterpolation(std::vector<Pixel> setOfPixels, int channel, int width);
        int sign2 (int x1, int y1, int x2, int y2, int x3, int y3);
        bool PointInTriangle (int px, int py, int x1, int y1, int x2, int y2, int x3, int y3);
        double GetInterpolatedPixel(int x1, int x2, int x3, int y1, int y2, int y3, int x, int y, double wDenominator, int channel);


    protected:
        boost::gil::rgb8_image_t img;
        boost::gil::rgb8_view_t view;

    public:    
        void XYZ(int channel, int width, std::set<vertex_descriptor> pixels, std::shared_ptr<CachedGraph> graph, boost::gil::rgb8_image_t img, vertex_descriptor v);
        int GetRGBChannelValue(Pixel p, int channel);
        virtual double SquaredErrorOfInterpolation(int xx1, int xx2, int yy1, int yy2, int channel);
        virtual double SquaredErrorOfInterpolation(int x1, int x2, int x3, int y1, int y2, int y3, int channel);
        virtual double GetInterpolatedPixel(int x1, int x2, int y1, int y2, int x, int y, int channel);
        Image(std::string filename);
        Image(uint8_t*** pixels, int width, int height, int channels);
        Image(int width, int height);
        Image(std::vector<std::shared_ptr<CachedGraph>> graphs);
        long long CompareWith(Image& other);
        virtual long long CompareWith(Image& other, int x, int y, int width, int height);
        virtual double CompareWithInterpolation(int xx1, int xx2, int yy1, int yy2, int channel);
        virtual double CompareWithInterpolation(int x1, int x2, int x3, int y1, int y2, int y3, int channel);
        virtual std::tuple<int, int, int> getPixel(int x, int y);
        virtual int width();
        virtual int height();
        virtual void save(std::string filename);
        static int const RED_CHANNEL=0;
        static int const GREEN_CHANNEL=1;
        static int const BLUE_CHANNEL=2;
        virtual void Save3Colors(std::string filename);
        virtual std::tuple<int,int> GetNearestPixelCoords(int x, int y);
        virtual void SetPixel(int x, int y, int channel, int value);
        virtual void SetPixelSafe(int x, int y, int channel, int value);
        virtual void DrawLine(int x1, int y1, int x2, int y2, int channel, int color);
        virtual void DrawBlackLine(int x1, int y1, int x2, int y2);
        void BilinearInterpolation(int channel, std::vector<Pixel> pixels);
        void BaricentricInterpolation(int channel, std::vector<Pixel> pixels);
        void SVDInterpolation(int channel, std::vector<Pixel> pixels);
        void Asdf(int channel, int width, std::shared_ptr<CachedGraph> graph);
        void SetSquare(int x, int y, int channel, int value, int width);      
};

class ImageMagnifier : public Image
{
    private:
        int ratio = 4;
    public: 
        ImageMagnifier(std::string filename) : Image(filename){};
        ImageMagnifier(std::vector<std::shared_ptr<CachedGraph>> graphs) : Image(graphs){}
        ImageMagnifier(uint8_t*** pixels, int width, int height, int channels) : Image(pixels, width, height, channels){}
        double SquaredErrorOfInterpolation(int xx1, int xx2, int yy1, int yy2, int channel) override;
        double SquaredErrorOfInterpolation(int x1, int x2, int x3, int y1, int y2, int y3, int channel) override;
        double GetInterpolatedPixel(int x1, int x2, int y1, int y2, int x, int y, int channel) override;
        long long CompareWith(Image& other, int x, int y, int width, int height) override;
        double CompareWithInterpolation(int xx1, int xx2, int yy1, int yy2, int channel) override;
        double CompareWithInterpolation(int x1, int x2, int x3, int y1, int y2, int y3, int channel) override;
        std::tuple<int, int, int> getPixel(int x, int y) override;
        int width() override;
        int height() override;
        std::tuple<int,int> GetNearestPixelCoords(int x, int y) override;
        void SetPixel(int x, int y, int channel, int value) override;
};
#endif // __IMAGE_HPP__
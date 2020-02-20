#ifndef __IMAGE_HPP__
#define __IMAGE_HPP__

#include <string>
#include <boost/gil/extension/io/bmp.hpp>



class Image
{

    private:
        boost::gil::rgb8_image_t img;
        boost::gil::rgb8_view_t view;
        std::tuple<int, int, int> getPixelInternal(int x, int y);
        int getPixelInternal(int x, int y, int channel);

    public:    
        double SquaredErrorOfInterpolation(int xx1, int xx2, int yy1, int yy2, int channel);
        double GetInterpolatedPixel(int x1, int x2, int y1, int y2, int x, int y, int channel);
        Image(std::string filename);
        Image(uint8_t*** pixels, int width, int height, int channels);
        long long CompareWith(Image& other);
        long long CompareWith(Image& other, int x, int y, int width, int height);
        double CompareWithInterpolation(int xx1, int xx2, int yy1, int yy2, int channel);
        std::tuple<int, int, int> getPixel(int x, int y);
        int width();
        int height();
        static int const RED_CHANNEL=0;
        static int const GREEN_CHANNEL=1;
        static int const BLUE_CHANNEL=2;
};

class ImageMagnifier : public Image
{
    private:
        int ratio = 4;
    public:    
        double SquaredErrorOfInterpolation(int xx1, int xx2, int yy1, int yy2, int channel);
        double GetInterpolatedPixel(int x1, int x2, int y1, int y2, int x, int y, int channel);
        long long CompareWith(Image& other, int x, int y, int width, int height);
        double CompareWithInterpolation(int xx1, int xx2, int yy1, int yy2, int channel);
        std::tuple<int, int, int> getPixel(int x, int y);
        int width();
        int height();
};
#endif // __IMAGE_HPP__
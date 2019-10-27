#ifndef __IMAGE_HPP__
#define __IMAGE_HPP__

#include <string>
#include <boost/gil/extension/io/bmp.hpp>



class Image
{

    private:
        boost::gil::rgb8_image_t img;
        boost::gil::rgb8_view_t view;

    public:    
        Image(std::string filename);
        Image(std::uint8_t ** pixels, int width, int height);
        long long CompareWith(Image other);
        long long CompareWith(Image other, int x, int y, int width, int height);
        std::tuple<int, int, int> getPixel(int x, int y);
        int width();
        int height();
        static int const RED_CHANNEL=0;
        static int const GREEN_CHANNEL=1;
        static int const BLUE_CHANNEL=2;
};
#endif // __IMAGE_HPP__
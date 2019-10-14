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
        int width();
        int height();
};
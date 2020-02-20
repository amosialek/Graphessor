#include "Image.hpp"
#include <string>
#include <iostream>



Image::Image(std::string filename)
{
    boost::gil::read_image(filename, img, boost::gil::bmp_tag());
    view = boost::gil::view(img);
}

Image::Image(uint8_t*** pixels, int width, int height, int channels)
{
    img = boost::gil::rgb8_image_t(width, height);
    boost::gil::fill_pixels(img._view, boost::gil::rgb8_pixel_t(200,0,0));
    for(int y=0;y<height;y++)
        for(int x=0;x<width;x++)
            for(int channel=0; channel < std::min(channels, 3); channel++)
                img._view[y*width+x][channel]=pixels[x][y][channel];
    view = boost::gil::view(img);
}

long long Image::CompareWith(Image& other, int x0, int y0, int width, int height)
{
    long long sum = 0;
    for(int y = y0; y < height; y++)
    {
        for(int x = x0; x < width; x++)
        {
            for(int channel=0; channel<3; channel++)
                sum += abs(view[y * width + x][channel] - other.view[y * width + x][channel]);
        }
    }
    return sum;
}

long long Image::CompareWith(Image& other)
{
    return CompareWith(other, 0, 0, width(), height());
}

int Image::width()
{
    return img.width();
}

int Image::height()
{
    return img.height();
}

std::tuple<int, int, int> Image::getPixel(int x, int y)
{
    return getPixelInternal(x, y);
}

std::tuple<int, int, int> Image::getPixelInternal(int x, int y)
{
    int r = view[y * img.width() + x][0];
    int g = view[y * img.width() + x][1];
    int b = view[y * img.width() + x][2];
    //std::cerr<<x<<" "<<y<<" "<<r<<" "<<g<<" "<<b<<std::endl;
    return std::make_tuple(r,g,b);   
}

int Image::getPixelInternal(int x, int y, int channel)
{
    return view[y * img.width() + x][channel];
}

double Image::GetInterpolatedPixel(int x1, int x2, int y1, int y2, int x, int y, int channel)
{
        int r11,r21,r12,r22;
        r11 = getPixelInternal(x1,y1, channel);
        r21 = getPixelInternal(x2,y1, channel);
        r12 = getPixelInternal(x1,y2, channel);
        r22 = getPixelInternal(x2,y2, channel);
        double coef22 = (x-x1)*(y-y1)/(1.0*(x2-x1)*(y2-y1));
        double coef12 = (x2-x)*(y-y1)/(1.0*(x2-x1)*(y2-y1));
        double coef21 = (x-x1)*(y2-y)/(1.0*(x2-x1)*(y2-y1));
        double coef11 = (x2-x)*(y2-y)/(1.0*(x2-x1)*(y2-y1));
        double rInterpolated = r11 * coef11 + r21 * coef21 + r12 * coef12 + r22 * coef22;
        return rInterpolated;
}

double Image::SquaredErrorOfInterpolation(int xx1, int xx2, int yy1, int yy2, int channel)
{
    int x1 = xx1;
    int x2 = xx2;
    int y1 = yy1;
    int y2 = yy2;
    double sum = 0;
    std::cout<<"x2 -x1, y2-y1: "<<x2-x1<<" "<<y2-y1<<std::endl;
    if (x2-x1==0 or y2-y1==0) return 0;

    for(int x=x1;x<=x2;x++)
        for(int y=y1;y<=y2;y++)
        {
            int rOriginal;
            rOriginal = getPixelInternal(x, y, channel);
            double rInterpolated = GetInterpolatedPixel(x1,x2,y1,y2,x,y,channel);
            std::cout<<x<<" "<<y<<" "<<rOriginal<<" "<<rInterpolated<<std::endl;
            sum+=(rInterpolated - rOriginal)*(rInterpolated - rOriginal);
            ;
        }
    return sum;
}

double Image::CompareWithInterpolation(int xx1, int xx2, int yy1, int yy2, int channel)
{
    int x1 = xx1;
    int x2 = xx2;
    int y1 = yy1;
    int y2 = yy2;
    double maxSum = 255*255*(x2-x1)*(y2-y1);
    return SquaredErrorOfInterpolation(xx1, xx2, yy1, yy2, channel)/maxSum;
}



double ImageMagnifier::SquaredErrorOfInterpolation(int xx1, int xx2, int yy1, int yy2, int channel)
{
    return Image::SquaredErrorOfInterpolation(xx1/ratio, xx2/ratio, yy1/ratio, yy2/ratio, channel);
}
double ImageMagnifier::GetInterpolatedPixel(int x1, int x2, int y1, int y2, int x, int y, int channel)
{
    return Image::GetInterpolatedPixel(x1/ratio, x2/ratio, y1/ratio, y2/ratio, x/ratio, y/ratio, channel);
}
long long ImageMagnifier::CompareWith(Image& other, int x, int y, int width, int height)
{
    return Image::CompareWith(other, x/ratio, y/ratio, width/ratio, height/ratio);
}
double ImageMagnifier::CompareWithInterpolation(int xx1, int xx2, int yy1, int yy2, int channel)
{
    return Image::CompareWithInterpolation(xx1/ratio, xx2/ratio, yy1/ratio, yy2/ratio, channel);
}
std::tuple<int, int, int> ImageMagnifier::getPixel(int x, int y)
{
    return Image::getPixel(x/ratio, y/ratio);
}
int ImageMagnifier::width()
{
    return Image::width()*ratio;
}
int ImageMagnifier::height()
{
    return Image::height()*ratio;
}

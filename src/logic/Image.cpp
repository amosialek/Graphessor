#include "Image.hpp"
#include <string>
#include <iostream>

int ratio = 4;

Image::Image(std::string filename)
{
    boost::gil::read_image(filename, img, boost::gil::bmp_tag());
    view = boost::gil::view(img);
}

Image::Image(uint8_t** pixels, int width, int height)
{

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
    return ratio*img.width();
}

int Image::height()
{
    return ratio*img.height();
}

std::tuple<int, int, int> Image::getPixel(int x, int y)
{
    return getPixelInternal(x/ratio, y/ratio);
}

std::tuple<int, int, int> Image::getPixelInternal(int x, int y)
{
    int r = view[y * img.width() + x][0];
    int g = view[y * img.width() + x][1];
    int b = view[y * img.width() + x][2];
    //std::cerr<<x<<" "<<y<<" "<<r<<" "<<g<<" "<<b<<std::endl;
    return std::make_tuple(r,g,b);   
}

long long Image::CompareWithInterpolation(int xx1, int xx2, int yy1, int yy2)
{
    int x1 = xx1/ratio;
    int x2 = xx2/ratio;
    int y1 = yy1/ratio;
    int y2 = yy2/ratio;
    double sum = 0;
    if(x2-x1==0 or y2-y1==0)return 0;
    //std::cerr<<x1<<" "<<x2<<" "<<y1<<" "<<y2<<std::endl; 
    int r11,r21,r12,r22,g11,g21,g12,g22,b11,b21,b12,b22;
    std::tie(r11,g11,b11) = getPixelInternal(x1,y1);
    std::tie(r21,g21,b21) = getPixelInternal(x2,y1);
    std::tie(r12,g12,b12) = getPixelInternal(x1,y2);
    std::tie(r22,g22,b22) = getPixelInternal(x2,y2);
    for(int x=x1;x<=x2;x++)
        for(int y=y1;y<=y2;y++)
        {
            int rOriginal,gOriginal,bOriginal;
            std::tie(rOriginal,gOriginal,bOriginal) = getPixelInternal(x,y);
            double coef11 = (x-x1)*(y-y1)/(1.0*(x2-x1)*(y2-y1));
            double coef21 = (x2-x)*(y-y1)/(1.0*(x2-x1)*(y2-y1));
            double coef12 = (x-x1)*(y2-y)/(1.0*(x2-x1)*(y2-y1));
            double coef22 = (x2-x)*(y2-y)/(1.0*(x2-x1)*(y2-y1));
            double rInterpolated = r11 * coef11 + r21 * coef21 + r12 * coef12 + r22 * coef22;
            double gInterpolated = g11 * coef11 + g21 * coef21 + g12 * coef12 + g22 * coef22;
            double bInterpolated = b11 * coef11 + b21 * coef21 + b12 * coef12 + b22 * coef22;
            sum+=abs(rInterpolated - rOriginal);
            sum+=abs(gInterpolated - gOriginal);
            sum+=abs(bInterpolated - bOriginal);
        }
    return sum;
}
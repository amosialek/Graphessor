#include "ImageMagnifier.hpp"

// double ImageMagnifier::SquaredErrorOfInterpolation(int x1, int x2, int y1, int y2, int channel)
// {
//     int originalRatio = ratio;
//     ratio = 1;
//     auto result = Image::SquaredErrorOfInterpolation(x1/originalRatio, x2/originalRatio, y1/originalRatio, y2/originalRatio, channel);
//     ratio = originalRatio;
//     return result;
// }

// double ImageMagnifier::SquaredErrorOfInterpolation(int x1, int x2, int x3, int y1, int y2, int y3, int channel)
// {
//     int originalRatio = ratio;
//     ratio = 1;
//     auto result = Image::SquaredErrorOfInterpolation(x1/originalRatio, x2/originalRatio, x3/originalRatio, y1/originalRatio, y2/originalRatio, y3/originalRatio, channel);
//     ratio = originalRatio;
//     return result;
// }

double ImageMagnifier::GetInterpolatedPixel(int x1, int x2, int y1, int y2, int x, int y, int channel)
{
    int originalRatio = ratio;
    ratio = 1;
    auto result = Image::GetInterpolatedPixel(x1/originalRatio, x2/originalRatio, y1/originalRatio, y2/originalRatio, x/originalRatio, y/originalRatio, channel);
    ratio = originalRatio;
    return result;
}

// long long ImageMagnifier::CompareWith(Image& other, int x, int y, int width, int height)
// {
//     int originalRatio = ratio;
//     ratio = 1;
//     auto result = Image::CompareWith(other, x/originalRatio, y/originalRatio, width/originalRatio, height/originalRatio);
//     ratio = originalRatio;
//     return result;
// }

// double ImageMagnifier::CompareWithInterpolation(int x1, int x2, int y1, int y2, int channel)
// {
//     int originalRatio = ratio;
//     ratio = 1;
//     auto result = Image::CompareWithInterpolation(x1/originalRatio, x2/originalRatio, y1/originalRatio, y2/originalRatio, channel);
//     ratio = originalRatio;
//     return result;
// }

// double ImageMagnifier::CompareWithInterpolation(int x1, int x2, int x3, int y1, int y2, int y3, int channel)
// {
//     int originalRatio = ratio;
//     ratio = 1;
//     auto result = Image::CompareWithInterpolation(x1/originalRatio, x2/originalRatio, x3/originalRatio, y1/originalRatio, y2/originalRatio, y3/originalRatio, channel);
//     ratio = originalRatio;
//     return result;
// }

std::tuple<int, int, int> ImageMagnifier::getPixel(int x, int y)
{
    int originalRatio = ratio;
    ratio = 1;
    auto result = Image::getPixel(x/originalRatio, y/originalRatio);
    ratio = originalRatio;
    return result;
}

int ImageMagnifier::width()
{
    int originalRatio = ratio;
    ratio = 1;
    auto result = Image::width()*originalRatio;
    ratio = originalRatio;
    return result;
}

int ImageMagnifier::height()
{
    int originalRatio = ratio;
    ratio = 1;
    auto result = Image::height()*originalRatio;
    ratio = originalRatio;
    return result;
}

std::tuple<int,int> ImageMagnifier::GetNearestPixelCoords(int x, int y)
{
    int halfRatio = ratio/2;
    int xmod = x%ratio;
    int ymod = y%ratio;
    int xres = x - xmod;
    int yres = y - ymod;
    if(xmod >= halfRatio && xres+ratio<width())
        xres += ratio;
    if(ymod >= halfRatio && yres+ratio<height())
        yres += ratio;
    return std::make_tuple(xres,yres);
}

void ImageMagnifier::SetPixel(int x, int y, int channel, int value)
{
    int originalRatio = ratio;
    ratio = 1;
    Image::SetPixel(x/originalRatio, y/originalRatio, channel, value);
    ratio = originalRatio;
}

Image* ImageMagnifier::GetImageInternal()
{
    ratio = 1;
    return Image::GetImageInternal();
}

double ImageMagnifier::PSNR(Image* image)
{
    int originalRatio = ratio;
    ratio = 1;
    auto result = Image::PSNR(image->GetImageInternal());
    ratio = originalRatio;
    return result;
}

std::vector<std::shared_ptr<Array2D>> ImageMagnifier::GetChannelsAsArrays()
{
    std::vector<std::shared_ptr<Array2D>> result;
    int imgWidth = width();
    int imgHeight = height();
    
    for(int channel=0;channel<3;channel++)
    {
        result.push_back(std::make_shared<Array2D>(imgWidth, imgHeight));
        for(int x=0;x<imgWidth;x++)
            for(int y=0;y<imgHeight;y++)
                (*(result[channel]))[x][y] = view[y/ratio*imgWidth/ratio + x/ratio][channel];
    }
    return result;
}
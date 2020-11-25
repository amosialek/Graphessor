#include "Image.hpp"
#include "GraphessorConstants.hpp"
#include <iostream>
#include "spdlog/spdlog.h"
#include <cassert>


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

Image::Image(int width, int height)
{
    img = boost::gil::rgb8_image_t(width, height);
    boost::gil::fill_pixels(img._view, boost::gil::rgb8_pixel_t(255,255,255));
    view = boost::gil::view(img);
}

Image::Image(std::vector<std::shared_ptr<Array2D>> arrays)
{
    int width = arrays[0]->width;
    int height = arrays[0]->height;
    img = boost::gil::rgb8_image_t(width+1, height+1);
    boost::gil::fill_pixels(img._view, boost::gil::rgb8_pixel_t(0,0,0));
    for(int channel=0;channel<3;channel++)
    {
        view = boost::gil::view(img);
        for(int x=0;x<width;x++)
            for(int y=0;y<height;y++)
                SetPixel(x,y,channel,arrays[channel]->operator[](x)[y]);
    }
    view = boost::gil::view(img);
}


Image::Image(std::vector<std::shared_ptr<CachedGraph>> graphs)
{
    int width = 0;
    int height = 0;
    auto pixels = graphs[0] -> GetPixels();
    for(auto pixel : pixels)
    {
        width = std::max(width ,graphs[0] -> operator[](pixel).x);
        height = std::max(height ,graphs[0] -> operator[](pixel).y);
    }
    img = boost::gil::rgb8_image_t(width+1, height+1);
    boost::gil::fill_pixels(img._view, boost::gil::rgb8_pixel_t(0,0,0));
    for(int channel=0;channel<3;channel++)
    {
        view = boost::gil::view(img);
        //graphs[channel]->GetImage(this);
        graphs[channel] -> GetPixels();
        Interpolate(channel, width, graphs[channel]);
    }
    view = boost::gil::view(img);
}


int Image::GetRGBChannelValue(Pixel p, int channel)
{
    if (channel==0)
        return p.r;
    if (channel==1)        
        return p.g;
    return p.b;
}

void Image::BilinearInterpolation(int channel, std::vector<Pixel> pixels)
{
    int minx,miny,maxx,maxy;
    minx = maxx = pixels.begin() -> x;
    miny = maxy = pixels.begin() -> y;
    for(auto adjacentVertex : pixels)
    {
        minx = std::min(minx, adjacentVertex.x);
        maxx = std::max(maxx, adjacentVertex.x);
        miny = std::min(miny, adjacentVertex.y);
        maxy = std::max(maxy, adjacentVertex.y);
    }
    for(int y=miny;y<=maxy;y++)
        for(int x=minx;x<=maxx;x++)
            SetPixel(x, y, channel, GetInterpolatedPixel(minx,maxx,miny, maxy,x,y,channel));
            
}

void Image::Interpolate(int channel, int width, std::shared_ptr<CachedGraph> graph)
{
    std::set<vertex_descriptor> pixels;
    pixels = graph -> GetPixels();
    for(auto pixel : pixels)
    {
        this -> SetPixel(graph -> operator[](pixel).x, graph -> operator[](pixel).y, channel, this -> GetRGBChannelValue(graph->operator[](pixel), channel));
         //img._view[graph->operator[](pixel).y * (width+1) + graph -> operator[](pixel).x][channel] = this -> GetRGBChannelValue(graph->operator[](pixel), channel);
        spdlog::debug("Setting x={} y={} in channel {} to {}", graph ->operator[](pixel).x,graph ->operator[](pixel).y, channel, this -> GetRGBChannelValue(graph->operator[](pixel), channel));    
    }
    auto PixelsForBilinearInterpolation = graph ->  GetPixelsForBilinearInterpolation();
    auto PixelsForBaricentricInterpolation = graph ->  GetPixelsForBaricentricInterpolation();
    auto PixelsForSVDInterpolation = graph ->  GetPixelsForSVDInterpolation();
    for(auto v : PixelsForBilinearInterpolation)
    {
        BilinearInterpolation(channel, v);
    }
    for(auto v : PixelsForBaricentricInterpolation)
    {
        BaricentricInterpolation(channel, v);
    }
    for(auto v : PixelsForSVDInterpolation)
    {
        // SVDInterpolation(channel, v);
    }
        
}

void Image::FillMissingSpacesBasedOnLargerBlocks(std::shared_ptr<IGraph> graph, std::set<vertex_descriptor>& pixels, int channel, int width)
{
    for(auto pixel : pixels)
    {
        auto pixelNeighbors = graph -> GetAdjacentVertices(pixel, NODELABEL_I);
        if(pixelNeighbors.size()<4)
            continue;
        std::set<vertex_descriptor> neighborPixels;
        for(auto v : pixelNeighbors)
        {
            auto adjPixels = graph -> GetAdjacentVertices(v, NODELABEL_P);
            std::copy(adjPixels.begin(), adjPixels.end(), std::inserter(neighborPixels,neighborPixels.end()));
        }
        if(neighborPixels.size()<9) //not all neighbor I have been broken
        {
            int minx,maxx,miny,maxy;
            minx=maxx= graph -> operator[](*neighborPixels.begin()).x;
            miny=maxy= graph -> operator[](*neighborPixels.begin()).y;
            for(auto v : neighborPixels)
            {
                minx = std::min(minx, graph ->operator[](v).x);
                maxx = std::max(maxx, graph ->operator[](v).x);
                miny = std::min(miny, graph ->operator[](v).y);
                maxy = std::max(maxy, graph ->operator[](v).y);
            }
            for(int y=miny;y<=maxy;y++)
                for(int x=minx;x<=maxx;x++)//img._view[miny*(width+1)+minx][channel]==0 || img._view[miny*(width+1)+maxx][channel]==0 || img._view[maxy*(width+1)+minx][channel]==0 ||img._view[maxy*(width+1)+maxx][channel]==0
                    if(img._view[y*(width+1)+x][channel]==0)
                        img._view[y*(width+1)+x][channel]=this->GetInterpolatedPixel(minx,maxx,miny, maxy,x,y,channel);
        }
    }
}

void Image::BaricentricInterpolation(int channel, std::vector<Pixel> pixels)
{
    int y2y3 = pixels[1].y-pixels[2].y;
    int x3x2 = pixels[2].x-pixels[1].x;
    int y3y1 = pixels[2].y-pixels[0].y;
    int y1y3 = pixels[0].y-pixels[2].y;
    int x1x3 = pixels[0].x-pixels[2].x;
    int minx = std::min(pixels[0].x,(std::min(pixels[1].x,pixels[2].x)));
    int miny = std::min(pixels[0].y,(std::min(pixels[1].y,pixels[2].y)));
    int maxx = std::max(pixels[0].x,(std::max(pixels[1].x,pixels[2].x)));
    int maxy = std::max(pixels[0].y,(std::max(pixels[1].y,pixels[2].y)));
    double w1Coefficient = 1.0/(y2y3 * x1x3 + x3x2 * y1y3);
    double w2Coefficient = w1Coefficient;
    double w1,w2,w3;
    Pixel p;
    for(int y = miny; y <= maxy; y++)
        for(int x=minx;x<=maxx;x++)//img._view[miny*(width+1)+minx][channel]==0 || img._view[miny*(width+1)+maxx][channel]==0 || img._view[maxy*(width+1)+minx][channel]==0 ||img._view[maxy*(width+1)+maxx][channel]==0
        {
            std::tie(p.r,p.g,p.b) = getPixel(x,y);
            if(GetRGBChannelValue(p,channel)==0)
            {
                w1 = w1Coefficient*(y2y3*(x-pixels[2].x) + x3x2 * (y-pixels[2].y));
                w2 = w2Coefficient*(y3y1*(x-pixels[2].x) + x1x3*(y-pixels[2].y));
                w3 = 1 - w1 - w2;
                if(w1>=-0.01 and w2>=-0.01 and w3>=-0.01)
                    SetPixel(x,y,channel,
                        w1 * GetRGBChannelValue(pixels[0],channel) 
                        + w2 * GetRGBChannelValue(pixels[1],channel)
                        + w3 * GetRGBChannelValue(pixels[2],channel));
            }
        }
}

void Image::SVDInterpolation(int channel, std::vector<Pixel> pixels)
{

}

void Image::Save3Colors(std::string filename)
{
    auto imageRed = std::make_unique<boost::gil::rgb8_image_t>(width(), height());
    auto imageGreen = std::make_unique<boost::gil::rgb8_image_t>(width(), height());
    auto imageBlue = std::make_unique<boost::gil::rgb8_image_t>(width(), height());
    auto redView = boost::gil::view(*imageRed);
    auto greenView = boost::gil::view(*imageGreen);
    auto blueView = boost::gil::view(*imageBlue);

    int r,g,b;
    for(int x=0;x<width();x++)
        for(int y=0;y<height();y++)
        {
            std::tie(r,g,b)=getPixel(x,y);
            redView[y * width()+x][0] = r; 
            greenView[y * width()+x][1] = g; 
            blueView[y * width()+x][2] = b; 
        }
    boost::gil::write_view(filename+"_red.bmp", redView, boost::gil::bmp_tag());
    boost::gil::write_view(filename+"_green.bmp", greenView, boost::gil::bmp_tag());
    boost::gil::write_view(filename+"_blue.bmp", blueView, boost::gil::bmp_tag());
}

// long long Image::CompareWith(Image& other, int x0, int y0, int width, int height)
// {
//     long long sum = 0;
//     for(int y = y0; y < height; y++)
//     {
//         for(int x = x0; x < width; x++)
//         {
//             for(int channel=0; channel<3; channel++)
//                 sum += abs(view[y * width + x][channel] - other.view[y * width + x][channel]);
//         }
//     }
//     return sum;
// }

// long long Image::CompareWith(Image& other)
// {
//     return CompareWith(other, 0, 0, width(), height());
// }

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
        double coef22 = (x-x1)*(y-y1)/(1.0*(x2-x1)*(y2-y1));//r11==0 || r22==0 || r21==0 || r12==0
        double coef12 = (x2-x)*(y-y1)/(1.0*(x2-x1)*(y2-y1));
        double coef21 = (x-x1)*(y2-y)/(1.0*(x2-x1)*(y2-y1));
        double coef11 = (x2-x)*(y2-y)/(1.0*(x2-x1)*(y2-y1));
        double rInterpolated = r11 * coef11 + r21 * coef21 + r12 * coef12 + r22 * coef22;
        return rInterpolated;
}

double Image::GetInterpolatedPixel(int x1, int x2, int x3, int y1, int y2, int y3, int x, int y, double wDenominator, int channel)
{

    double w1Coefficient = wDenominator;
    double w2Coefficient = wDenominator;
    double w1,w2,w3;
    int r1,r2,r3;
    r1 = getPixelInternal(x1,y1, channel);
    r2 = getPixelInternal(x2,y2, channel);
    r3 = getPixelInternal(x3,y3, channel);
    w1 = w1Coefficient * ((y2 - y3) * (x - x3) + (x3 - x2) * (y - y3));
    w2 = w2Coefficient * ((y3 - y1) * (x - x3) + (x1 - x3) * (y - y3));
    w3 = 1 - w1 - w2;
    double rInterpolated = r1 * w1 + r2 * w2 + r3 * w3;
    return rInterpolated;
}

// double Image::SquaredErrorOfInterpolation(int x1, int x2, int y1, int y2, int channel)
// {
//     double sum = 0;
//     if (x2-x1==0 or y2-y1==0) return 0;
//     for(int x=x1;x<=x2;x++)
//         for(int y=y1;y<=y2;y++)
//         {
//             int rOriginal;
//             rOriginal = getPixelInternal(x, y, channel);
//             double rInterpolated = GetInterpolatedPixel(x1,x2,y1,y2,x,y,channel);
//             sum+=(rInterpolated - rOriginal)*(rInterpolated - rOriginal);
//         }
//     return sum;
// }

// double Image::SquaredErrorOfInterpolation(int x1, int x2, int x3, int y1, int y2, int y3, int channel)
// {
//     double sum = 0;

//     int minx = std::min(x1, std::min(x2,x3));
//     int miny = std::min(y1, std::min(y2,y3));
//     int maxx = std::max(x1, std::max(x2,x3));
//     int maxy = std::max(y1, std::max(y2,y3));

//     if (maxx-minx==0 or maxy-miny==0) 
//         return 0;
//     double wDenominator = 1.0/((y2-y3) * (x1-x3) + (x3-x2) * (y1-y3));
//     for(int x=minx;x<=maxx;x++)
//         for(int y=miny;y<=maxy;y++)
//             if(PointInTriangle(x,y,x1,y1,x2,y2,x3,y3))
//             {
//                 int rOriginal;
//                 rOriginal = getPixelInternal(x, y, channel);
//                 double rInterpolated = GetInterpolatedPixel(x1,x2,x3,y1,y2,y3,x,y,wDenominator,channel);
//                 sum+=(rInterpolated - rOriginal)*(rInterpolated - rOriginal);
//         //      std::cout<<x<<" "<<y<<" "<<rOriginal<<" "<<rInterpolated<<" "<<sum<<std::endl;
//             }
//     return sum;
// }

// double Image::CompareWithInterpolation(int x1, int x2, int y1, int y2, int channel)
// {
//     double maxSum = 255.0*255*(x2-x1+1)*(y2-y1+1);
//     return SquaredErrorOfInterpolation(x1, x2, y1, y2, channel)/maxSum;
// }

// double Image::CompareWithInterpolation(int x1, int x2, int x3, int y1, int y2, int y3, int channel)
// {
//     double maxSum = 255.0*255*abs(x1*y2+x2*y3+x3*y1-x1*y3-x2*y1-x3*y2) / 2;
//     double result =  SquaredErrorOfInterpolation(x1, x2, x3, y1, y2, y3, channel)/maxSum;
//     return result;
// }

void Image::save(std::string filename)
{
    boost::gil::write_view(filename, this->view, boost::gil::bmp_tag());
}

std::tuple<int,int> Image::GetNearestPixelCoords(int x, int y)
{
    return std::make_tuple(x,y);
}

void Image::SetPixelSafe(int x, int y, int channel, int value)
{
    if(x>0 and y>0 and x<width() and y<height())
        SetPixel(x,y,channel, value);
}

void Image::SetPixel(int x, int y, int channel, int value)
{
    view[y*width()+x][channel] = value;
}

void Image::SetSquare(int x, int y, int channel, int value, int width)
{
    for(int x1 = x-width/2;x1<=x+width/2;x1++)
        for(int y1 = y-width/2;y1<=y+width/2;y1++)
            SetPixelSafe(x,y,channel,value);
}

template <typename T> int sign(T val) {
    return (T(0) < val) - (val < T(0));
}

void Image::DrawLine(int x1, int y1, int x2, int y2, int channel, int color)
{
    if(x1==x2)
    {
        for(int y=y1;y<y2;y++)
        {
            view[y * width() + x1][channel]=color;
        }
        return;
    }
    if(y1==y2)
    {
        for(int x=x1;x<x2;x++)
        {
            view[y1 * width() + x][channel]=color;
        }
        return;
    }
    double deltax = x2 - x1;
    double deltay = y2 - y1;
    double deltaerr = abs(1.0 * deltay / deltax);    // Assume deltax != 0 (line is not vertical),
          // note that this division needs to be done in a way that preserves the fractional part
    double error = 0.0; // No error at start
    int y = y1;
    for (int x = x1;x< x2;x++)
    {
        SetPixel(x, y, channel, color);
        error += deltaerr;
        if (error >= 0.5)
        {
            y += sign(deltay) * 1;
            error -= 1.0;
        }
    }
}

void Image::DrawBlackLine(int x1, int y1, int x2, int y2)
{
    int pixelCoord;
    if(x1==x2)
    {
        for(int y=y1;y<y2;y++)
        {
            pixelCoord = y * width() + x1;
            view[pixelCoord][0] = view[pixelCoord][1] = view[pixelCoord][2] = 0;
        }
        return;
    }
    if(y1==y2)
    {
        for(int x=x1;x<x2;x++)
        {
            pixelCoord = y1 * width() + x;
            view[pixelCoord][0] = view[pixelCoord][1] = view[pixelCoord][2] = 0;
        }
        return;
    }
    double deltax = x2 - x1;
    double deltay = y2 - y1;
    double deltaerr = abs(1.0 * deltay / deltax);    // Assume deltax != 0 (line is not vertical),
          // note that this division needs to be done in a way that preserves the fractional part
    double error = 0.0; // No error at start
    int y = y1;
    for (int x = x1;x< x2;x++)
    {
        pixelCoord = y * width() + x;
        view[pixelCoord][0] = view[pixelCoord][1] = view[pixelCoord][2] = 0;
        error += deltaerr;
        if (error >= 0.5)
        {
            y += sign(deltay) * 1;
            error -= 1.0;
        }
    }
}

// int Image::sign2 (int x1, int y1, int x2, int y2, int x3, int y3)
// {
//     return (x1 - x3) * (y2 - y3) - (x2 - x3) * (y1 - y3);
// }

// bool Image::PointInTriangle (int px, int py, int x1, int y1, int x2, int y2, int x3, int y3)
// {
//     int d1, d2, d3;
//     bool has_neg, has_pos;

//     d1 = sign2(px, py, x1, y1, x2, y2);
//     d2 = sign2(px, py, x2, y2, x3, y3);
//     d3 = sign2(px, py, x3, y3, x1, y1);

//     has_neg = (d1 < 0) && (d2 < 0) && (d3 < 0);
//     has_pos = (d1 > 0) && (d2 > 0) && (d3 > 0);

//     return !(has_neg && has_pos);
// }

double Image::PSNR(Image* other)
{
    //assert(this -> width() == other -> width() && "Cannot compare images of different width");
    //assert(this -> height() == other -> height() && "Cannot compare images of different height");
    int r,g,b;
    int otherR, otherG, otherB;
    double sum=0;
    for(int x=0;x<this->width() ;x++)
        for(int y=0;y<this->height() ;y++)
        {
            std::tie(r,g,b) = getPixel(x,y);
            std::tie(otherR,otherG,otherB) = other -> getPixel(x,y);

            sum+=(r-otherR)*(r-otherR)+(b-otherB)*(b-otherB)+(g-otherG)*(g-otherG);
        }
    std::cout<<"sum for PSNR "<<sum<<std::endl;
    sum = sum / (255.0*255.0*3.0*width()*height());
    return 10*log10(1/sum);
}

Image* Image::GetImageInternal()
{
    return this;
}

std::vector<std::shared_ptr<Array2D>> Image::GetChannelsAsArrays()
{
    std::vector<std::shared_ptr<Array2D>> result;

    for(int channel=0;channel<3;channel++)
    {
        result.push_back(std::make_shared<Array2D>(width(), height()));
        for(int x=0;x<width();x++)
            for(int y=0;y<height();y++)
                (*(result[channel]))[x][y] = view[y*width()+x][channel];
    }
    return result;
}
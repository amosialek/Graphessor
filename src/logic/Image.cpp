#include "Image.hpp"
#include "GraphessorConstants.hpp"
#include <string>
#include <iostream>
#include "spdlog/spdlog.h"


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


int GetRGBChannelValue(Pixel p, int channel)
{
    if (channel==0)
        return p.r;
    if (channel==1)        
        return p.g;
    return p.b;
}

Image::Image(std::vector<std::shared_ptr<CachedGraph>> graphs)
{
    int width = 0;
    int height = 0;
    auto pixels = graphs[0] -> GetCacheIterator(NODELABEL_P);
    for(auto pixel : pixels)
    {
        width = std::max(width ,graphs[0] -> operator[](pixel).x);
        height = std::max(height ,graphs[0] -> operator[](pixel).y);
    }
    img = boost::gil::rgb8_image_t(width+1, height+1);
    boost::gil::fill_pixels(img._view, boost::gil::rgb8_pixel_t(0,0,0));
    for(int channel=0;channel<3;channel++)
    {
        pixels.clear();
        pixels = graphs[channel] -> GetCacheIterator(NODELABEL_P);
        for(auto pixel : pixels)
        {
            img._view[graphs[channel]->operator[](pixel).y * (width+1) + graphs[channel]-> operator[](pixel).x][channel] = GetRGBChannelValue(graphs[channel]->operator[](pixel), channel);
            spdlog::debug("Setting x={} y={} in channel {} to {}", graphs[channel]->operator[](pixel).x,graphs[channel]->operator[](pixel).y, channel, GetRGBChannelValue(graphs[channel]->operator[](pixel), channel));    
        }
        auto IEdges = graphs[channel]->GetCacheIterator(NODELABEL_I);
        std::vector<vertex_descriptor> fullIEdges;
        for(auto v : IEdges)
        {
            if(graphs[channel] -> GetAdjacentVertices(v).size()==4)
            fullIEdges.emplace_back(v);
        }
        for(auto v : fullIEdges)
        {
            view = boost::gil::view(img);
            auto adjacentVertices = graphs[channel]-> GetAdjacentVertices(v);
            int minx,miny,maxx,maxy;
            minx = maxx = (*graphs[channel])[adjacentVertices[0]].x;
            miny = maxy = (*graphs[channel])[adjacentVertices[0]].y;
            for(auto adjacentVertex : adjacentVertices)
            {
                minx = std::min(minx,(*graphs[channel])[adjacentVertex].x);
                maxx = std::max(maxx,(*graphs[channel])[adjacentVertex].x);
                miny = std::min(miny,(*graphs[channel])[adjacentVertex].y);
                maxy = std::max(maxy,(*graphs[channel])[adjacentVertex].y);
            }
            for(int y=miny;y<=maxy;y++)
                for(int x=minx;x<maxx;x++)//img._view[miny*(width+1)+minx][channel]==0 || img._view[miny*(width+1)+maxx][channel]==0 || img._view[maxy*(width+1)+minx][channel]==0 ||img._view[maxy*(width+1)+maxx][channel]==0
                    //for(int channel=0; channel < std::min(1, 3); channel++)
                        img._view[y*(width+1)+x][channel]=this->GetInterpolatedPixel(minx,maxx,miny, maxy,x,y,channel);
        }
        //FillMissingSpacesBasedOnLargerBlocks(graphs[channel], pixels, channel, width);
        FillMissingSpacesBasedOnBaricentricInterpolation(graphs[channel], pixels, channel, width);
    }
    view = boost::gil::view(img);
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
                for(int x=minx;x<maxx;x++)//img._view[miny*(width+1)+minx][channel]==0 || img._view[miny*(width+1)+maxx][channel]==0 || img._view[maxy*(width+1)+minx][channel]==0 ||img._view[maxy*(width+1)+maxx][channel]==0
                    if(img._view[y*(width+1)+x][channel]==0)
                        img._view[y*(width+1)+x][channel]=this->GetInterpolatedPixel(minx,maxx,miny, maxy,x,y,channel);
        }
    }

}

void Image::FillMissingSpacesBasedOnBaricentricInterpolation(std::shared_ptr<CachedGraph> graph, std::set<vertex_descriptor>& pixels, int channel, int width)
{
    auto FEdges = graph->GetCacheIterator(NODELABEL_F);
    std::set<vertex_descriptor> danglingFedges;
    for(auto fEdge : FEdges)
    {
        if(graph -> GetAdjacentVertices(fEdge).size()==1)
            danglingFedges.insert(fEdge);
    }
    for(auto f : danglingFedges)
    {
        auto adjacentPixel = graph->GetAdjacentVertices(f)[0];
        auto adjacentIEdges = graph->GetAdjacentVertices(adjacentPixel,NODELABEL_I);
        
        std::sort(adjacentIEdges.begin(), adjacentIEdges.end(), [f, graph](const vertex_descriptor& v1, const vertex_descriptor& v2)->bool
            {
                auto fx = (*graph)[f].x;
                auto fy = (*graph)[f].y;
                auto v1x = (*graph)[v1].x;
                auto v1y = (*graph)[v1].y;
                auto v2x = (*graph)[v2].x;
                auto v2y = (*graph)[v2].y;
                return (v1x-fx)*(v1x-fx)+(v1y-fy)*(v1y-fy) < (v2x-fx)*(v2x-fx)+(v2y-fy)*(v2y-fy);
            });
        
        std::set<vertex_descriptor> closerPixels;
        std::set<vertex_descriptor> outerPixels;
        auto a0 = graph->GetAdjacentVertices(adjacentIEdges[0], NODELABEL_P);
        auto a1 = graph->GetAdjacentVertices(adjacentIEdges[1], NODELABEL_P);
        std::copy(a0.begin(),a0.end(),std::inserter(closerPixels, closerPixels.end()));
        std::copy(a1.begin(),a1.end(),std::inserter(closerPixels, closerPixels.end()));
        a0 = graph->GetAdjacentVertices(adjacentIEdges[2], NODELABEL_P);
        a1 = graph->GetAdjacentVertices(adjacentIEdges[3], NODELABEL_P);
        std::copy(a0.begin(),a0.end(),std::inserter(outerPixels, outerPixels.end()));
        std::copy(a1.begin(),a1.end(),std::inserter(outerPixels, outerPixels.end()));

        std::vector<vertex_descriptor> otherTriangleVertices;

        std::set_difference(closerPixels.begin(), closerPixels.end(), outerPixels.begin(), outerPixels.end(), std::inserter(otherTriangleVertices, otherTriangleVertices.begin()));

        auto v1 = graph -> operator[](adjacentPixel);
        auto v2 = graph -> operator[](otherTriangleVertices[0]);
        auto v3 = graph -> operator[](otherTriangleVertices[1]);

        int minx = std::min(v1.x,(std::min(v2.x,v3.x)));
        int miny = std::min(v1.y,(std::min(v2.y,v3.y)));
        int maxx = std::max(v1.x,(std::max(v2.x,v3.x)));
        int maxy = std::max(v1.y,(std::max(v2.y,v3.y)));
        double w1Coefficient = 1.0/((v2.y-v3.y)*(v1.x-v3.x) + (v3.x-v2.x)*(v1.y-v3.y));
        double w2Coefficient = 1.0/((v2.y-v3.y)*(v1.x-v3.x) + (v3.x-v2.x)*(v1.y-v3.y));
        int y2y3 = v2.y-v3.y;
        int x3x2 = v3.x-v2.x;
        int y3y1 = v3.y-v1.y;
        int x1x3 = v1.x-v3.x;
        double w1,w2,w3;
        for(int y=miny;y<=maxy;y++)
            for(int x=minx;x<maxx;x++)//img._view[miny*(width+1)+minx][channel]==0 || img._view[miny*(width+1)+maxx][channel]==0 || img._view[maxy*(width+1)+minx][channel]==0 ||img._view[maxy*(width+1)+maxx][channel]==0
                if(img._view[y*(width+1)+x][channel]==0)
                {
                    w1 = w1Coefficient*(y2y3*(x-v3.x)+x3x2*(y-v3.y));
                    w2 = w2Coefficient*(y3y1*(x-v3.x) + x1x3*(y-v3.y));
                    w3 = 1 - w1 - w2;
                    img._view[y*(width+1)+x][channel] =
                       w1 * GetRGBChannelValue(v1,channel) 
                     + w2 * GetRGBChannelValue(v2,channel)
                     + w3 * GetRGBChannelValue(v3,channel);
                }
    }
    

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
        double coef22 = (x-x1)*(y-y1)/(1.0*(x2-x1)*(y2-y1));//r11==0 || r22==0 || r21==0 || r12==0
        double coef12 = (x2-x)*(y-y1)/(1.0*(x2-x1)*(y2-y1));
        double coef21 = (x-x1)*(y2-y)/(1.0*(x2-x1)*(y2-y1));
        double coef11 = (x2-x)*(y2-y)/(1.0*(x2-x1)*(y2-y1));
        double rInterpolated = r11 * coef11 + r21 * coef21 + r12 * coef12 + r22 * coef22;
        return rInterpolated;
}

double Image::SquaredErrorOfInterpolation(int x1, int x2, int y1, int y2, int channel)
{
    double sum = 0;
    //std::cout<<"x2 -x1, y2-y1: "<<x2-x1<<" "<<y2-y1<<std::endl;
    if (x2-x1==0 or y2-y1==0) return 0;

    for(int x=x1;x<=x2;x++)
        for(int y=y1;y<=y2;y++)
        {
            int rOriginal;
            rOriginal = getPixelInternal(x, y, channel);
            double rInterpolated = GetInterpolatedPixel(x1,x2,y1,y2,x,y,channel);
            sum+=(rInterpolated - rOriginal)*(rInterpolated - rOriginal);
      //      std::cout<<x<<" "<<y<<" "<<rOriginal<<" "<<rInterpolated<<" "<<sum<<std::endl;
            ;
        }
    return sum;
}

double Image::CompareWithInterpolation(int x1, int x2, int y1, int y2, int channel)
{
    double maxSum = 255.0*255*(x2-x1+1)*(y2-y1+1);
    return SquaredErrorOfInterpolation(x1, x2, y1, y2, channel)/maxSum;
}

void Image::save(std::string filename)
{
    boost::gil::write_view(filename, this->view, boost::gil::bmp_tag());
}

std::tuple<int,int> Image::GetNearestPixelCoords(int x, int y)
{
    return std::make_tuple(x,y);
}

double ImageMagnifier::SquaredErrorOfInterpolation(int x1, int x2, int y1, int y2, int channel)
{
    int originalRatio = ratio;
    ratio = 1;
    auto result = Image::SquaredErrorOfInterpolation(x1/originalRatio, x2/originalRatio, y1/originalRatio, y2/originalRatio, channel);
    ratio = originalRatio;
return result;
}
double ImageMagnifier::GetInterpolatedPixel(int x1, int x2, int y1, int y2, int x, int y, int channel)
{
    int originalRatio = ratio;
    ratio = 1;
    auto result = Image::GetInterpolatedPixel(x1/originalRatio, x2/originalRatio, y1/originalRatio, y2/originalRatio, x/originalRatio, y/originalRatio, channel);
    ratio = originalRatio;
return result;
}
long long ImageMagnifier::CompareWith(Image& other, int x, int y, int width, int height)
{
    int originalRatio = ratio;
    ratio = 1;
    auto result = Image::CompareWith(other, x/originalRatio, y/originalRatio, width/originalRatio, height/originalRatio);
    ratio = originalRatio;
return result;
}
double ImageMagnifier::CompareWithInterpolation(int x1, int x2, int y1, int y2, int channel)
{
    int originalRatio = ratio;
    ratio = 1;
    auto result = Image::CompareWithInterpolation(x1/originalRatio, x2/originalRatio, y1/originalRatio, y2/originalRatio, channel);
    ratio = originalRatio;
return result;
}
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

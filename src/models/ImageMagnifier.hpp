#ifndef __IMAGEMAGNIFIER_HPP__
#define __IMAGEMAGNIFIER_HPP__

#include "Image.hpp"

class ImageMagnifier : public Image
{
    private:
        int ratio = 4;
    public: 
        ImageMagnifier(std::string filename) : Image(filename){};
        ImageMagnifier(std::vector<std::shared_ptr<CachedGraph>> graphs) : Image(graphs){}
        ImageMagnifier(uint8_t*** pixels, int width, int height, int channels) : Image(pixels, width, height, channels){}
        // double SquaredErrorOfInterpolation(int xx1, int xx2, int yy1, int yy2, int channel) override;
        // double SquaredErrorOfInterpolation(int x1, int x2, int x3, int y1, int y2, int y3, int channel) override;
        double GetInterpolatedPixel(int x1, int x2, int y1, int y2, int x, int y, int channel) override;
        // long long CompareWith(Image& other, int x, int y, int width, int height) override;
        // double CompareWithInterpolation(int xx1, int xx2, int yy1, int yy2, int channel) override;
        // double CompareWithInterpolation(int x1, int x2, int x3, int y1, int y2, int y3, int channel) override;
        std::tuple<int, int, int> getPixel(int x, int y) override;
        int width() override;
        int height() override;
        std::tuple<int,int> GetNearestPixelCoords(int x, int y) override;
        void SetPixel(int x, int y, int channel, int value) override;
        double PSNR(Image* other) override;
        Image* GetImageInternal();
        std::vector<std::shared_ptr<Array2D>> GetChannelsAsArrays() override;

};

#endif // __IMAGEMAGNIFIER_HPP__
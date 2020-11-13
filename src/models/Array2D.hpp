#ifndef __ARRAY2D_HPP__
#define __ARRAY2D_HPP__

#include <vector>
#include <functional>

class Array2D
{
    private:
        std::vector<std::vector<double>> a;
    public:
        int height;
        int width;
        Array2D(std::vector<std::vector<double>> array);
        Array2D(int width, int height, double value = 0);
        Array2D(int width, int height, double (*func)(double, double));
        Array2D GetCopy();
        void FillWith(int x1, int x2, int y1, int y2, double value);
        void FillWith(int x1, int x2, int x3, int y1, int y2, int y3, double value);
        void FillWith(double value);
        void Subtract(const Array2D& other);
        void Subtract(double (*func)(double, double));
        double MultiplyElementWiseAndSum(double (*func)(double, double));
        double MultiplyElementWiseAndSum(double (*func)(double, double), int x1, int x2, int y1, int y2);
        double MultiplyElementWiseAndSum(std::function<double(double, double)>func, int x1, int x2, int y1, int y2);
        double SquaredError(Array2D& other, int x1, int x2, int y1, int y2);
        double SquaredError(Array2D& other, int x1, int x2, int x3, int y1, int y2, int y3);
        double CompareWith(Array2D& other, int xx1, int xx2, int yy1, int yy2);
        double CompareWith(Array2D& other, int x1, int x2, int x3, int y1, int y2, int y3);
        void BilinearInterpolation(Array2D& base, int x1, int x2, int y1, int y2);
        void BaricentricInterpolation(Array2D& base, int x1, int x2, int x3, int y1, int y2, int y3);
        std::vector<double>& operator[](int x);
};


#endif // __ARRAY2D_HPP__

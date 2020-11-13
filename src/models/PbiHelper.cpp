#include<functional>
#include"Array2D.hpp"
#include <math.h> 

std::function<double(double, double)> GetXSquareIntegral(double x1, double x2)
{
    return [x1,x2](double x, double y){return x*x*x/3 -(x*x)*(x1+x2)/2.0+x1*x2*x;};
}

std::function<double(double, double)> GetX4Integral(double x1, double x2)
{
    return [x1,x2](double x, double y){return x*x*x*x*x/5 - x*x*x*(x1+x2)*x/2 + x*x*4*x1*x2*x/3 + x*x*x1*x1*x/3 + x*x*x2*x2*x/3 - x*x1*x1*x2*x - x*x1*x2*x2*x + x1*x1*x2*x2*x;};
}

double GetXIntegralValue(std::function<double(double, double)> integral, double a, double b, double y)
{
    return integral(b,y)-integral(a,y);
}

std::function<double(double, double)> TransposeFunction(std::function<double(double, double)> f)
{
    return [f](double x, double y){return f(y,x);};
}

std::function<double(double, double)> QuadratureIntegralFunctionByX(std::function<double(double, double)> f)
{
    return [f](double x, double y){return f(floor(x)+1,y)-f(floor(x),y);};
}

std::function<double(double, double)> QuadratureIntegralFunction2D(std::function<double(double, double)> f)
{
    return [f](double x, double y){return f(floor(x)+1,floor(y)+1)-f(floor(x),floor(y));};
}

double GetSquareInterpolationOfEdge(Array2D& array,int x1, int x2, int y)
{
    std::function<double(double, double)> integral = GetXSquareIntegral(x1,x2);
    Array2D array2 = array.GetCopy();
    double sum = array2.MultiplyElementWiseAndSum(QuadratureIntegralFunctionByX(integral),x1,x2,y,y);
    double denominator = GetXIntegralValue(GetX4Integral(x1,x2),x1,x2,y);
    return sum/denominator;
}

double GetSquareInterpolationOfYEdge(Array2D& array,int x, int y1, int y2)
{
    std::function<double(double, double)> integral = GetXSquareIntegral(y1,y2);
    Array2D array2 = array.GetCopy();
    double sum = array2.MultiplyElementWiseAndSum(TransposeFunction(QuadratureIntegralFunctionByX(integral)),x,x,y1,y2);
    double denominator = GetXIntegralValue(GetX4Integral(y1,y2),y1,y2,x);
    return sum/denominator;
}

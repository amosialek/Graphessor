#include<functional>
#include"Array2D.hpp"
#include <math.h> 

std::function<double(double, double)> Multiply(std::function<double(double, double)> f, std::function<double(double, double)> g)
{
    return [f,g](double x, double y){return f(x,y) * g(x,y);};
}

///
/// integral of (x-x1)(x-x2)
///
std::function<double(double, double)> GetXSquareIntegral(double x1, double x2)
{
    return [x1,x2](double x, double y){return x*x*x/3 -(x*x)*(x1+x2)/2.0+x1*x2*x;};
}

///
/// integral of ((x-x1)(x-x2))^2
///
std::function<double(double, double)> GetX4Integral(double x1, double x2)
{
    return [x1,x2](double x, double y){return x*x*x*x*x/5 - x*x*x*(x1+x2)*x/2 + x*x*4*x1*x2*x/3 + x*x*x1*x1*x/3 + x*x*x2*x2*x/3 - x*x1*x1*x2*x - x*x1*x2*x2*x + x1*x1*x2*x2*x;};
}


double GetXIntegralValue(std::function<double(double, double)> integral, double a, double b, double y)
{
    return integral(b,y)-integral(a,y);
}

double Get2DIntegralValue(std::function<double(double, double)> integral, double x1, double x2, double y1, double y2)
{
    return integral(x2,y2)-integral(x1,y1);
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
    return [f](double x, double y){return f(floor(x)+1,floor(y)+1)-f(floor(x+1),floor(y))-f(floor(x),floor(y+1))+f(floor(x),floor(y));};
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

double GetSquareInterpolationOfRectangle(Array2D& array,int x1, int x2, int y1, int y2)
{
    std::function<double(double, double)> testingFunction = [x1,x2,y1,y2](double x, double y){return (x-x1)*(x-x2)*(y-y1)*(y-y2);};
    std::function<double(double, double)> testingFunctionSquared = Multiply(testingFunction, testingFunction);
    auto yIntegral = TransposeFunction(GetXSquareIntegral(y1,y2));
    auto xIntegral = GetXSquareIntegral(x1,x2);
    auto integral = Multiply(Multiply(xIntegral, yIntegral),[](double x, double y){return -1.0;});
    auto ySquaredIntegral = TransposeFunction(GetX4Integral(y1,y2));
    auto xSquaredIntegral = GetX4Integral(x1,x2);
    auto squaredIntegral = Multiply(xSquaredIntegral, ySquaredIntegral);
    Array2D array2 = array.GetCopy();
    double sum = array2.MultiplyElementWiseAndSum(QuadratureIntegralFunction2D(integral),x1,x2,y1,y2);
    double denominator = 0;
    for(int i=0;i<11;i++)
        for(int j=0;j<11;j++)
            denominator += testingFunctionSquared(i,j);
    double denominator2 = Get2DIntegralValue(squaredIntegral, x1, x2, y1, y2);
    return sum/denominator;
}
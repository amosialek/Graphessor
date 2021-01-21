#ifndef __PBIHELPER_HPP__
#define __PBIHELPER_HPP__

#include<functional>
#include"Array2D.hpp"
#include <math.h>

const double integralOfTestFunctionSquaredOnZeroOneRange[]{
    1.0/30,
    1.0/210,
    1.0/630,
    1.0/1386,
    1.0/2574,
    1.0/4290,
    1.0/6630,
    1.0/9690,
    1.0/13566,
    1.0/18354,
    1.0/24150
}; //for testFunction = (1-x)x(2x-1)^i

const std::function<double(double, double)> integralOfTestFunction[]{
    [](double x, double y){return (-x/3.0+0.5)*x*x;},
    [](double x, double y){return ((-0.5*x+1)*x-0.5)*x*x;},
    [](double x, double y){return (((-4.0*x/5.0+2)*x-5/3.0)*x+0.5)*x*x;},
    [](double x, double y){return ((((-x*4/3.0+4)*x-4.5)*x+7/3.0)*x-0.5)*x*x;},
    [](double x, double y){return (((((-16*x/7.0+8)*x-11.2)*x+8)*x-3)*x+0.5)*x*x;},
    [](double x, double y){return ((((((-4*x+16)*x-80/3.0)*x+24)*x-12.5)*x+11/3.0)*x-0.5)*x*x;},
    [](double x, double y){return (((((((-64*x/9.0+32)*x-432/7.0)*x+200/3.0)*x-44)*x+18)*x-13/3.0)*x+0.5)*x*x;},
    [](double x, double y){return ((((((((-12.8*x+64)*x-140)*x+176)*x-140)*x+72.8)*x-24.5)*x+5)*x-0.5)*x*x;},
    [](double x, double y){return (((((((((-256/11.0*x+128)*x-2816/9.0)*x+448)*x-416)*x+784/3.0)*x-112)*x+32)*x-17/3.0)*x+0.5)*x*x;},
    [](double x, double y){return ((((((((((-128*x/3.0+256)*x-691.2)*x+3328/3.0)*x-1176)*x+864)*x-448)*x+163.2)*x-40.5)*x+19/3.0)*x-0.5)*x*x;},
    [](double x, double y){return (((((((((((-1024*x/13.0+512)*x-16640/11.0)*x+2688)*x-3200)*x+2688)*x-1632)*x+720)*x-228)*x+50)*x-7)*x+0.5)*x*x;}
};  //for testFunction = (1-x)x(2x-1)^i

const std::function<double(double, double)> testFunctions[]{
    [](double x, double y){return (1-x)*x;},
    [](double x, double y){return (1-x)*x*(2*x-1);},
    [](double x, double y){return (1-x)*x*pow(2*x-1,2);},
    [](double x, double y){return (1-x)*x*pow(2*x-1,3);},
    [](double x, double y){return (1-x)*x*pow(2*x-1,4);},
    [](double x, double y){return (1-x)*x*pow(2*x-1,5);},
    [](double x, double y){return (1-x)*x*pow(2*x-1,6);},
    [](double x, double y){return (1-x)*x*pow(2*x-1,7);},
    [](double x, double y){return (1-x)*x*pow(2*x-1,8);},
    [](double x, double y){return (1-x)*x*pow(2*x-1,9);},
    [](double x, double y){return (1-x)*x*pow(2*x-1,10);}
};  //for testFunction = (1-x)x(2x-1)^i

std::function<double(double, double)> Multiply(std::function<double(double, double)> f, std::function<double(double, double)> g);

///
/// integral of (x-x1)(x-x2)
///
std::function<double(double, double)> GetXSquareIntegral(double x1, double x2);

///
/// integral of ((x-x1)(x-x2))^2
///
std::function<double(double, double)> GetX4Integral(double x1, double x2);

double GetXIntegralValue(std::function<double(double, double)> integral, double a, double b, double y);

double Get2DIntegralValue(std::function<double(double, double)> integral, double x1, double x2, double y1, double y2);

std::function<double(double, double)> TransposeFunction(std::function<double(double, double)> f);

std::function<double(double, double)> QuadratureIntegralFunctionByX(std::function<double(double, double)> f);

std::function<double(double, double)> QuadratureIntegralFunction2D(std::function<double(double, double)> f);

double GetSquareInterpolationOfEdge(Array2D& array, int x1, int x2, int y);

double GetSquareInterpolationOfYEdge(Array2D& array, int x, int y1, int y2);

double GetSquareInterpolationOfRectangle(Array2D& array, int x1, int x2, int y1, int y2);

double GetNthOrderInterpolationOfEdge(Array2D& array, int x1, int x2, int y, int n);

#endif // __PBIHELPER_HPP__
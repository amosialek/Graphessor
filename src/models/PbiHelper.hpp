#ifndef __PBIHELPER_HPP__
#define __PBIHELPER_HPP__

#include<functional>
#include"Array2D.hpp"

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

#endif // __PBIHELPER_HPP__
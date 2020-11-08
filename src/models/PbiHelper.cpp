#include<functional>
std::function<double(double, double)> GetXSquareIntegral(double x1, double x2)
{
    return [x1,x2](double x, double y){return x*x*x/3 -(x*x)*(x1+x2)+x1*x2*x;};
}

double GetXIntegralValue(std::function<double(double, double)> integral, double a, double b, int y)
{
    return integral(b,y)-integral(a,y);
}
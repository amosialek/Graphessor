#include "PbiHelper.hpp"
#include <cassert>
#include <lapacke.h>

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
    return integral(x2,y2) - integral(x2,y1) - integral(x1,y2) + integral(x1,y1);
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

double GetSquareInterpolationOfEdge(Array2D& array, int x1, int x2, int y)
{
    int offset = x1;
    Array2D array2 = array.GetCopy(x1,x2,y,y);
    array2.xOffset = 0;
    x1-=offset;
    x2-=offset;
    std::function<double(double, double)> integral = GetXSquareIntegral(x1,x2);
    double sum = array2.MultiplyElementWiseAndSum(QuadratureIntegralFunctionByX(integral),0,x2-x1,0,0);
    double denominator = GetXIntegralValue(GetX4Integral(x1,x2),x1,x2,y);
    if(denominator==0)
    {
        assert(sum==0 && "GetSquareInterpolationOfEdge: denominator = 0");
        if(sum==0)
            return 0.0;
    }
    return sum/denominator;
}

Array2D GetFunctionSplitToNElements(std::function<double(double, double)> f, int x1, int x2, int elements)
{
    double delta = 1.0 * (x2-x1) / elements;
    Array2D a = Array2D(elements, 1);
    for(int i=0;i<elements;i++)
        a[i][0] = f(i * delta, 0);
    return a;
}

double GetNthOrderInterpolationOfEdge(Array2D& array, int x1, int x2, int y, int n)
{
    int offset = x1;
    Array2D array2 = array.GetCopy(x1,x2,y,y);
    array2.xOffset = 0;
    array2.yOffset = 0;
    x1-=offset;
    x2-=offset;
    double sum = 0;
    Array2D functionArray = GetFunctionSplitToNElements(integralOfTestFunction[n], 0,1, x2-x1+1);
    for(int i = functionArray.width - 1; i >= 1; i--)
        functionArray[i][0]-=functionArray[i-1][0];
    sum = array2.MultiplyElementWiseAndSum(functionArray,x1,x2,0,0);
    double denominator = integralOfTestFunctionSquaredOnZeroOneRange[n];
    if(denominator==0)
    {
        assert(sum==0 && "GetNthOrderInterpolationOfEdge: denominator = 0");
        if(sum==0)
            return 0.0;
    }
    return sum/denominator;
}

double* GetInterpolationsOfEdgeOfDifferentOrders(Array2D& array, int x1, int x2, int y)
{
    const int MAX_ORDERS = 10;
    int offset = x1;
    Array2D array2 = array.GetCopy(x1,x2,y,y);
    array2.xOffset = 0;
    array2.yOffset = 0;
    x1-=offset;
    x2-=offset;
    double* RHS = new double[MAX_ORDERS];
    double* A = new double[MAX_ORDERS*MAX_ORDERS];
    for(int order=0;order<MAX_ORDERS;order++)
    {
        RHS[order] = 0;
        Array2D functionArray = GetFunctionSplitToNElements(integralOfTestFunction[order], 0, 1, x2-x1+1);
        for(int i = functionArray.width - 1; i >= 1; i--)
            functionArray[i][0] -= functionArray[i-1][0];
        RHS[order] = array2.MultiplyElementWiseAndSum(functionArray,x1,x2,0,0);
    }
    for(int order = 0; order<MAX_ORDERS;order++)
        for(int order2 = 0; order2<MAX_ORDERS;order2++)
            A[order * MAX_ORDERS + order2] = integralOfTestFunctionsMultipliedOnZeroOneRange[order+2][order2+2];
    lapack_int *ipiv = new lapack_int[MAX_ORDERS];
    LAPACKE_dgesv( LAPACK_ROW_MAJOR, MAX_ORDERS, 1, A, MAX_ORDERS, ipiv, RHS, 1 );

    delete [] A;
    return RHS;
}

double GetSquareInterpolationOfYEdge(Array2D& array, int x, int y1, int y2)
{
    int offset = y1;
    Array2D array2 = array.GetCopy(x,x,y1,y2);
    array2.yOffset = 0;
    y1-=offset;
    y2-=offset;

    std::function<double(double, double)> integral = GetXSquareIntegral(y1,y2);
    double sum = array2.MultiplyElementWiseAndSum(TransposeFunction(QuadratureIntegralFunctionByX(integral)),0,0,0,y2-y1);
    double denominator = GetXIntegralValue(GetX4Integral(y1,y2),y1,y2,x);
    if(denominator==0)
    {
        assert(sum==0 && "GetSquareInterpolationOfYEdge: denominator = 0");
        if(sum==0)
            return 0.0;
    }
    return sum/denominator;
}

double GetSquareInterpolationOfRectangle(Array2D& array, int x1, int x2, int y1, int y2)
{
    int xDiff = x2-x1;
    int yDiff = y2-y1;
    if(xDiff<=1 or yDiff<=1) //no sense of interpolating flat rectangles
        return 0;
    int xOffset = x1;
    int yOffset = y1;
    std::function<double(double, double)> testingFunction = [xDiff, yDiff](double x, double y){return (x)*(x-xDiff)*(y)*(y-yDiff);};
    std::function<double(double, double)> testingFunctionSquared = Multiply(testingFunction, testingFunction);
    auto yIntegral = TransposeFunction(GetXSquareIntegral(0,yDiff));
    auto xIntegral = GetXSquareIntegral(0,xDiff);
    auto integral = Multiply(Multiply(xIntegral, yIntegral),[](double x, double y){return -1.0;});
    auto ySquaredIntegral = TransposeFunction(GetX4Integral(0,yDiff));
    auto xSquaredIntegral = GetX4Integral(0,xDiff);
    auto squaredIntegral = Multiply(xSquaredIntegral, ySquaredIntegral);
    Array2D array2 = array.GetCopy(x1,x2,y1,y2);
    array2.xOffset = 0;
    array2.yOffset = 0;
    double sum = array2.MultiplyElementWiseAndSum(QuadratureIntegralFunction2D(integral),0,x2-x1,0,y2-y1);
    double denominator = 0;
    for(int i=0;i<xDiff;i++)
        for(int j=0;j<yDiff;j++)
            denominator += testingFunctionSquared(i,j);
    double denominator2 = Get2DIntegralValue(squaredIntegral, 0, xDiff, 0, yDiff);
    if(denominator==0)
    {
        assert(sum==0 && "GetSquareInterpolationOfRectangle: denominator = 0");
        if(sum==0)
            return 0.0;
    }
    return sum/denominator;
}
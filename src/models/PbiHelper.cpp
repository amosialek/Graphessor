#include "PbiHelper.hpp"
#include <cassert>
#include <lapacke.h>
#include "IntegralConstants.hpp"



std::function<double(double, double)> Multiply(std::function<double(double, double)> f, std::function<double(double, double)> g)
{
    return [f,g](double x, double y){return f(x,y) * g(x,y);};
}

std::function<double(double, double)> Multiply(std::function<double(double, double)> f, double c)
{
    return [f,c](double x, double y){return f(x,y) * c;};
}

std::function<double(double, double)> Add(std::function<double(double, double)> f, std::function<double(double, double)> g)
{
    return [f,g](double x, double y){return f(x,y) + g(x,y);};
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

Array2D GetFunctionSplitToNElements2D(std::function<double(double, double)> f, int x1, int x2, int y1, int y2, int elementsX, int elementsY)
{
    double deltaX = 1.0 * (x2-x1) / (elementsX - 1);
    double deltaY = 1.0 * (y2-y1) / (elementsY - 1);
    Array2D a = Array2D(elementsX, elementsY);
    for(int x=0;x<elementsX;x++)
        for(int y=0;y<elementsY;y++)
            a[x][y] = f(x * deltaX, y * deltaY);
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

std::unique_ptr<double[]> GetInterpolationsOfEdgeOfDifferentOrders(Array2D& array, int x1, int x2, int y1, int y2, int orders)
{
    const int MAX_ORDERS = 15;
    orders = std::min(orders, MAX_ORDERS); //not to fall out of range
    int xOffset = x1;
    int yOffset = y1;
    Array2D array2 = array.GetCopy(x1,x2,y1,y2);
    array2.xOffset = 0;
    array2.yOffset = 0;
    x1-=xOffset;
    x2-=xOffset;
    y1-=yOffset;
    y2-=yOffset;
    std::unique_ptr<double[]> RHS(new double[orders]);
    std::unique_ptr<double[]> A(new double[orders*orders]);
    for(int order=0;order<orders;order++)
    {
        RHS[order] = 0;
        Array2D functionArray = GetFunctionSplitToNElements(integralOfTestFunction[order+2], 0, 1, x2-x1+1+y2-y1);
        if(x2==x1)
        {
            for(int j = y2; j >= 1; j--)
                functionArray[j][0] -= functionArray[j-1][0];
            functionArray.Transpose();
        }
        else
            for(int i = x2; i >= 1; i--)
                functionArray[i][0] -= functionArray[i-1][0];
        RHS[order] = array2.MultiplyElementWiseAndSum(functionArray,x1,x2,y1,y2);
    }
    for(int order = 0; order<orders;order++)
        for(int order2 = 0; order2<orders;order2++)
            A[order * orders + order2] = integralOfTestFunctionsMultipliedOnZeroOneRange[order+2][order2+2];
    std::unique_ptr<lapack_int[]> ipiv(new lapack_int[orders]);
    LAPACKE_dgesv( LAPACK_ROW_MAJOR, orders, 1, A.get(), orders, ipiv.get(), RHS.get(), 1 );
    return RHS;
}

std::unique_ptr<double[]> GetInterpolationsOfRectangleOfDifferentOrders(Array2D& array, int x1, int x2, int y1, int y2, int orders)
{
    const int MAX_ORDERS = 15;
    orders = std::min(orders, MAX_ORDERS); //not to fall out of range
    int xDiff = x2-x1;
    int yDiff = y2-y1;
    int xOffset = x1;
    int yOffset = y1;
    if(xDiff<=1 or yDiff<=1) 
    {
        //no sense of interpolating flat rectangles
        //result with only zeros
        std::unique_ptr<double[]> result(new double[orders*orders]());
        return result;
    }
    Array2D array2 = array.GetCopy(x1,x2,y1,y2);
    array2.xOffset = 0;
    array2.yOffset = 0;
    x1-=xOffset;
    x2-=xOffset;
    y1-=yOffset;
    y2-=yOffset;
    std::unique_ptr<double[]> RHS(new double[orders*orders]);
    std::unique_ptr<double[]> A(new double[orders*orders*orders*orders]);
    for(int orderX=0;orderX<orders;orderX++)
    {
        for(int orderY=0;orderY<orders;orderY++)
        {
            RHS[orderX*orders+orderY] = 0;
            auto yIntegral = TransposeFunction(integralOfTestFunction[orderY+2]);
            auto xIntegral = integralOfTestFunction[orderX+2];   
            auto integral = Multiply(xIntegral, yIntegral);
            Array2D functionArray = GetFunctionSplitToNElements2D(integral, 0, 1, 0, 1, x2-x1+1, y2-y1+1);
            for(int i = functionArray.width - 1; i >= 1; i--)
                for(int j = functionArray.height - 1; j >= 1; j--)
                    functionArray[i][j] += functionArray[i-1][j-1] - functionArray[i][j-1] - functionArray[i-1][j];
            RHS[orderX*orders+orderY] = array2.MultiplyElementWiseAndSum(functionArray,0,x2-x1,0,y2-y1);
        }
    }
    for(int orderX1 = 0; orderX1<orders; orderX1++)
        for(int orderX2 = 0; orderX2<orders; orderX2++)
            for(int orderY1 = 0; orderY1<orders; orderY1++)
                for(int orderY2 = 0; orderY2<orders; orderY2++)  
                    A[orderX1 * orders*orders*orders + orderY1*orders*orders + orderX2*orders + orderY2] = integralOf2DTestFunctionsMultipliedOnZeroOneRange
                    [orderX1+2][orderX2+2][orderY1+2][orderY2+2];
    
    std::unique_ptr<lapack_int[]> ipiv(new lapack_int[orders*orders]);
    LAPACKE_dgesv( LAPACK_ROW_MAJOR, orders*orders, 1, A.get(), orders*orders, ipiv.get(), RHS.get(), 1 );

    return RHS;
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

std::function<double(double, double)> GetBilinearInterpolationFunctionFromOneCorner(int x1, int x2, int y1, int y2, int cornerX, int cornerY, int value)
{
    if(x1==cornerX and y1==cornerY)
        return [x1, x2, y1, y2, value](double x, double y){return value*(x2-x)/(x2-x1)*(y2-y)/(y2-y1);};
    if(x2==cornerX and y1==cornerY)
        return [x1, x2, y1, y2, value](double x, double y){return value*(x-x1)/(x2-x1)*(y2-y)/(y2-y1);};
    if(x1==cornerX and y2==cornerY)
        return [x1, x2, y1, y2, value](double x, double y){return value*(x2-x)/(x2-x1)*(y-y1)/(y2-y1);};
    if(x2==cornerX and y2==cornerY)
        return [x1, x2, y1, y2, value](double x, double y){return value*(x-x1)/(x2-x1)*(y-y1)/(y2-y1);};
    assert(false and "GetBilinearInterpolationFunctionFromOneCorner: (cornerX, cornerY) is not a corner of the rectangle");
}
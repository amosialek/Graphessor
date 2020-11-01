#include "Array2D.hpp"
#include "stdlib.h"
#include <cassert>

    Array2D::Array2D(std::vector<std::vector<double>> array)
    {
        assert(array.size()>0 && "Sequence contains no elements");
        height = array.size();
        width = array[0].size();
        for(int i=0;i<height;i++)
            assert(array[i].size()==width && "Matrix cannot have various row lenghts");
        a = array;
    }

    Array2D::Array2D(int width, int height, double value)
    {
        this -> height = height;
        this -> width = width;
        a.resize(width);
        for(auto array1D:a)
        {
            array1D.resize(height, value);
        }
    }

    Array2D::Array2D(int width, int height, double (*func)(double, double))
    {
        this -> height = height;
        this -> width = width;
        a.resize(width);
        for(int x=0;x<width;x++)
        {
            a[x].resize(height);
            for(int y=0;y<height;y++)
                a[x][y] = func(x,y);
        }

    }

    void Array2D::FillWith(double value)
    {
        for(int y=0;y<height;y++)
            for(int x=0;x<width;x++)
                a[x][y]=value;
    }

    void Array2D::Subtract(const Array2D& other)
    {
        for(int y=0;y<height;y++)
            for(int x=0;x<width;x++)
                a[x][y]-=other.a[y][x];
    }


    void Array2D::Subtract(double (*func)(double, double))
    {
        for(int y=0;y<height;y++)
            for(int x=0;x<width;x++)
                a[x][y]-=func(x,y);
    }

    double Array2D::DotProduct(double (*func)(double, double))
    {
        double sum = 0;

        for(int y=0;y<height;y++)
            for(int x=0;x<width;x++)
                sum+=a[x][y]*func(x, y);

        return sum;
    }

    double Array2D::SquaredError(Array2D& other, int x1, int x2, int y1, int y2)
    {
        double sum = 0;
        if (x2-x1==0 or y2-y1==0) return 0;

        for(int x=x1;x<=x2;x++)
            for(int y=y1;y<=y2;y++)
            {
                sum+=(other.a[x][y] - a[x][y])*(other.a[x][y] - a[x][y]);
            }
        return sum;
    }

int sign2 (int x1, int y1, int x2, int y2, int x3, int y3)
{
    return (x1 - x3) * (y2 - y3) - (x2 - x3) * (y1 - y3);
}

bool PointInTriangle (int px, int py, int x1, int y1, int x2, int y2, int x3, int y3)
{
    int d1, d2, d3;
    bool has_neg, has_pos;

    d1 = sign2(px, py, x1, y1, x2, y2);
    d2 = sign2(px, py, x2, y2, x3, y3);
    d3 = sign2(px, py, x3, y3, x1, y1);

    has_neg = (d1 < 0) && (d2 < 0) && (d3 < 0);
    has_pos = (d1 > 0) && (d2 > 0) && (d3 > 0);

    return !(has_neg && has_pos);
}


double Array2D::SquaredError(Array2D& other, int x1, int x2, int x3, int y1, int y2, int y3)
{
    double sum = 0;

    int minx = std::min(x1, std::min(x2,x3));
    int miny = std::min(y1, std::min(y2,y3));
    int maxx = std::max(x1, std::max(x2,x3));
    int maxy = std::max(y1, std::max(y2,y3));

    if (maxx-minx==0 or maxy-miny==0) 
        return 0;
    double wDenominator = 1.0/((y2-y3) * (x1-x3) + (x3-x2) * (y1-y3));
    for(int x=minx;x<=maxx;x++)
        for(int y=miny;y<=maxy;y++)
            if(PointInTriangle(x,y,x1,y1,x2,y2,x3,y3))
            {
                sum+=(other.a[x][y] - a[x][y])*(other.a[x][y] - a[x][y]);
            }
    return sum;
}

double Array2D::CompareWith(Array2D& other, int x1, int x2, int y1, int y2)
{
    double maxSum = 255.0*255*(x2-x1+1)*(y2-y1+1);
    return SquaredError(other, x1, x2, y1, y2)/maxSum;
}

double Array2D::CompareWith(Array2D& other, int x1, int x2, int x3, int y1, int y2, int y3)
{
    double maxSum = 255.0*255*abs(x1*y2+x2*y3+x3*y1-x1*y3-x2*y1-x3*y2) / 2;
    double result =  SquaredError(other, x1, x2, x3, y1, y2, y3)/maxSum;
    return result;
}

void Array2D::BilinearInterpolation(Array2D& base, int x1, int x2, int y1, int y2)
{

        int r11,r21,r12,r22;
        r11 = base.a[x1][y1];
        r21 = base.a[x2][y1];
        r12 = base.a[x1][y2];
        r22 = base.a[x2][y2];
        double denominator = 1.0*(x2-x1)*(y2-y1);
        for(int x=x1;x<=x2;x++)
            for(int y=y1;y<=y2;y++)
            {
                double coef22 = (x-x1)*(y-y1)/denominator;//r11==0 || r22==0 || r21==0 || r12==0
                double coef12 = (x2-x)*(y-y1)/denominator;
                double coef21 = (x-x1)*(y2-y)/denominator;
                double coef11 = (x2-x)*(y2-y)/denominator;
                a[x][y] = r11 * coef11 + r21 * coef21 + r12 * coef12 + r22 * coef22;
            }
}

void Array2D::BaricentricInterpolation(Array2D& base, int x1, int x2, int x3, int y1, int y2, int y3)
{

    int y2y3 = y2-y3;
    int x3x2 = x3-x2;
    int y3y1 = y3-y1;
    int y1y3 = y1-y3;
    int x1x3 = x1-x3;
    int minx = std::min(x1,(std::min(x2,x3)));
    int miny = std::min(y1,(std::min(y2,y3)));
    int maxx = std::max(x1,(std::max(x2,x3)));
    int maxy = std::max(y1,(std::max(y2,y3)));
    double w1Coefficient = 1.0/(y2y3 * x1x3 + x3x2 * y1y3);
    double w2Coefficient = w1Coefficient;
    double w1,w2,w3;
    for(int y = miny; y <= maxy; y++)
        for(int x=minx;x<=maxx;x++)//img._view[miny*(width+1)+minx][channel]==0 || img._view[miny*(width+1)+maxx][channel]==0 || img._view[maxy*(width+1)+minx][channel]==0 ||img._view[maxy*(width+1)+maxx][channel]==0
        {
            if(a[x][y]==0)
            {
                w1 = w1Coefficient*(y2y3*(x-x3) + x3x2 * (y-y3));
                w2 = w2Coefficient*(y3y1*(x-x3) + x1x3*(y-y3));
                w3 = 1 - w1 - w2;
                if(w1>=-0.01 and w2>=-0.01 and w3>=-0.01)
                    a[x][y] = w1 * base.a[x1][y1]
                            + w2 * base.a[x2][y2]
                            + w3 * base.a[x3][y3];
            }
        }
}

std::vector<double> Array2D::operator[](int x)
{
    return a[x];
}
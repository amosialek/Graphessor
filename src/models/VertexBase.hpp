#include<boost/function.hpp>

class VertexBase
{
    public:
        int x,y;
        void InvokeProduction(); 
    protected:
        boost::function<void()> Produce = NULL;
};
#ifndef __WRITERS_HPP__
#define __WRITERS_HPP__

#include<string.h>
#include "mygraph.hpp"
#include<boost/graph/graphviz.hpp>

class AbstractOutputWriter
{
    public:
        virtual void WriteItOut(std::string fileSuffix, PixelGraph &graph)
        {};
};

class DebugWriter: public AbstractOutputWriter
{
    private:
        std::string fileBaseName;
    public:
        DebugWriter(std::string fileBaseName);
        void WriteItOut(std::string fileSuffix, PixelGraph &graph) override;
};

class WriterFactory
{
    public:
        static AbstractOutputWriter* GetDebugWriter(std::string fileBaseName)
        {
            return fileBaseName.empty() 
                ?  new AbstractOutputWriter()
                :  new DebugWriter(fileBaseName);
        }
};
#endif // __WRITERS_HPP__

#ifndef __WRITERS_HPP__
#define __WRITERS_HPP__

#include<string.h>
#include "mygraph.hpp"
#include<boost/graph/graphviz.hpp> 

class AbstractOutputWriter
{
    public:
        virtual void Write([[maybe_unused]] std::string fileSuffix, [[maybe_unused]] IGraph &graph)
        {};
};

class DebugWriter: public AbstractOutputWriter
{
    private:
        std::string fileBaseName;
    public:
        DebugWriter(std::string fileBaseName);
        void Write(std::string fileSuffix, IGraph &graph) override;
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

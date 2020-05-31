#ifndef __ATTRIBUTES_HPP__
#define __ATTRIBUTES_HPP__

#include <cereal/archives/json.hpp>
class Attributes
{
    public:
        int fake;
        virtual bool GetBool(char attributeName)=0;
        virtual double GetDouble(char attributeName)=0;
        virtual void SetBool(char attributeName, bool value)=0;
        virtual void SetDouble(char attributeName, double value)=0;
    template<class Archive>
    void serialize(Archive & archive)
    {
        archive(CEREAL_NVP(fake)); // serialize things by passing them to the archive
    }
};
#endif // __ATTRIBUTES_HPP__
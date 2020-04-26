#ifndef __ATTRIBUTES_HPP__
#define __ATTRIBUTES_HPP__



class Attributes
{
    public:
        virtual bool GetBool(char attributeName)=0;
        virtual double GetDouble(char attributeName)=0;
        virtual void SetBool(char attributeName, bool value)=0;
        virtual void SetDouble(char attributeName, double value)=0;

};
#endif // __ATTRIBUTES_HPP__
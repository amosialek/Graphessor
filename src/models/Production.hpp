#ifndef PRODUCTION_HPP
#define PRODUCTION_HPP

class Production
{
    public:
        virtual ~Production(){};
        virtual void Perform()=0;
};

#endif /* PRODUCTION_HPP */

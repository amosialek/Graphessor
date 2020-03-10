#ifndef PRODUCTION_HPP
#define PRODUCTION_HPP

class Production
{
    public:
        virtual ~Production(){};
        virtual void Perform()=0;
        virtual std::vector<uint8_t> Serialize()=0;
};

#endif /* PRODUCTION_HPP */

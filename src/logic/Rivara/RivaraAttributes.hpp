#ifndef __RivaraAttributes_hpp_RivaraAttributes_hpp__
#define __RivaraAttributes_hpp_RivaraAttributes_hpp__

#include "Attributes.hpp"
#include <cereal/archives/json.hpp>
#include <cereal/types/base_class.hpp>
#include <cereal/types/polymorphic.hpp>



namespace Rivara
{
    class RivaraAttributes : public Attributes
    {
        public:
            bool GetBool(char attributeName) override;
            double GetDouble(char attributeName) override;
            void SetBool(char attributeName, bool value) override;
            void SetDouble(char attributeName, double value) override;
            bool HN, B, R;
            double L, X, Y;
            
            template<class Archive>
            void serialize(Archive & archive)
            {
                archive(cereal::base_class<Attributes>(this),CEREAL_NVP(HN), CEREAL_NVP(B), CEREAL_NVP(R), CEREAL_NVP(L), CEREAL_NVP(X), CEREAL_NVP(Y)); // serialize things by passing them to the archive
            }
    };
}
#define NODELABEL_N "N"
#define NODELABEL_E "E"
#define NODELABEL_T "T"

#define RIVARA_ATTRIBUTE_B 'B'
#define RIVARA_ATTRIBUTE_HN 'H'
#define RIVARA_ATTRIBUTE_R 'R'
#define RIVARA_ATTRIBUTE_L 'L'
#define RIVARA_ATTRIBUTE_X 'X'
#define RIVARA_ATTRIBUTE_Y 'Y'
CEREAL_REGISTER_TYPE(Rivara::RivaraAttributes)
#endif // __RivaraAttributes hpp_RivaraAttributes_hpp__
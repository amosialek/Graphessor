#ifndef __RivaraAttributes_hpp_RivaraAttributes_hpp__
#define __RivaraAttributes_hpp_RivaraAttributes_hpp__

#include "Attributes.hpp"
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

#endif // __RivaraAttributes hpp_RivaraAttributes_hpp__
#include "Comb.h"
#include "SchroederAllpass.h"

class SchroederReverb {

public:
    SchroederReverb();

    SchroederReverb(float sampleRate, float amount, bool useInterpolation);

    void setAmount(float value);

    float process(float input);

private:
    const static int numEchoes_ = 4;
    const static int numDiffusers_ = 2;

    Comb echoes_[numEchoes_];
    SchroederAllpass diffusers_[numDiffusers_];

    float sampleRate_;
    bool useInterpolation_;
    float amount_;
};
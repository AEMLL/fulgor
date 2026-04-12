#include "RingBuffer.h"

class Comb {

public:
    Comb();

    Comb(float sampleRate, float maxDelay, bool useInterpolation);

    void setSampleRate(float rate);

    void setMaxDelay(float delay);

    void setFeedback(float value);

    float process(float input, float delay);

    float process(float input);

    ~Comb();

private:
    float sampleRate_;
    float maxDelay_;
    float feedbackCoeff_;
    bool useInterpolation_;
    RingBuffer ring_;
};
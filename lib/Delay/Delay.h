#pragma once
#include "RingBuffer.h"

class Delay {

public:
    Delay();

    Delay(float sampleRate, float maxDelay, bool useInterpolation);

    void setSampleRate(float rate);

    void setMaxDelay(float delay);

    float getMaxDelay();
    
    void setLevel(float level);

    float process(float input, float delay);
    
    ~Delay();

private:
    RingBuffer ring_;
    float sampleRate_;
    float level_;
    float maxDelay_;
    bool useInterpolation_;
    
};
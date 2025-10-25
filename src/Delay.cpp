#include <cmath>
#include "Delay.h"

Delay::Delay() : Delay(44100.0, 0.0, true) {}

Delay::Delay(float sampleRate, float maxDelay, bool useInterpolation) {
    setSampleRate(sampleRate);
    setMaxDelay(maxDelay);

    level_ = 0;
    useInterpolation_ = useInterpolation;
}

void Delay::setSampleRate(float rate) {
    sampleRate_ = rate;
}

void Delay::setLevel(float level) {
    if (level_ < 1) {
        level_ = level;
    }
}

void Delay::setMaxDelay(float maxDelay) {
    if (maxDelay >= 0) {
        maxDelay_ = maxDelay;
        ring_.resize(ceilf(maxDelay * sampleRate_)+1);
    }
}

float Delay::process(float input, float delay) {
    if (delay < 0) 
    return input;
    
    if (delay > maxDelay_)
    return input;
    
    float out;
    float readPointer = delay * sampleRate_;
    
    if (useInterpolation_) {
        int indexBelow = floorf(readPointer);
        int indexAbove = ceilf(readPointer);

        float fractionAbove = readPointer - indexBelow;
		float fractionBelow = 1.0 - fractionAbove;

        out = fractionBelow * ring_[indexBelow] +
	    	  fractionAbove * ring_[indexAbove];
    } else {
        out = ring_[(int)readPointer];
    }

    out = out * level_ + input;
    ring_.push(out);
    return out;
}

Delay::~Delay() {
    // nothing to do here
}
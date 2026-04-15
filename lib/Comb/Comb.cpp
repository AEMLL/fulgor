#include "Comb.h"

#include <cmath>

Comb::Comb() : Comb(44100.0, 0.0, true) {}

Comb::Comb(float sampleRate, float maxDelay, bool useInterpolation) {
    setSampleRate(sampleRate);
    setMaxDelay(maxDelay);

    useInterpolation_ = useInterpolation;
}

void Comb::setSampleRate(float rate) {
    sampleRate_ = rate;
}

void Comb::setMaxDelay(float maxDelay) {
    if (maxDelay >= 0) {
        maxDelay_ = maxDelay;
        ring_.resize(ceilf(maxDelay * sampleRate_)+1);
    }
}

void Comb::setFeedback(float value) {
    if (value < 1) {
        feedbackCoeff_ = value;
    }
}

float Comb::process(float input, float delay) {
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

    ring_.push(input + out * feedbackCoeff_);
    return out;
}

float Comb::process(float input) {
    return process(input, maxDelay_);
}

Comb::~Comb() {}

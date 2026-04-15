#include "Chorus.h"
#include <cmath>

Chorus::Chorus() : Chorus(44100.0, true) {}

Chorus::Chorus(float sampleRate, bool useInterpolation) {
    setSampleRate(sampleRate);
    setDepth(0.3);
    setWidth(0.005);
    setDelay(0.020);
    resizeRing();
    useInterpolation_ = useInterpolation;

    std::vector<float> wavetable;
    const unsigned int wavetableSize = 512;
		
	// Populate a buffer with the first 64 harmonics of a sawtooth wave
	wavetable.resize(wavetableSize);
    for (unsigned int n = 0; n < wavetable.size(); n++) {
        wavetable[n] = sinf(2.0 * M_PI * (float)n / (float)wavetable.size());
    }

    lfo_.setup(sampleRate_, wavetable);
    lfo_.setFrequency(3);
}

void Chorus::resizeRing() {
    float maxDelay = delay_ + depth_;
    ring_.resize(maxDelay * sampleRate_);
}

void Chorus::setSampleRate(float rate) {
    sampleRate_ = rate;
}

void Chorus::setDepth(float depth) {
    depth_ = depth;
}

void Chorus::setWidth(float width) {
    width_ = width;
}

void Chorus::setDelay(float delay) {
    delay_ = delay;
}

float Chorus::process(float input) {
    // buffer inputs
    ring_.push(input);

    float delay = width_/2 * (1+lfo_.process()) + delay_;

    float out;
    float readPointer =  delay * sampleRate_;
    
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

    out = out * depth_ + input;
    return out;
}

Chorus::~Chorus() {
    // nothing to do here
}
#include "Delay.h"

Delay::Delay() : Delay(44100.0, 0.0) {}

Delay::Delay(float sampleRate, float seconds) {
    setSampleRate(sampleRate);
    setDelay(seconds);

    level_ = 0;

    // ready_ = false;
}

void Delay::setSampleRate(float rate) {
    sampleRate_ = rate;
}

void Delay::setLevel(float level) {
    if (level_ < 1) {
        level_ = level;
    }
}

unsigned long Delay::getLength() {
    return ring_.getCapacity();
}

void Delay::setDelay(float seconds) {
    if (seconds >= 0) {
        delay_ = (unsigned long)(seconds * sampleRate_);
    }

    ring_.resize(delay_+1);
}

float Delay::process(float input) {
    float out = ring_[delay_] * level_ + input;
    ring_.push(out);
    return out;
}

Delay::~Delay() {
    // nothing to do here
}
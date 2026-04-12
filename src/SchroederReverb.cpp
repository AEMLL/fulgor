#include "SchroederReverb.h"

SchroederReverb::SchroederReverb() : SchroederReverb(44100.0, 0.5, true) {}

SchroederReverb::SchroederReverb(float sampleRate, float amount, bool useInterpolation) {
    float defaultEchoDelay[4] = {0.031, 0.037, 0.041, 0.043};
    float defaultEchoFeedback[4] = {0.651, 0.599, 0.567, 0.552};

    for (int i=0; i<numEchoes_; i++) {
        echoes_[i].setSampleRate(sampleRate);
        echoes_[i].setMaxDelay(defaultEchoDelay[i]);
        echoes_[i].setFeedback(defaultEchoFeedback[i]);
    }

    float defaultDiffuserDelay[2] = {0.005, 0.0017};
    float defaultDiffuserFeedback[2] = {0.708, 0.7};

    for (int i=0; i<numDiffusers_; i++) {
        diffusers_[i].setSampleRate(sampleRate);
        diffusers_[i].setMaxDelay(defaultDiffuserDelay[i]);
        diffusers_[i].setFeedback(defaultEchoFeedback[i]);
    }

    setAmount(amount);
}

float SchroederReverb::process(float input) {
    float out = 0.0;

    for (int i=0; i<numEchoes_; i++) {
        out += echoes_[i].process(input);
    }

    for (int i=0; i<numDiffusers_; i++) {
        out = diffusers_[i].process(out);
    }

    out = input + amount_*out;
    return out;
}

void SchroederReverb::setAmount(float value) {
    amount_ = value;
}
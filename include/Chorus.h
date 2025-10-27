#include "RingBuffer.h"
#include "Wavetable.h"

class Chorus {

public:
    Chorus();

    Chorus(float sampleRate, bool useInterpolation);

    void setSampleRate(float rate);

    void setDelay(float delay);

    void setDepth(float depth);
    
    void setWidth(float width);

    float process(float input);

    ~Chorus();

private:
    void resizeRing();

    float sampleRate_;
    float depth_;
    float width_;
    float delay_;
    bool useInterpolation_;
    RingBuffer ring_;
    Wavetable lfo_;
};
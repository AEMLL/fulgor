#include "RingBuffer.h"

class Chorus {

public:
    Chorus();

    Chorus(float sampleRate);

    float process(float input);

    ~Chorus();

private:
    RingBuffer ring_;
};
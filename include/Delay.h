#include "RingBuffer.h"

class Delay {

public:
    Delay();

    Delay(float sampleRate, float seconds);

    void setSampleRate(float rate);

    void setDelay(float seconds);
    
    void setLevel(float level);

    unsigned long getLength();

    float process(float input);
    
    ~Delay();

private:
    RingBuffer ring_;
    float sampleRate_;
    float level_;
    unsigned long delay_;
    bool ready_;
    
};
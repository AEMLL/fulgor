#pragma once
#include <vector>
#include <fftw3.h>

class FFT {

public:
    FFT() {}

    FFT(unsigned int length) {setup(length);};
    
    ~FFT() {cleanup();};

    void setup(unsigned int length);

    void cleanup();

private:
    float* timeDomain = nullptr;
    fftwf_complex* freqDomain = nullptr;
    fftwf_plan p;
};
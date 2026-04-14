#include "FFT.h"
#include <fftw3.h>

void FFT::setup(unsigned int length) {
    timeDomain = (float*) fftw_malloc(sizeof(float) * length);
    freqDomain = (fftwf_complex*) fftw_malloc(sizeof(fftw_complex) * length);
    p = fftwf_plan_dft_r2c_1d(length, timeDomain, freqDomain, FFTW_ESTIMATE);
}

void FFT::cleanup() {
    fftwf_destroy_plan(p);
    fftwf_free(timeDomain);
    fftwf_free(freqDomain);
}
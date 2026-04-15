# fftw3
find_library(FFTW3_LIB fftw3f REQUIRED)
find_path(FFTW3_INCLUDE fftw3.h REQUIRED)

target_link_libraries(FFT PRIVATE ${FFTW3_LIB})
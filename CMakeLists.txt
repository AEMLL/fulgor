CMAKE_MINIMUM_REQUIRED(VERSION 3.16)
PROJECT(fulgor)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

### System libraries ###

# fftw3
find_library(FFTW3_LIB fftw3 REQUIRED)
find_path(FFTW3_INCLUDE fftw3.h REQUIRED)

### Third party ###

# libremidi
add_subdirectory(thirdparty/libremidi)

### Local libraries ###

# FFT
add_library(FFT STATIC lib/FFT/FFT.cpp)
target_include_directories(FFT PUBLIC  ${CMAKE_SOURCE_DIR}/lib/FFT PRIVATE ${FFTW3_INCLUDE})
target_link_libraries(FFT PRIVATE ${FFTW3_LIB})


# Store all executables in bin/
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/build)

# Add include directory
include_directories(${CMAKE_SOURCE_DIR}/include)

# Collect your source files
file(GLOB SOURCES ${CMAKE_SOURCE_DIR}/src/*.cpp)

# Add the executable
add_executable(app ${SOURCES})

# Tell CMake where to find your custom libraries
# link_directories(${CMAKE_SOURCE_DIR}/lib)

# Dynamically link against the math library
target_link_libraries(app PRIVATE portaudio rt m asound pthread libremidi FFT)
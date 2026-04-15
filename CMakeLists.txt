CMAKE_MINIMUM_REQUIRED(VERSION 3.16)
PROJECT(fulgor)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Third party ##########################################################

# libremidi
add_subdirectory(thirdparty/libremidi)

# Local libraries ######################################################

file(GLOB LIB_DIRS CONFIGURE_DEPENDS ${CMAKE_SOURCE_DIR}/lib/*)

function(add_internal_library LIB_NAME)
    add_library(${LIB_NAME} SHARED
        lib/${LIB_NAME}/${LIB_NAME}.cpp
    )
    target_include_directories(${LIB_NAME}
        PUBLIC ${CMAKE_SOURCE_DIR}/lib/${LIB_NAME}
    )
endfunction()

foreach(LIB_DIR ${LIB_DIRS})
    if(IS_DIRECTORY ${LIB_DIR})
        get_filename_component(LIB_NAME ${LIB_DIR} NAME)
        add_internal_library(${LIB_NAME})
    endif()
endforeach()

foreach(LIB_DIR ${LIB_DIRS})
    if(IS_DIRECTORY ${LIB_DIR})
        if(EXISTS ${LIB_DIR}/CMakeLists.txt)
            include(${LIB_DIR}/CMakeLists.txt)
        endif()
    endif()
endforeach()

# Executable ###########################################################

# Store all executables in bin/
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/build)

# Add include directory
include_directories(${CMAKE_SOURCE_DIR}/include)

# Collect your source files
file(GLOB SOURCES ${CMAKE_SOURCE_DIR}/src/*.cpp)

# Add the executable
add_executable(app ${SOURCES})

# Dynamically link against the math library
target_link_libraries(app PRIVATE 
    portaudio 
    rt
    m
    asound
    pthread 
    libremidi
    FFT
    Wavetable
    Ramp
    ADSR
    Filter
    RingBuffer
    Delay
    Chorus
    Comb
    SchroederAllpass
    SchroederReverb
)
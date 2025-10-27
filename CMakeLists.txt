CMAKE_MINIMUM_REQUIRED(VERSION 3.10)
PROJECT(fulgor)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Store all executables in bin/
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/bin)

# Add include directory
include_directories(${CMAKE_SOURCE_DIR}/include)

# Collect your source files
file(GLOB SOURCES ${CMAKE_SOURCE_DIR}/src/*.cpp)

add_subdirectory(thirdparty/libremidi)

# Add the executable
add_executable(app ${SOURCES})

# Tell CMake where to find your custom libraries
link_directories(${CMAKE_SOURCE_DIR}/lib)

# Dynamically link against the math library
target_link_libraries(app portaudio rt m asound pthread libremidi)

# Link the libremidi library
# target_link_libraries(app PRIVATE libremidi)
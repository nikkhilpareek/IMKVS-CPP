# Set the minimum required version of CMake and define the project
cmake_minimum_required(VERSION 3.10)
project(IMKVS LANGUAGES CXX)

# Set the C++ standard to C++17
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Find the Threads library, which is required for std::recursive_mutex
find_package(Threads REQUIRED)

# Define the executable and its source files
add_executable(
    imkvs
    main.cpp
    KeyValueStore.cpp
    KeyValueStore.h
    json.hpp
    picosha2.h
)

# Pass the source directory path to the C++ code
target_compile_definitions(imkvs PRIVATE "PROJECT_SOURCE_DIR=\"${CMAKE_SOURCE_DIR}\"")

# Link the necessary libraries to our executable
target_link_libraries(
    imkvs
    PRIVATE Threads::Threads
)

# Display a helpful message after configuration
message(STATUS "Configuration complete. You can now build with: cmake --build .")
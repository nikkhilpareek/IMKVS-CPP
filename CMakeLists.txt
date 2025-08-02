# Set the minimum required version of CMake and define the project
cmake_minimum_required(VERSION 3.14)
project(IMKVS LANGUAGES CXX)

# Set the C++ standard to C++17
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Find the Threads library, required for std::recursive_mutex
find_package(Threads REQUIRED)

# --- Create a library for the core KeyValueStore logic ---
# This allows us to share the code between our main app and our tests
add_library(
    kv_store
    KeyValueStore.cpp
    KeyValueStore.h
    json.hpp
    picosha2.h
)
target_link_libraries(
    kv_store
    PRIVATE Threads::Threads
)

# --- Define the main application executable ---
add_executable(
    imkvs
    main.cpp
)
# Link our main app against the KeyValueStore library
target_link_libraries(
    imkvs
    PRIVATE kv_store
)
# Pass the source directory path to the main executable
target_compile_definitions(imkvs PRIVATE "PROJECT_SOURCE_DIR=\"${CMAKE_SOURCE_DIR}\"")


# --- Configure Unit Testing with Google Test ---
# Enable testing for the project
enable_testing()

# Include FetchContent to download external libraries like Google Test
include(FetchContent)

# Declare the Google Test dependency
FetchContent_Declare(
    googletest
    URL https://github.com/google/googletest/archive/v1.14.0.zip
)

# Make the Google Test content available to our project
FetchContent_MakeAvailable(googletest)

# --- Define the test executable ---
add_executable(
    kv_tests # This will be the name of our test executable
    tests.cpp
)

# Link our test executable against our library and Google Test
target_link_libraries(
    kv_tests
    PRIVATE
    kv_store
    gtest_main
)

# Include Google Test in our test executable
include(GoogleTest)
# Discover and add the tests from our kv_tests executable
gtest_discover_tests(kv_tests)

FetchContent_Declare(
    googlebenchmark
    URL https://github.com/google/benchmark/archive/v1.8.3.zip
)

FetchContent_MakeAvailable(googlebenchmark)

add_executable(
    kv_benchmarks
    benchmarks.cpp
)

target_link_libraries(
    kv_benchmarks
    PRIVATE
    kv_store
    benchmark::benchmark_main
)
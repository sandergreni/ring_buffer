cmake_minimum_required(VERSION 3.21)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})

project(test_ring_buffer
	DESCRIPTION "Library that holds ring buffer logic"
        LANGUAGES CXX)

add_library(ring_buffer ring_buffer.cpp)

option(BUILD_TESTS "Build unit tests" OFF)
if (BUILD_TESTS)
	include(FetchContent)
	FetchContent_Declare(
		GoogleTest
		GIT_REPOSITORY https://github.com/google/googletest.git
		GIT_TAG v1.16.0
	)
	# For Windows: Prevent overriding the parent project's compiler/linker settings
	set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
	FetchContent_MakeAvailable(googletest)
	
	enable_testing()
	message(STATUS "Building tests...")
	add_executable(tests testing_ringbuffer.cpp)
	target_link_libraries(tests ring_buffer GTest::gtest)
	include(GoogleTest)	

	#include_directories(. "C:/Program Files (x86)/googletest-distribution/include")
	#link_directories("C:/Program Files (x86)/googletest-distribution/lib")
	
	gtest_discover_tests(tests)
endif()

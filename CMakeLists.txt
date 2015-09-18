cmake_minimum_required(VERSION 2.8.7)
project(variant)

set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)

add_subdirectory(google-test)

include_directories(google-test/include)

if (MSVC)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /W4")
else()
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall")
endif()
    
enable_testing()

set(SOURCE_FILES main.cpp variant.hpp)
add_executable(variant ${SOURCE_FILES})
#target_compile_features(variant PRIVATE cxx_constexpr cxx_variadic_templates)
set_property(TARGET variant PROPERTY CXX_STANDARD 14)
target_link_libraries(variant gtest)


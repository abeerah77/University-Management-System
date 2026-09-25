cmake_minimum_required(VERSION 3.10)
project(UniversityManagementSystem LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

add_executable(university_management_system main.cpp)
add_executable(ums_tests tests.cpp)

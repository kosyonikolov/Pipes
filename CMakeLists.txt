cmake_minimum_required(VERSION 3.0.0)
project(Pipes VERSION 0.1.0)

include(CTest)
enable_testing()

set(CMAKE_CXX_STANDARD 11)

add_executable(Pipes src/main.cpp src/cpp_semaphore.cpp)
target_include_directories(Pipes PRIVATE inc/)
target_link_libraries(Pipes PRIVATE pthread)

set(CPACK_PROJECT_NAME ${PROJECT_NAME})
set(CPACK_PROJECT_VERSION ${PROJECT_VERSION})
include(CPack)

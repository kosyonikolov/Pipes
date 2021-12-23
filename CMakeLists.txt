cmake_minimum_required(VERSION 3.0.0)
project(Pipes VERSION 0.1.0)

include(CTest)
enable_testing()

set(CMAKE_CXX_STANDARD 20)

add_subdirectory(MtLib)
add_subdirectory(NsLib)

add_executable(Pipes src/main.cpp)
target_include_directories(Pipes PRIVATE inc/)
target_link_libraries(Pipes PRIVATE pthread)
target_link_libraries(Pipes PRIVATE MtLib NsLib)

set(CPACK_PROJECT_NAME ${PROJECT_NAME})
set(CPACK_PROJECT_VERSION ${PROJECT_VERSION})
include(CPack)

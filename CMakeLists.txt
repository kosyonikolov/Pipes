cmake_minimum_required(VERSION 3.0.0)
project(Pipes VERSION 0.1.0)

include(CTest)
enable_testing()

set(CMAKE_CXX_STANDARD 20)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -stdlib=libc++")
#set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -stdlib=libc++ -lc++abi")

add_executable(Pipes src/main.cpp src/cpp_semaphore.cpp src/cpp20_semaphore.cpp)
target_include_directories(Pipes PRIVATE inc/)
target_link_libraries(Pipes PRIVATE pthread)

set(CPACK_PROJECT_NAME ${PROJECT_NAME})
set(CPACK_PROJECT_VERSION ${PROJECT_VERSION})
include(CPack)

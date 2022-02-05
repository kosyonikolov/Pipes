add_executable(Downscaler src/downscaler.cpp)
target_include_directories(Downscaler PRIVATE inc/)
target_link_libraries(Downscaler PRIVATE pthread)
target_link_libraries(Downscaler PRIVATE MtLib NsLib)

find_package(OpenCV REQUIRED)
target_link_libraries(Downscaler PRIVATE ${OpenCV_LIBS})
target_include_directories(Downscaler PRIVATE ${OpenCV_INCLUDES})
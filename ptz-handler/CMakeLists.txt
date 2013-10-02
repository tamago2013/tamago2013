cmake_minimum_required(VERSION 2.8 FATAL_ERROR)

project(ptz-handler)

include_directories(../ssmtype ../gndlib)
#find_package( OpenCV REQUIRED )
#link_directories(/usr/local/lib/kaw_lib.a)
#link_directories(/usr/local/lib/kaw_lib.a)
#add_definitions(-DMYDEFINITION1 -DMYDEFINITION2)

# set the default path for built executables to the "bin" directory
set(EXECUTABLE_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/Release)
# set the default path for built libraries to the "lib" directory
set(LIBRARY_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/lib)

# set the build type
set(CMAKE_BUILD_TYPE Release)
# confirmation messages
message(STATUS "CMAKE_BUILD_TYPE: ${CMAKE_BUILD_TYPE}")
message(STATUS "CMAKE_C_FLAGS: ${CMAKE_C_FLAGS}")
message(STATUS "CMAKE_CXX_FLAGS: ${CMAKE_CXX_FLAGS}")
message(STATUS "CMAKE_C_FLAGS_RELEASE: ${CMAKE_C_FLAGS_RELEASE}")
message(STATUS "CMAKE_CXX_FLAGS_RELEASE: ${CMAKE_CXX_FLAGS_RELEASE}")

# set the executable name, sources, libraries
add_executable(ptz-handler src/ptz-handler.cpp)
target_link_libraries(ptz-handler m ssm)
#target_link_libraries(color-detector ssm )


cmake_minimum_required(VERSION 2.4)

project(RasterRenderer)

SET(LIBRARY_OUTPUT_PATH ${CMAKE_BINARY_DIR}/lib CACHE INTERNAL "Lib output dir")
SET(EXECUTABLE_OUTPUT_PATH ${CMAKE_BINARY_DIR}/bin CACHE INTERNAL "Binary output dir")

# defaulting to release build
IF (NOT CMAKE_BUILD_TYPE)
  SET(CMAKE_BUILD_TYPE         "Release" CACHE STRING "Type of build (Debug Release)" FORCE)
ENDIF ()

#add_definitions(-D_DEBUG)
if(CMAKE_COMPILER_IS_GNUCXX)
   set(CMAKE_CXX_FLAGS "-std=c++0x ${CMAKE_CXX_FLAGS} -ftest-coverage -msse4.1")
endif()

if(CMAKE_COMPILER_IS_GNUCXX)
if (CMAKE_SIZEOF_VOID_P EQUAL 8)
set(TBB_DEBUG "${CMAKE_CURRENT_SOURCE_DIR}/lib/x64/libtbb_debug.so")
set(TBB_RELEASE "${CMAKE_CURRENT_SOURCE_DIR}/lib/x64/libtbb.so")
else()
set(TBB_DEBUG "${CMAKE_CURRENT_SOURCE_DIR}/lib/x86/libtbb_debug.so")
set(TBB_RELEASE "${CMAKE_CURRENT_SOURCE_DIR}/lib/x86/libtbb.so")
endif()
else()
if (CMAKE_SIZEOF_VOID_P EQUAL 8)
set(TBB_DEBUG "${CMAKE_CURRENT_SOURCE_DIR}/lib/x64/tbb_debug")
set(TBB_RELEASE "${CMAKE_CURRENT_SOURCE_DIR}/lib/x64/tbb")
else()
set(TBB_DEBUG "${CMAKE_CURRENT_SOURCE_DIR}/lib/x86/tbb_debug")
set(TBB_RELEASE "${CMAKE_CURRENT_SOURCE_DIR}/lib/x86/tbb")
endif()
endif()
add_subdirectory(../CoreLib "${CMAKE_CURRENT_BINARY_DIR}/CoreLib")
include_directories("../" "RasterRenderer" "lib")
add_subdirectory(RasterRenderer)
add_subdirectory(TestDriver)

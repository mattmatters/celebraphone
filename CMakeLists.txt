project (FaceReplace)
cmake_minimum_required(VERSION 3.0)

add_definitions(-DDLIB_NO_GUI_SUPPORT)
SET(DLIB_USE_CUDA NO)
SET(OpenCV_STATIC ON)
SET(OpenCV_DIR /opencv-3.4.0/build_wasm)
SET(PreloadPth ./shape_predictor_68_face_landmarks.dat)
SET(PreloadPth2 ./haarcascade_frontalface_alt2.xml)

find_package(OpenCV REQUIRED)
# add_subdirectory(/dlib-19.6/dlib dlib_build)

include_directories(${OpenCV_INCLUDE_DIRS} /dlib-19.6 .)

# add_library(color_cycle_lib color_cycle.cpp color_cycle.h)

if (EMSCRIPTEN)
  add_executable (face_replace face-replace.cpp /dlib-19.6/dlib/all/source.cpp)
  target_link_libraries(face_replace ${OpenCV_LIBS})

  # All the WASM flags
  SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 --llvm-lto 1 --bind -s ASSERTIONS=2 --memory-init-file 0 ") # -Oz --profiling
  SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O3 -s DISABLE_EXCEPTION_CATCHING=0")
  SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -s WASM=1 --bind -s FORCE_FILESYSTEM=1")
  SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} --preload-file ${PreloadPth} -s TOTAL_MEMORY=640000kb")
  SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -s \"EXTRA_EXPORTED_RUNTIME_METHODS=['addOnPostRun']\" --preload-file ${PreloadPth2}")
  SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -s NO_EXIT_RUNTIME=1") # reduce file size
else()
        add_executable (face_replace facce-replace.cpp /dlib-19.6/dlib/all/source.cpp)
        target_link_libraries(face_replace ${OpenCV_LIBS})
endif()


if(DEFINED CMAKE_BUILD_TYPE)
  SET(CMAKE_BUILD_TYPE ${CMAKE_BUILD_TYPE})
else()
  SET(CMAKE_BUILD_TYPE Release)
endif()

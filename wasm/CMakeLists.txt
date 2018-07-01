project (FaceReplace)
cmake_minimum_required(VERSION 3.0)

add_definitions(-DDLIB_NO_GUI_SUPPORT)
SET(DLIB_USE_CUDA NO)
SET(OpenCV_STATIC ON)
SET(OpenCV_DIR /src/opencv-3.4.0/build_wasm)
SET(PreloadPth ./haarcascade_frontalface_alt2.xml)
SET(PreloadPthHigh ./shape_predictor_68_face_landmarks.dat)

find_package(OpenCV REQUIRED)
include_directories(${OpenCV_INCLUDE_DIRS} /src/dlib-19.6 .)


if (EMSCRIPTEN)
  SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 --llvm-lto 1 --bind -s ASSERTIONS=2 --memory-init-file 0")

  if (DEBUG_BUILD)
    SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O0 -g4 -s DEMANGLE_SUPPORT=1")
  else()
    SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Oz")
  endif()

  SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -s DISABLE_EXCEPTION_CATCHING=0")
  SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -s WASM=1 --bind -s FORCE_FILESYSTEM=1")
  SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} --preload-file ${PreloadPth} -s TOTAL_MEMORY=640000kb")
  SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -s \"EXTRA_EXPORTED_RUNTIME_METHODS=['addOnPostRun']\"")

  # We bundle a different preload file depending on the size
  if(LOW_QUALITY)
    add_executable (face_replace_low face-detect.cpp face-replace.cpp /src/dlib-19.6/dlib/all/source.cpp)
    target_link_libraries(face_replace_low ${OpenCV_LIBS})
    SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} --preload-file ${PreloadPthHigh}")
  else()
    add_executable (face_replace_high face-detect.cpp face-replace.cpp /src/dlib-19.6/dlib/all/source.cpp)
    target_link_libraries(face_replace_high ${OpenCV_LIBS})
    SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} --preload-file ${PreloadPthHigh}")
  endif()

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

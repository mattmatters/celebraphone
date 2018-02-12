project (FaceReplace)
cmake_minimum_required(VERSION 3.0)

add_definitions(-DDLIB_NO_GUI_SUPPORT)
SET(DLIB_USE_CUDA NO)
set(OpenCV_STATIC ON)
set(OpenCV_DIR /opencv-3.4.0/build_wasm)
SET(PreloadPth ./shape_predictor_68_face_landmarks.dat)
SET(PreloadPth2 ./dmx.jpg)
find_package(OpenCV REQUIRED)
# add_subdirectory(/dlib-19.6/dlib dlib_build)

include_directories(${OpenCV_INCLUDE_DIRS} /dlib-19.6 .)

# add_library(color_cycle_lib color_cycle.cpp color_cycle.h)

if (EMSCRIPTEN)
        add_executable (face_replace face-replace.cpp /dlib-19.6/dlib/all/source.cpp) #web/color_cycle_js.cpp)
        target_link_libraries(face_replace ${OpenCV_LIBS})

   # copy files to docs folder
        # configure_file(${CMAKE_SOURCE_DIR}/web/index.html        ${CMAKE_SOURCE_DIR}/docs/demo.html      COPYONLY)
        # configure_file(${CMAKE_SOURCE_DIR}/web/color_cycle.js    ${CMAKE_SOURCE_DIR}/docs/color_cycle.js COPYONLY)

   # post build events - copy files to hosted demo folder
   # add_custom_command(TARGET color_cycle_asm POST_BUILD              # Adds a post-build event to color_cycle_asm
   #                    COMMAND ${CMAKE_COMMAND} -E copy_if_different  # which executes "cmake - E copy_if_different..."
   #                    ${CMAKE_SOURCE_DIR}/data/sirocco_w320.mp4      # <--this is in-file
   #                    ${CMAKE_SOURCE_DIR}/docs/sirocco.mp4)          # <--this is out-file path

   # add_custom_command(TARGET color_cycle_asm POST_BUILD              # Adds a post-build event to color_cycle_asm
   #                    COMMAND ${CMAKE_COMMAND} -E copy_if_different  # which executes "cmake - E copy_if_different..."
   #                    ${CMAKE_CURRENT_BINARY_DIR}/color_cycle_asm.js # <--this is in-file
   #                    ${CMAKE_SOURCE_DIR}/color_cycle_asm.js)   # <--this is out-file path

else()
        add_executable (color_cycle main.cpp)
        target_link_libraries(color_cycle ${OpenCV_LIBS} color_cycle_lib)
endif()

SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 --llvm-lto 1 --bind -s ASSERTIONS=2 --memory-init-file 0 -O1 -s DISABLE_EXCEPTION_CATCHING=0") # -Oz --profiling
SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -s WASM=1 --bind -s FORCE_FILESYSTEM=1 -o cow.html --preload-file ${PreloadPth} -s TOTAL_MEMORY=640000kb") # WASM Stuff
SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -s \"EXTRA_EXPORTED_RUNTIME_METHODS=['addOnPostRun']\"") # WASM Stuff #
SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -s NO_EXIT_RUNTIME=1 -o cow2.html") # reduce file size

if(DEFINED CMAKE_BUILD_TYPE)
  SET(CMAKE_BUILD_TYPE ${CMAKE_BUILD_TYPE})
else()
  SET(CMAKE_BUILD_TYPE Release)
endif()

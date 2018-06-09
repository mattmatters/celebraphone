#!/bin/bash

# Stuff for wasm
source /usr/emsdk/emsdk_env.sh

rm -rf web

# Build directories
mkdir web
mkdir web/wasm
mkdir web/static

cp index.html web

cp static/* web/static

# Now build
emcmake cmake
emmake make

cp face_* web

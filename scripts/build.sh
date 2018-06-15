#!/bin/bash

# Stuff for wasm
source /usr/emsdk/emsdk_env.sh

rm -rf dist

# Build directories
mkdir dist
mkdir dist/wasm
mkdir dist/static


cp index.html dist

# Static stuff
cp favicon.ico dist
cp static/* dist/static

# Now build
emcmake cmake
emmake make

cp face_* dist/wasm

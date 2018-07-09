#!/bin/bash

rm -rf dist/wasm

# Build directories
if [[ ! -d ./dist ]]; then
    mkdir ./dist
fi

if [[ ! -d ./dist/wasm ]]; then
    mkdir ./dist/wasm
fi

# Now build
emcmake cmake
emmake make

cp face_* dist/wasm
cp bindings_high.js dist/wasm/bindings.js
cp *.js dist/wasm

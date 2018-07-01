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

cat premodule.js face_replace_high.js bindings_high.js worker.js > dist/wasm/replacer.high.js

cp face_* dist/wasm

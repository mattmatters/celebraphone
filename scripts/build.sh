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

cat premodule.js face_replace.js bindings.js worker.js > dist/wasm/replacer.bundle.js

cp face_* dist/wasm

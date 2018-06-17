#!/bin/bash

if [[ ! -d ./dist ]]; then
    mkdir ./dist
fi

cp --recursive ./static ./dist

cp --recursive ./js ./dist

cp ./index.html ./dist

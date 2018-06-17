#!/bin/bash

NAME="face_replace"

pip install awscli
aws s3 cp dist $BUCKET --recursive --exclude *.wasm --exclude *.data

# The aws cli guesses wrong for this file
aws s3 cp dist/wasm/$NAME.wasm $BUCKET/wasm/$NAME.wasm --content-type "application/wasm"

# Compress data file
gzip -9 dist/wasm/$NAME.data && \
  mv dist/wasm/$NAME.data.gz dist/wasm/$NAME.data
aws s3 cp dist/wasm/$NAME.data $BUCKET/wasm/$NAME.data --content-encoding "gzip"

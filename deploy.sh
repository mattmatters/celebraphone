#!/bin/bash

NAME="face_replace"

pip install awscli
aws s3 cp web $BUCKET --recursive --exclude *.wasm --exclude *.data

# The aws cli guesses wrong for this file
aws s3 cp web/$NAME.wasm $BUCKET --content-type "application/wasm"

# Compress data file
gzip -9 web/$NAME.data
mv web/$NAME.data.gz web/$NAME.data
aws s3 cp web/$NAME.data $BUCKET --content-encoding "gzip"

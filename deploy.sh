#!/bin/bash

NAME="face_replace"

pip install awscli
aws s3 cp . $BUCKET --recursive --exclude *.wasm --exclude *.data

# The aws cli guesses wrong for this file
aws s3 cp $NAME.wasm $BUCKET --content-type "application/wasm"

# Compress data file
gzip -9 $NAME.data
mv $NAME.data.gz $NAME.data
aws s3 cp $NAME.data $BUCKET --content-encoding "gzip"

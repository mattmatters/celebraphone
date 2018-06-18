# [Realtime Face Replacer](https://celebraphone.io)
[![pipeline status](https://gitlab.com/mattmatters/opencv-wasm/badges/master/pipeline.svg)](https://gitlab.com/mattmatters/opencv-wasm/commits/master)

Identify and manipulate faces in a web browser.

The facial recognition software uses a combination of OpenCV, DLib, and some
custom C++ code all compiled into web assembly.  The front end utilizes
Javascript's new found love for primitive data types to pass pointers to and
from native machine code to modify images.

## Running locally

To run locally, make sure you have docker up and type `docker-compose up --build`.

## Future

The end goal is to implement a real time Hollywood squares type game with people
communicating over WebRTC. However for now it's an nice prototype.

## Disclaimer

This is extremely rough still with a laundry list of things still to do

* A bunch of optimizations still to be made, mostly will allocating the image data in web assembly land
* the build process needs to be completely overhauled
* the way the docker file copies libraries is not exactly elegant
* the static and web page stuff needs to get its own home.

That being said, I am extremely proud of what is currently there.

## Testing

Testing is a bit of a pain, because it mainly needs to be tested in a variety of browser environments.

I am using Browser Stack for most of the testing portions.

[![BrowserStack](/static/Browserstack-logo.svg)](https://www.browserstack.com)

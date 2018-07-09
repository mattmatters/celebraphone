<h1 align="center">Wasm Face Recognition</h1>
<p align="center">
    <a href="https://gitlab.com/mattmatters/opencv-wasm/commits/master"><img alt="pipeline status" src="https://gitlab.com/mattmatters/opencv-wasm/badges/master/pipeline.svg" /></a>
    <a href="https://www.browserstack.com/automate/public-build/WlM3bkVOdnplNUpXYkwzeSsvOVEzU1BEbzJoTU41anhhMlN5N1F5VXNQMD0tLW8xNSsxOHZCT3A1dmJ4N011RUROcmc9PQ==--c3d9d9808fc1e4f9292c315c64c6ec94b54a6a83%"><img alt="BrowserStack Status" src="https://www.browserstack.com/automate/badge.svg?badge_key=WlM3bkVOdnplNUpXYkwzeSsvOVEzU1BEbzJoTU41anhhMlN5N1F5VXNQMD0tLW8xNSsxOHZCT3A1dmJ4N011RUROcmc9PQ==--c3d9d9808fc1e4f9292c315c64c6ec94b54a6a83%" /></a>
</p>

<p align="center">
    <img align="center" src="static/demo.gif"></img>
</p>

Identify and manipulate faces in a web browser.

The facial recognition software uses a combination of OpenCV, DLib, and some
custom C++ code all compiled into web assembly.  The front end utilizes
Javascript's new found love for primitive data types to pass pointers to and
from native machine code to modify images.

Our sister project [Celebraphone](https://gitlab.com/mattmatters/celebraphone) uses this for p2p video chatting in a browser. You can find that project [here](https://celebraphone.io).

The project also uses a base image for compilation that you can find the source for [here](https://github.com/mattmatters/docker-emscripten-opencv-dlib) and pull from [here](https://hub.docker.com/r/superpolkadance/emscripten-opencv-dlib/).

## Running locally

To run locally, make sure you have docker up and type `docker-compose up --build`.

## Thank You Everyone!

This project was made possible by some of the people below.

<img src="/static/Browserstack-logo.svg" width="300" />

FROM ubuntu:17.10

# Update distro and install essentials
RUN apt-get update && apt-get -y install git \
    build-essential \
    cmake \
    clang \
    wget \
    bzip2 \
    unzip \
    libx11-dev \
    libatlas-base-dev \
    libgtk-3-dev \
    libboost-python-dev \
    libopencv-dev \
    gcc-multilib \
    g++-multilib

RUN cd /usr

# Install emcc
RUN cd /usr && \
    git clone https://github.com/juj/emsdk.git && \
    cd emsdk && \
    ./emsdk install latest && \
    ./emsdk activate latest && \
    /bin/bash -c "source /usr/emsdk/emsdk_env.sh" && \
    cd ..

# Get latest opencv
RUN wget https://github.com/opencv/opencv/archive/3.4.0.zip && \
    unzip 3.4.0.zip && \
    cd opencv-3.4.0 && \
    /bin/bash -c "source /usr/emsdk/emsdk_env.sh && \
                  python ./platforms/js/build_js.py build_wasm --build_wasm"

# Update to latest cmake, current one has a nasty bug that affects the project
RUN apt-get purge -y cmake && \
    mkdir temp-c && \
    cd temp-c && \
    wget https://cmake.org/files/v3.10/cmake-3.10.0.tar.gz && \
    tar -xzvf cmake-3.10.0.tar.gz && \
    cd cmake-3.10.0 && \
    ./bootstrap && \
    make -j4 && \
    make install


# RUN mkdir buildthing && \
#     cd buildthing && \
#     /bin/bash -c "source /usr/emsdk/emsdk_env.sh && emcmake cmake --no DLIB_USE_CUDA /dlib-19.6/dlib"

WORKDIR /usr/src/app

RUN cd /usr/src/app
RUN wget https://raw.githubusercontent.com/opencv/opencv/master/data/haarcascades/haarcascade_frontalface_alt2.xml && \
    wget https://raw.githubusercontent.com/kurnianggoro/GSOC2017/master/data/lbfmodel.yaml

COPY . .

# Set up for building
RUN /bin/bash -c "source /usr/emsdk/emsdk_env.sh && emcmake cmake && emmake make"

CMD /bin/bash -c "source /usr/emsdk/emsdk_env.sh && bash"

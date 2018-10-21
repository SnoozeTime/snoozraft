FROM ubuntu:18.04 AS base

# Dependencies are zmq and boost
RUN apt-get update && apt-get install -y \
    libzmq3-dev \
    libboost-all-dev \
    python3-pip

RUN pip3 install conan

# now tha application. Everytime a file changes in our folder, the cache
# here and all the following lines will be invalidated.
COPY . /app
WORKDIR . /app

RUN mkdir build && conan install .. && cmake .. && make



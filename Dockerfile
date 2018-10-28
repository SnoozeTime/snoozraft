FROM ubuntu:18.04 AS base

# Dependencies are zmq and boost
RUN apt-get update && apt-get install -y \
    libzmq3-dev \
    libboost-all-dev \
    python3-pip

RUN pip3 install conan

RUN apt-get install -y cmake

# now tha application. Everytime a file changes in our folder, the cache
# here and all the following lines will be invalidated.

FROM base as step1
COPY conanfile.txt /app/conanfile.txt
WORKDIR /app

RUN mkdir build && cd build && conan install ..

RUN apt-get install -y git
RUN git clone https://github.com/msgpack/msgpack-c.git /app/msgpack

RUN cd /app/msgpack && mkdir build && cd build && cmake .. && make && make install

# Just compile the code
FROM step1 as step2

COPY . /app
RUN cd /app/build && cmake .. && make

CMD ["./build/bin/main", "--config_type", "ENV"]


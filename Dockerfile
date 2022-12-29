
FROM ubuntu:jammy

RUN apt-get update

RUN apt-get install -y llvm-14 llvm-14-runtime llvm-14-dev cmake g++ bison flex

RUN ln -s /usr/bin/llvm-config-14 /usr/local/bin/llvm-config

WORKDIR /usr/local/src/

# Copy over everything
ADD bindings/ /usr/local/src/aotrc/bindings/
ADD src/ /usr/local/src/aotrc/src/
ADD tests/ /usr/local/src/aotrc/tests/
ADD thirdparty/ /usr/local/src/aotrc/thirdparty/
COPY CMakeLists.txt /usr/local/src/aotrc/

WORKDIR /usr/local/src/aotrc/

RUN cmake -B ./build -DCMAKE_BUILD_TYPE=Release

RUN cmake --build ./build --config Release

RUN ctest -C Release

# Basic gRPC
## Install gRPC
```
$ export INSTALL_DIR=$HOME/.local
$ mkdir -p $INSTALL_DIR
$ export PATH="$INSTALL_DIR/bin:$PATH"
$ git clone --recurse-submodules -b v1.37.1 https://github.com/grpc/grpc
$ cd grpc
$ mkdir -p cmake/build
$ pushd cmake/build
$ cmake -DgRPC_INSTALL=ON -DgRPC_BUILD_TESTS=OFF -DCMAKE_INSTALL_PREFIX=$INSTALL_DIR ../..
$ make -j
$ make install
$ popd
$ mkdir -p third_party/abseil-cpp/cmake/build
$ pushd third_party/abseil-cpp/cmake/build
$ cmake -DCMAKE_INSTALL_PREFIX=$INSTALL_DIR -DCMAKE_POSITION_INDEPENDENT_CODE=TRUE ../..
$ make -j
$ make install
$ popd
```

When building the `cpp/helloworld` example, you might encounter the `fatal error: absl/synchronization/mutex.h: 
No such file or directory` error. This could be fixed by copying the absl folder from `grpc/third_party/abseil-cpp/absl` 
to `/usr/local/include/`.

```
$ (sudo) cp -r grpc/third_party/abseil-cpp/absl /usr/local/include/
```

## Example: Sample service

### Define a service

#### Instruction
Define a service using protocol buffers IDL in a `.proto` file.

#### Example
[sample.proto](protos/sample.proto)

```
syntax = "proto3";

option java_package = "sample.grpc";

package sample;

service SampleService {
    rpc SampleMethod (SampleRequest) returns (SampleResponse) {}
}

message SampleRequest {
    string request_sample_field = 1;
}

message SampleResponse {
    string response_sample_field = 1;
}
```

### Generate gRPC code

#### Instruction
Use the protocol buffer compiler `protoc` to generate client and server code:
```
$ protoc -I=$SRC_DIR --cpp_out=$DST_DIR $SRC_DIR/sample.proto
$ protoc -I=$SRC_DIR --grpc_out=$DST_DIR --plugin=protoc-gen-grpc=/usr/local/bin/grpc_cpp_plugin $SRC_DIR/sample.proto
```
where:
* `SRC_DIR`: The source directory, or the directory contains the `.proto` file.
* `DST_DIR`: The destination directory, or the directory contains the `.pb.h`, `.pb.cc`, `.grpc.pb.h` and `.grpc.pb.cc` newly generated files.

#### Example
With `SRC_DIR = protos/` and `DST_DIR = sample/`:
```
$ protoc -I=protos/ --cpp_out=sample/ protos/sample.proto
$ protoc -I=protos/ --grpc_out=sample/ --plugin=protoc-gen-grpc=/usr/local/bin/grpc_cpp_plugin protos/sample.proto
```

#### Note
These files should be generated automatically by the CMake's `add_custom_command` command and should not be included in the actual project.

### Write a client

#### Instruction
1. Create a channel.
2. Create a stub.
3. Make a unary RPC.
4. Check returned status and response.

#### Example
[sample_client.cc](sample/sample_client.cc)

### Write a server

#### Instruction
1. Implement the service interface.
2. Build a server exporting the service.

#### Example
[sample_server.cc](sample/sample_server.cc)
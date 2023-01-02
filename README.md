# gRPC C++
## Installing

### How to install

To install **gRPC**, run the following commands:

```bash
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

### Note

If you encounter the `fatal error: absl/synchronization/mutex.h: 
No such file or directory` error when building the [Hello world](https://github.com/grpc/grpc/tree/master/examples/cpp/helloworld) example, simply fix it by copying the `absl` directory from `grpc/third_party/abseil-cpp/absl` 
to `/usr/local/include/`:

```
$ (sudo) cp -r grpc/third_party/abseil-cpp/absl /usr/local/include/
```

## Working with gRPC

### How to define a service

#### Instruction
Define a service in a `.proto` file using the **Interface Definition Language (IDL)** from **Protocol Buffers**.

#### Example: [Sample service](protos/sample/v1/sample.proto)
```protobuf
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

#### Example: [Calculator service](protos/calculator/v1/calculator.proto)

#### Note

### How to generate gRPC code

#### Instruction
Use the protocol buffer compiler `protoc` to generate client and server code:
```bash
$ protoc -I=$SRC_DIR --cpp_out=$DST_DIR $SRC_DIR/sample.proto
$ protoc -I=$SRC_DIR --grpc_out=$DST_DIR --plugin=protoc-gen-grpc=/usr/local/bin/grpc_cpp_plugin $SRC_DIR/sample.proto
```
where:
* `SRC_DIR`: The source directory, or the directory contains the `.proto` file.
* `DST_DIR`: The destination directory, or the directory contains the `.pb.h`, `.pb.cc`, `.grpc.pb.h` and `.grpc.pb.cc` files.

#### Example: Sample
With `SRC_DIR = protos/` and `DST_DIR = sample/`:
```bash
$ protoc -I=protos/ --cpp_out=sample/ protos/sample.proto
$ protoc -I=protos/ --grpc_out=sample/ --plugin=protoc-gen-grpc=/usr/local/bin/grpc_cpp_plugin protos/sample.proto
```

#### Example: Calculator

With `SRC_DIR = protos/` and `DST_DIR = calculator/`:

```bash
$ protoc -I=protos/ --cpp_out=calculator/ protos/calculator.proto
$ protoc -I=protos/ --grpc_out=calculator/ --plugin=protoc-gen-grpc=/usr/local/bin/grpc_cpp_plugin protos/calculator.proto
```

#### Note

The `.pb.h`, `.pb.cc`, `.grpc.pb.h` and `.grpc.pb.cc` files could be generated automatically by the CMake's `add_custom_command` command and should not be included in the actual project. See also: [Sample CMakeLists.txt](sample/CMakeLists.txt), [Calculator CMakeLists.txt](calculator/CMakeLists.txt).

### How to write a client

#### Instruction
1. Create a channel.
2. Create a stub.
3. Make a unary RPC.
4. Check returned status and response.

#### Example: [Sample client](sample/sample_client.cc)
#### Example: [Calculator client](calculator/calculator_client.cc)

#### Note

### How to write a server

#### Instruction
1. Implement the service interface.
2. Build a server exporting the service.

#### Example: [Sample server](sample/sample_server.cc)
#### Example: [Calculator server](calculator/calculator_server.cc)

#### Note

### How to write an async client

#### Instruction

1. Create a channel.
2. Create a stub.
3. Initiate the RPC and bind it to a `CompletionQueue`.
4. Request to update the response and the call status upon completion of the RPC with a unique tag.
5. Wait for the completion queue to return the next tag.

#### Example: [Sample async client](sample/sample_async_client.cc)

#### Example: [Calculator async client](calculator/calculator_async_client.cc)

#### Note

### How to write an async server

#### Instruction

1. Build a server exporting the async service.
2. Request an RPC with a unique tag.
3. Wait for the completion queue to return the next tag.

#### Example: [Sample async server](sample/sample_async_server.cc)

#### Example: [Calculator async server](calculator/calculator_async_server.cc)

#### Note

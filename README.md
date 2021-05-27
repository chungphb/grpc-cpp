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

**Note:** When building the `cpp/helloworld` example, we might encounter the `fatal error: absl/synchronization/mutex.h: 
No such file or directory`. This can be fixed by copying the absl folder from `grpc/third_party/abseil-cpp/absl` to `/usr/
local/include/`:

```
$ (sudo) cp -r grpc/third_party/abseil-cpp/absl /usr/local/include/
```

## Example: Sample service
### Define a service
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
* For client
  * `$ protoc -I=$SRC_DIR --cpp_out=$DST_DIR $SRC_DIR/sample.proto`
  * **Example:** $ protoc -I=protobuf/ --cpp_out=. protobuf/sample.proto
    
* For server
  * `$ protoc -I=$SRC_DIR --grpc_out=$DST_DIR --plugin=protoc-gen-grpc=/usr/local/bin/grpc_cpp_plugin $SRC_DIR/sample.proto`
  * **Example:** $ protoc -I=protobuf/ --grpc_out=. --plugin=protoc-gen-grpc=/usr/local/bin/grpc_cpp_plugin protobuf/sample.proto
get_filename_component(calculator_proto "../protos/calculator.proto" ABSOLUTE)
get_filename_component(calculator_proto_path "${calculator_proto}" PATH)

set(calculator_proto_sources "${CMAKE_CURRENT_BINARY_DIR}/calculator.pb.cc")
set(calculator_proto_headers "${CMAKE_CURRENT_BINARY_DIR}/calculator.pb.h")
set(calculator_grpc_sources "${CMAKE_CURRENT_BINARY_DIR}/calculator.grpc.pb.cc")
set(calculator_grpc_headers "${CMAKE_CURRENT_BINARY_DIR}/calculator.grpc.pb.h")
add_custom_command(
        OUTPUT "${calculator_proto_sources}" "${calculator_proto_headers}" "${calculator_grpc_sources}" "${calculator_grpc_headers}"
        COMMAND ${_PROTOBUF_PROTOC}
        ARGS --grpc_out "${CMAKE_CURRENT_BINARY_DIR}" --cpp_out "${CMAKE_CURRENT_BINARY_DIR}"
        -I "${calculator_proto_path}"
        --plugin=protoc-gen-grpc="${_GRPC_CPP_PLUGIN_EXECUTABLE}"
        "${calculator_proto}"
        DEPENDS "${calculator_proto}")

include_directories("${CMAKE_CURRENT_BINARY_DIR}")

add_library(calculator_grpc_proto
        ${calculator_proto_sources}
        ${calculator_proto_headers}
        ${calculator_grpc_sources}
        ${calculator_grpc_headers})
target_link_libraries(calculator_grpc_proto
        ${_PROTOBUF_LIBPROTOBUF}
        ${_REFLECTION}
        ${_GRPC_GRPCPP})

foreach(_target calculator_client calculator_server)
    add_executable(${_target} "${_target}.cc")
    target_link_libraries(${_target}
            calculator_grpc_proto
            ${_PROTOBUF_LIBPROTOBUF}
            ${_REFLECTION}
            ${_GRPC_GRPCPP})
endforeach()
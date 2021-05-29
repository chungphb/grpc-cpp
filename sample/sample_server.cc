#include "sample.grpc.pb.h"
#include <grpc++/grpc++.h>
#include <memory>
#include <iostream>

using grpc::Server;
using grpc::ServerBuilder;
using grpc::ServerContext;
using grpc::Status;
using sample::SampleRequest;
using sample::SampleResponse;
using sample::SampleService;

class SampleServiceImpl final : public SampleService::Service {
    Status SampleMethod(ServerContext* context, const SampleRequest* request, SampleResponse* response) override {
        response->set_response_sample_field("Hello " + request->request_sample_field());
        return Status::OK;
    }
};

void RunServer() {
    std::string server_address{"localhost:2510"};
    SampleServiceImpl service;

    ServerBuilder builder;
    builder.AddListeningPort(server_address, grpc::InsecureServerCredentials());
    builder.RegisterService(&service);

    std::unique_ptr<Server> server{builder.BuildAndStart()};
    std::cout << "Server listening on " << server_address << std::endl;
    server->Wait();
}

int main(int argc, char** argv) {
    RunServer();
    return 0;
}
#include "sample.grpc.pb.h"
#include <grpc++/grpc++.h>
#include <memory>
#include <iostream>

using grpc::Channel;
using grpc::ClientContext;
using grpc::Status;
using sample::SampleRequest;
using sample::SampleResponse;
using sample::SampleService;

class SampleClient {
public:
    SampleClient(std::shared_ptr<Channel> channel) : _stub{SampleService::NewStub(channel)} {}

    std::string SampleMethod(const std::string& request_sample_field) {
        SampleRequest request;
        request.set_request_sample_field(request_sample_field);
        SampleResponse response;
        ClientContext context;

        Status status = _stub->SampleMethod(&context, request, &response);
        if (status.ok()) {
            return response.response_sample_field();
        } else {
            std::cerr << status.error_code() << ": " << status.error_message() << std::endl;
            return "RPC Failed";
        }
    }

private:
    std::unique_ptr<SampleService::Stub> _stub;
};

int main(int argc, char** argv) {
    SampleClient client{grpc::CreateChannel("localhost:2510", grpc::InsecureChannelCredentials())};
    std::string request_sample_field{"world"};
    std::string response_sample_field = client.SampleMethod(request_sample_field);
    std::cout << "Client received: " << response_sample_field << std::endl;
    return 0;
}
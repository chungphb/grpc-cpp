#include <sample.grpc.pb.h>
#include <grpc++/grpc++.h>
#include <memory>
#include <thread>
#include <iostream>

using grpc::Channel;
using grpc::ClientAsyncResponseReader;
using grpc::ClientContext;
using grpc::CompletionQueue;
using grpc::Status;
using sample::SampleRequest;
using sample::SampleResponse;
using sample::SampleService;

namespace ns1 {

class SampleClient {
public:
    SampleClient(std::shared_ptr<Channel> channel) : _stub{SampleService::NewStub(channel)} {}

    std::string SampleMethod(const std::string& request_sample_field) {
        SampleRequest request;
        request.set_request_sample_field(request_sample_field);
        SampleResponse response;
        ClientContext context;
        CompletionQueue queue;
        Status status;

        std::unique_ptr<ClientAsyncResponseReader<SampleResponse>> rpc;
        rpc = _stub->PrepareAsyncSampleMethod(&context, request, &queue);
        rpc->StartCall();
        rpc->Finish(&response, &status, (void*)1);

        void* tag;
        bool ok = false;
        if (queue.Next(&tag, &ok) && ok && tag == (void*)1) {
            if (status.ok()) {
                return response.response_sample_field();
            } else {
                return "RPC failed";
            }
        } else {
            std::cerr << "Something went wrong" << std::endl;
            abort();
        }
    }
private:
    std::unique_ptr<SampleService::Stub> _stub;
};

void RunClient() {
    SampleClient client{grpc::CreateChannel("localhost:2511", grpc::InsecureChannelCredentials())};
    std::string request_sample_field{"world"};
    std::string response_sample_field = client.SampleMethod(request_sample_field);
    std::cout << "Client received: " << response_sample_field << std::endl;
}

}

namespace ns2 {

class SampleClient {
public:
    SampleClient(std::shared_ptr<Channel> channel) : _stub{SampleService::NewStub(channel)} {}

    void SampleMethod(const std::string& request_sample_field) {
        SampleRequest request;
        request.set_request_sample_field(request_sample_field);
        auto* response_handler = new ResponseHandler;
        response_handler->rpc = _stub->PrepareAsyncSampleMethod(&response_handler->context, request, &_queue);
        response_handler->rpc->StartCall();
        response_handler->rpc->Finish(&response_handler->response, &response_handler->status, (void*)response_handler);
    }

    void CompleteRPC() {
        void* tag;
        bool ok = false;
        while (_queue.Next(&tag, &ok)) {
            auto* response_handler = static_cast<ResponseHandler*>(tag);
            if (ok) {
                std::cout << "Client received: ";
                if (response_handler->status.ok()) {
                    std::cout << response_handler->response.response_sample_field() << std::endl;
                } else {
                    std::cout << "RPC failed" << std::endl;
                }
                delete response_handler;
            } else {
                delete response_handler;
                std::cerr << "Something went wrong" << std::endl;
                abort();
            }
        }
    }

private:
    struct ResponseHandler {
        SampleResponse response;
        ClientContext context;
        Status status;
        std::unique_ptr<ClientAsyncResponseReader<SampleResponse>> rpc;
    };
    std::unique_ptr<SampleService::Stub> _stub;
    CompletionQueue _queue;
};

void RunClient() {
    SampleClient client{grpc::CreateChannel("localhost:2511", grpc::InsecureChannelCredentials())};
    std::thread complete_rpc_thread{&SampleClient::CompleteRPC, &client};
    for (int i = 0; i < 100; ++i) {
        std::string request_sample_field{"world " + std::to_string(i)};
        client.SampleMethod(request_sample_field);
    }
    std::cout << "Press Ctrl + C to quit..." << std::endl << std::endl;
    complete_rpc_thread.join();
}

}

int main(int argc, char** argv) {
    ns1::RunClient();
    ns2::RunClient();
    return 0;
}


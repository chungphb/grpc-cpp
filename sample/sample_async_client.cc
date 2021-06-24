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
        // Prepare request
        SampleRequest request;
        request.set_request_sample_field(request_sample_field);

        // Create an RPC object
        SampleResponse response;
        ClientContext context;
        CompletionQueue queue;
        Status status;
        std::unique_ptr<ClientAsyncResponseReader<SampleResponse>> rpc;
        rpc = _stub->PrepareAsyncSampleMethod(&context, request, &queue);
        
        // Initiate the RPC call
        rpc->StartCall();
        
        // Request to update the server's response and the call status upon completion of the RPC
        rpc->Finish(&response, &status, (void*)1);

        // Complete the RPC call
        void* tag;
        bool ok = false;
        if (queue.Next(&tag, &ok) && ok && tag == (void*)1) {
            if (status.ok()) {
                return response.response_sample_field();
            } else {
                std::cerr << status.error_code() << ": " << status.error_message() << std::endl;
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
    std::string server_address{"localhost:2511"};
    SampleClient client{grpc::CreateChannel(server_address, grpc::InsecureChannelCredentials())};
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
        // Prepare request
        SampleRequest request;
        request.set_request_sample_field(request_sample_field);

        // Create an AsyncClientCall object to store RPC data
        auto* call = new AsyncClientCall;
        
        // Create an RPC object
        call->rpc = _stub->PrepareAsyncSampleMethod(&call->context, request, &_queue);
        
        // Initiate the RPC call
        call->rpc->StartCall();
        
        // Request to update the server's response and the call status upon completion of the RPC
        call->rpc->Finish(&call->response, &call->status, (void*)call);
    }

    void AsyncCompleteRPC() {
        void* tag;
        bool ok = false;
        while (_queue.Next(&tag, &ok)) {
            if (!ok) {
                std::cerr << "Something went wrong" << std::endl;
                abort();
            }
            std::string err;
            auto* call = static_cast<AsyncClientCall*>(tag);
            if (call) {
                if (call->status.ok()) {
                    std::cout << "Client received: " << call->response.response_sample_field() << std::endl;
                } else {
                    std::cerr << call->status.error_code() << ": " << call->status.error_message() << std::endl;
                    std::cout << "Client received: RPC failed" << std::endl;
                }
            } else {
                err = "A client call was deleted";
            }
            delete call;
            if (!err.empty()) {
                throw std::runtime_error(err);
            }
        }
    }

private:
    struct AsyncClientCall {
        SampleResponse response;
        ClientContext context;
        Status status;
        std::unique_ptr<ClientAsyncResponseReader<SampleResponse>> rpc;
    };
    std::unique_ptr<SampleService::Stub> _stub;
    CompletionQueue _queue;
};

void RunClient() {
    std::string server_address{"localhost:2511"};
    SampleClient client{grpc::CreateChannel(server_address, grpc::InsecureChannelCredentials())};
    std::thread thread{&SampleClient::AsyncCompleteRPC, &client};
    for (int i = 0; i < 100; ++i) {
        std::string request_sample_field{"world " + std::to_string(i)};
        client.SampleMethod(request_sample_field);
    }
    std::cout << "Press Ctrl + C to quit..." << std::endl << std::endl;
    thread.join();
}

}

int main(int argc, char** argv) {
    ns1::RunClient();
    ns2::RunClient();
    return 0;
}


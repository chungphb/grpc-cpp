#include <sample.grpc.pb.h>
#include <grpc++/grpc++.h>
#include <memory>
#include <iostream>

using grpc::Server;
using grpc::ServerAsyncResponseWriter;
using grpc::ServerBuilder;
using grpc::ServerCompletionQueue;
using grpc::ServerContext;
using grpc::Status;
using sample::SampleRequest;
using sample::SampleResponse;
using sample::SampleService;

class ServerImpl final {
public:
    ~ServerImpl() {
        _server->Shutdown();
        _queue->Shutdown();
    }

    void Run() {
        std::string server_address{"localhost:2511"};

        ServerBuilder builder;
        builder.AddListeningPort(server_address, grpc::InsecureServerCredentials());
        builder.RegisterService(&_service);

        _queue = builder.AddCompletionQueue();
        _server = builder.BuildAndStart();
        std::cout << "Server listening on " << server_address << std::endl;
        HandleRPCs();
    }

private:
    class RequestHandler {
    public:
        RequestHandler(SampleService::AsyncService* service, ServerCompletionQueue* queue)
                : _service{service}, _queue{queue}, _responder{&_context}, _status{CallStatus::CREATE} {
            Proceed();
        }

        void Proceed() {
            switch (_status) {
                case CallStatus::CREATE: {
                    _status = CallStatus::PROCESS;
                    _service->RequestSampleMethod(&_context, &_request, &_responder, _queue, _queue, this);
                    break;
                }
                case CallStatus::PROCESS: {
                    new RequestHandler{_service, _queue};
                    _response.set_response_sample_field("Hello " + _request.request_sample_field());
                    _status = CallStatus::FINISH;
                    _responder.Finish(_response, Status::OK, this);
                    break;
                }
                default: {
                    delete this;
                }
            }
        }

    private:
        SampleService::AsyncService* _service;
        ServerCompletionQueue* _queue;
        ServerContext _context;
        SampleRequest _request;
        SampleResponse _response;
        ServerAsyncResponseWriter<SampleResponse> _responder;
        enum class CallStatus {
            CREATE, PROCESS, FINISH
        };
        CallStatus _status;
    };

    void HandleRPCs() {
        new RequestHandler{&_service, _queue.get()};
        void* tag;
        bool ok;
        while (true) {
            if (_queue->Next(&tag, &ok) && ok) {
                static_cast<RequestHandler*>(tag)->Proceed();
            } else {
                std::cerr << "Something went wrong" << std::endl;
                abort();
            }
        }
    }

    SampleService::AsyncService _service;
    std::unique_ptr<ServerCompletionQueue> _queue;
    std::unique_ptr<Server> _server;
};

int main(int argc, char** argv) {
    ServerImpl server;
    server.Run();
    return 0;
}
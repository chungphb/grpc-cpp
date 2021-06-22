#include <calculator.grpc.pb.h>
#include <grpc++/grpc++.h>
#include <memory>
#include <iostream>

using namespace grpc;
using namespace calculator;

enum class MessageID : uint8_t {
    ADD = 0, SUB = 1, MUL = 2, DIV = 3, MOD = 4
};

class CalculatorServer {
public:
    ~CalculatorServer() {
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
    struct Tag {
        void* call;
        MessageID id;
    };

    class CallData {
    public:
        CallData(Calculator::AsyncService* service, ServerCompletionQueue* queue)
                : _service{service}, _queue{queue}, _status{CallStatus::CREATE} {}

        virtual void Proceed() = 0;

    protected:
        Calculator::AsyncService* _service;
        ServerCompletionQueue* _queue;
        ServerContext _context;
        enum class CallStatus {
            CREATE, PROCESS, FINISH
        };
        CallStatus _status;
    };

    class AddCallData : public CallData {
    public:
        AddCallData(Calculator::AsyncService* service, ServerCompletionQueue* queue)
                : CallData{service, queue}, _responder{&_context} {
            _tag.id = MessageID::ADD;
            _tag.call = this;
            Proceed();
        }

        void Proceed() override {
            switch (_status) {
                case CallStatus::CREATE: {
                    _status = CallStatus::PROCESS;
                    _service->RequestAdd(&_context, &_request, &_responder, _queue, _queue, (void*)&_tag);
                    break;
                }
                case CallStatus::PROCESS: {
                    new AddCallData{_service, _queue};
                    _response.set_sum(_request.addend_1() + _request.addend_2());
                    _status = CallStatus::FINISH;
                    _responder.Finish(_response, Status::OK, (void*)&_tag);
                    break;
                }
                default: {
                    delete this;
                }
            }
        }

    private:
        AddRequest _request;
        AddResponse _response;
        ServerAsyncResponseWriter<AddResponse> _responder;
        Tag _tag;
    };

    class SubCallData : public CallData {
    public:
        SubCallData(Calculator::AsyncService* service, ServerCompletionQueue* queue)
                : CallData{service, queue}, _responder{&_context} {
            _tag.id = MessageID::SUB;
            _tag.call = this;
            Proceed();
        }

        void Proceed() {
            switch (_status) {
                case CallStatus::CREATE: {
                    _status = CallStatus::PROCESS;
                    _service->RequestSub(&_context, &_request, &_responder, _queue, _queue, (void*)&_tag);
                    break;
                }
                case CallStatus::PROCESS: {
                    new SubCallData{_service, _queue};
                    _response.set_difference(_request.minuend() - _request.subtrahend());
                    _status = CallStatus::FINISH;
                    _responder.Finish(_response, Status::OK, (void*)&_tag);
                    break;
                }
                default: {
                    delete this;
                }
            }
        }

    private:
        SubRequest _request;
        SubResponse _response;
        ServerAsyncResponseWriter<SubResponse> _responder;
        Tag _tag;
    };

    class MulCallData : public CallData {
    public:
        MulCallData(Calculator::AsyncService* service, ServerCompletionQueue* queue)
                : CallData{service, queue}, _responder{&_context} {
            _tag.id = MessageID::MUL;
            _tag.call = this;
            Proceed();
        }

        void Proceed() {
            switch (_status) {
                case CallStatus::CREATE: {
                    _status = CallStatus::PROCESS;
                    _service->RequestMul(&_context, &_request, &_responder, _queue, _queue, (void*)&_tag);
                    break;
                }
                case CallStatus::PROCESS: {
                    new MulCallData{_service, _queue};
                    _response.set_product(_request.multiplicand() * _request.multiplier());
                    _status = CallStatus::FINISH;
                    _responder.Finish(_response, Status::OK, (void*)&_tag);
                    break;
                }
                default: {
                    delete this;
                }
            }
        }

    private:
        MulRequest _request;
        MulResponse _response;
        ServerAsyncResponseWriter<MulResponse> _responder;
        Tag _tag;
    };

    class DivCallData : public CallData {
    public:
        DivCallData(Calculator::AsyncService* service, ServerCompletionQueue* queue)
                : CallData{service, queue}, _responder{&_context} {
            _tag.id = MessageID::DIV;
            _tag.call = this;
            Proceed();
        }

        void Proceed() {
            switch (_status) {
                case CallStatus::CREATE: {
                    _status = CallStatus::PROCESS;
                    _service->RequestDiv(&_context, &_request, &_responder, _queue, _queue, (void*)&_tag);
                    break;
                }
                case CallStatus::PROCESS: {
                    new DivCallData{_service, _queue};
                    _response.set_quotient(_request.dividend() / _request.divisor());
                    _status = CallStatus::FINISH;
                    _responder.Finish(_response, Status::OK, (void*)&_tag);
                    break;
                }
                default: {
                    delete this;
                }
            }
        }

    private:
        DivRequest _request;
        DivResponse _response;
        ServerAsyncResponseWriter<DivResponse> _responder;
        Tag _tag;
    };

    class ModCallData : public CallData {
    public:
        ModCallData(Calculator::AsyncService* service, ServerCompletionQueue* queue)
                : CallData{service, queue}, _responder{&_context} {
            _tag.id = MessageID::MOD;
            _tag.call = this;
            Proceed();
        }

        void Proceed() {
            switch (_status) {
                case CallStatus::CREATE: {
                    _status = CallStatus::PROCESS;
                    _service->RequestMod(&_context, &_request, &_responder, _queue, _queue, (void*)&_tag);
                    break;
                }
                case CallStatus::PROCESS: {
                    new ModCallData{_service, _queue};
                    _response.set_result(_request.dividend() % _request.divisor());
                    _status = CallStatus::FINISH;
                    _responder.Finish(_response, Status::OK, (void*)&_tag);
                    break;
                }
                default: {
                    delete this;
                }
            }
        }

    private:
        ModRequest _request;
        ModResponse _response;
        ServerAsyncResponseWriter<ModResponse> _responder;
        Tag _tag;
    };

    void HandleRPCs() {
        new AddCallData{&_service, _queue.get()};
        new SubCallData{&_service, _queue.get()};
        new MulCallData{&_service, _queue.get()};
        new DivCallData{&_service, _queue.get()};
        new ModCallData{&_service, _queue.get()};
        void* tag;
        bool ok;
        while (true) {
            if (_queue->Next(&tag, &ok) && ok) {
                auto* tag_ptr = static_cast<Tag*>(tag);
                switch (tag_ptr->id) {
                    case MessageID::ADD: {
                        static_cast<AddCallData*>(tag_ptr->call)->Proceed();
                        break;
                    }
                    case MessageID::SUB: {
                        static_cast<SubCallData*>(tag_ptr->call)->Proceed();
                        break;
                    }
                    case MessageID::MUL: {
                        static_cast<MulCallData*>(tag_ptr->call)->Proceed();
                        break;
                    }
                    case MessageID::DIV: {
                        static_cast<DivCallData*>(tag_ptr->call)->Proceed();
                        break;
                    }
                    case MessageID::MOD: {
                        static_cast<ModCallData*>(tag_ptr->call)->Proceed();
                        break;
                    }
                }
            } else {
                std::cerr << "Something went wrong" << std::endl;
                abort();
            }
        }
    }

    Calculator::AsyncService _service;
    std::unique_ptr<ServerCompletionQueue> _queue;
    std::unique_ptr<Server> _server;
};

int main(int argc, char** argv) {
    CalculatorServer server;
    server.Run();
    return 0;
}

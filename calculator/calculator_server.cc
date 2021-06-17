#include <calculator.grpc.pb.h>
#include <grpc++/grpc++.h>
#include <memory>
#include <iostream>

using namespace grpc;
using namespace calculator;

class CalculatorServiceImpl final : public Calculator::Service {
    Status Add(ServerContext* context, const AddRequest* request, AddResponse* response) {
        response->set_sum(request->addend_1() + request->addend_2());
        return Status::OK;
    }

    Status Sub(ServerContext* context, const SubRequest* request, SubResponse* response) {
        response->set_difference(request->minuend() - request->subtrahend());
        return Status::OK;
    }

    Status Mul(ServerContext* context, const MulRequest* request, MulResponse* response) {
        response->set_product(request->multiplicand() * request->multiplier());
        return Status::OK;
    }

    Status Div(ServerContext* context, const DivRequest* request, DivResponse* response) {
        response->set_quotient(request->dividend() / request->divisor());
        return Status::OK;
    }

    Status Mod(ServerContext* context, const ModRequest* request, ModResponse* response) {
        response->set_result(request->dividend() % request->divisor());
        return Status::OK;
    }
};

void RunServer() {
    std::string server_address{"localhost:2510"};
    CalculatorServiceImpl service;

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
#include <string>
#include <mutex>
#include <unordered_map>

#include <grpcpp/ext/proto_server_reflection_plugin.h>
#include <grpcpp/grpcpp.h>
#include <grpcpp/health_check_service_interface.h>

#include "kv/protos/kv.grpc.pb.h"
#include "kv/protos/kv.pb.h"

#include "wiredtiger.hpp"

using grpc::Server;
using grpc::ServerBuilder;
using grpc::ServerContext;
using grpc::Status;

using std::string;
using std::unordered_map;
using std::mutex;

unordered_map<string, string> data;
mutex mtx;

namespace kv {

class KVStoreService final : public KVStore::Service {
  Status Set(ServerContext *ctx, const SetRequest *req, SetResponse *res) {
    auto &key = req->key();
    auto &value = req->value();
    mtx.lock();
    data[key] = value;
    mtx.unlock();
    res->set_ok(true);
    return Status::OK;
  }

  Status Unset(ServerContext *ctx, const UnsetRequest *req, UnsetResponse *res) {
    auto& key = req->key();
    mtx.lock();
    data.erase(key);
    mtx.unlock();
    res->set_ok(true);
    return Status::OK;
  }

  Status Get(ServerContext *ctx, const GetRequest *req, GetResponse *res) {
    auto &key = req->key();
    mtx.lock();
    auto &value = data[key];
    mtx.unlock();
    res->set_value(value);
    return Status::OK;
  }

  Status Incr(ServerContext *ctx, const IncrRequest *req, IncrResponse *res) {
    auto &key = req->key();
    auto incr = req->incr();
    mtx.lock();
    auto &val = data[key];
    int int_val = stoi(val);
    int_val += incr;
    data[key] = std::to_string(int_val);
    mtx.unlock();
    res->set_new_val(int_val);
    res->set_ok(true);
    return Status::OK;
  }
};

} // namespace kv

int main(int argc, char *argv[]) {
  std::string server_address("0.0.0.0:50051");
  kv::KVStoreService service;

  grpc::EnableDefaultHealthCheckService(true);
  grpc::reflection::InitProtoReflectionServerBuilderPlugin();
  ServerBuilder builder;

  builder.AddListeningPort(server_address, grpc::InsecureServerCredentials());
  builder.RegisterService(&service);

  std::unique_ptr<Server> server(builder.BuildAndStart());
  std::cout << "Server listening on " << server_address << std::endl;

  server->Wait();

  return 0;
}
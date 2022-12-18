#include <string>
#include <mutex>

#include <grpcpp/ext/proto_server_reflection_plugin.h>
#include <grpcpp/grpcpp.h>
#include <grpcpp/health_check_service_interface.h>

#include "kv/protos/kv.grpc.pb.h"
#include "kv/protos/kv.pb.h"

#include "wiredtiger.hpp"

namespace kv {

using grpc::ServerContext;
using grpc::Status;

using std::string;
using std::mutex;

mutex mtx;

class KVStoreService final : public KVStore::Service {
public:
  KVStoreService(wiredtiger::Session& session, wiredtiger::Cursor& cursor) :
    _session(std::move(session)),
    _cursor(std::move(cursor))
  {}

private:
  Status Set(ServerContext *ctx, const SetRequest *req, SetResponse *res) {
    auto &key = req->key();
    auto &value = req->value();
    mtx.lock();
    _session.begin_transaction();
    _cursor.set(key, value);
    _session.commit_transaction();
    mtx.unlock();
    res->set_ok(true);
    return Status::OK;
  }

  Status Unset(ServerContext *ctx, const UnsetRequest *req, UnsetResponse *res) {
    auto& key = req->key();
    //data.erase(key);
    res->set_ok(true);
    return Status::OK;
  }

  Status Get(ServerContext *ctx, const GetRequest *req, GetResponse *res) {
    auto &key = req->key();
    mtx.lock();
    auto value = _cursor.get(key);
    mtx.unlock();
    res->set_value(value.c_str());
    return Status::OK;
  }

  Status Incr(ServerContext *ctx, const IncrRequest *req, IncrResponse *res) {
    auto &key = req->key();
    auto incr = req->incr();
    mtx.lock();
    _session.begin_transaction();
    auto val = _cursor.get(key);
    int int_val = stoi(val);
    int_val += incr;
    _cursor.set(key, std::to_string(int_val));
    _session.commit_transaction();
    mtx.unlock();
    res->set_new_val(int_val);
    res->set_ok(true);
    return Status::OK;
  }

  wiredtiger::Session _session;
  wiredtiger::Cursor _cursor;
};

} // namespace kv

int main(int argc, char *argv[]) {
  using grpc::Server;
  using grpc::ServerBuilder;

  const char* home = "./data";
  wiredtiger::Connection conn(home);
  auto session = conn.getSession();
  //session.drop("tyler");
  session.create("tyler");
  auto cursor = session.cursor("tyler");

  std::string server_address("0.0.0.0:50051");
  kv::KVStoreService service(session, cursor);

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
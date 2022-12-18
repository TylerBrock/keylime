#include <iostream>
#include <memory>
#include <string>

#include "kvclient.hpp"

using std::string;

namespace kv {

using grpc::ClientContext;
using grpc::Status;

bool KVStoreClient::set(const string &key, const string &value) {
  SetRequest req;
  req.set_key(key);
  req.set_value(value);

  SetResponse res;
  ClientContext ctx;
  auto status = stub_->Set(&ctx, req, &res);
  return status.ok();
}

string KVStoreClient::get(const string &key) {
  GetRequest req;
  req.set_key(key);

  GetResponse res;
  ClientContext ctx;
  Status status = stub_->Get(&ctx, req, &res);

  return res.value();
}

bool KVStoreClient::incr(const string &key, const int64_t amount) {
  IncrRequest req;
  req.set_key(key);
  req.set_incr(amount);

  IncrResponse res;
  ClientContext ctx;
  auto status = stub_->Incr(&ctx, req, &res);
  return status.ok();
}

} // namespace kv
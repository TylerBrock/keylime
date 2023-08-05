#pragma once

#include <memory>
#include <string>

#include <grpcpp/grpcpp.h>

#include "kv/protos/kv.grpc.pb.h"

namespace kv {

using std::shared_ptr;
using std::string;
using std::unique_ptr;

using grpc::Channel;

class KVStoreClient {
public:
  KVStoreClient(shared_ptr<Channel> channel)
      : stub_(KVStore::NewStub(channel)){};

  virtual bool set(const string &key, const string &value);
  virtual bool unset(const string &key);
  virtual string get(const string &key);
  virtual bool incr(const string &key, const int64_t amount);

protected:
  unique_ptr<KVStore::Stub> stub_;
};

} // namespace kv
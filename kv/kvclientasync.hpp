#pragma once
#include <memory>
#include <string>

#include <grpcpp/grpcpp.h>

#include "kv/kvclient.hpp"
#include "kv/protos/kv.grpc.pb.h"

using std::shared_ptr;
using std::string;
using std::unique_ptr;

using grpc::Channel;
using grpc::ClientAsyncResponseReader;
using grpc::CompletionQueue;

namespace kv {

using AsyncSetResponse = unique_ptr<ClientAsyncResponseReader<SetResponse>>;
using AsyncGetResponse = unique_ptr<ClientAsyncResponseReader<GetResponse>>;
using AsyncIncrResponse = unique_ptr<ClientAsyncResponseReader<IncrResponse>>;

class KVStoreAsyncClient final : public KVStoreClient {

public:
  KVStoreAsyncClient(shared_ptr<Channel> channel) : KVStoreClient(channel) {}

  virtual bool set(const string &key, const string &value) override;
  virtual string get(const string &key) override;
  virtual bool incr(const string &key, const int64_t amount) override;

private:
  CompletionQueue cq_;

  bool getResponse(void *tag) {
    void *got_tag;
    bool ok = false;
    cq_.Next(&got_tag, &ok);
    return ok;
  }
};

} // namespace kv
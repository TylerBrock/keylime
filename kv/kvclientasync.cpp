#include "kvclientasync.hpp"

namespace kv {

    using grpc::Status;
    using grpc::ClientContext;

    bool KVStoreAsyncClient::set(const string& key, const string& value) {
        SetRequest req;
        req.set_key(key);
        req.set_value(value);

        SetResponse res;
        ClientContext ctx;
        AsyncSetResponse rpc = stub_->AsyncSet(&ctx, req, &cq_);

        Status status;
        rpc->Finish(&res, &status, (void*)1);
        getResponse((void*)1);
        return status.ok();
    }

    string KVStoreAsyncClient::get(const string& key) {
        GetRequest req;
        req.set_key(key);

        GetResponse res;
        ClientContext ctx;
        AsyncGetResponse rpc = stub_->AsyncGet(&ctx, req, &cq_);

        Status status;
        rpc->Finish(&res, &status, (void*)1);
        getResponse((void*)1);
        return res.value();
    }

    bool KVStoreAsyncClient::incr(const string& key, const int64_t amount) {
        IncrRequest req;
        req.set_key(key);
        req.set_incr(amount);

        IncrResponse res;
        ClientContext ctx;
        AsyncIncrResponse rpc = stub_->AsyncIncr(&ctx, req, &cq_);

        Status status;
        rpc->Finish(&res, &status, (void*)1);
        getResponse((void*)1);
        return status.ok();
    }
}
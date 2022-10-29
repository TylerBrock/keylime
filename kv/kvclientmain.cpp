#include <string>

#include <grpcpp/grpcpp.h>

#include "kvclient.hpp"
#include "kvclientasync.hpp"

using std::string;

using grpc::CreateChannel;
using grpc::InsecureChannelCredentials;

using namespace kv;

int main(int argc, char **argv) {
    string target = "localhost:50051";

    auto channel = CreateChannel(target, InsecureChannelCredentials());
    auto client = KVStoreClient(channel);
    auto asyncClient = KVStoreAsyncClient(channel);

    KVStoreClient* clients[] {&client, &asyncClient};

    for (auto client : clients) {
        client->set("Tyler", "Brock");
        auto response = client->get("Tyler");
        std::cout << response << std::endl;

        client->set("lol", "1");
        client->incr("lol", 2);
        response = client->get("lol");

        std::cout << response << std::endl;
    }
}
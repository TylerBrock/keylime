#include <string>
#include <gtest/gtest.h>

#include "kvclient.hpp"
#include "kvclientasync.hpp"

using std::string;
using grpc::CreateChannel;
using grpc::InsecureChannelCredentials;

using namespace kv;

string target = "localhost:50051";
auto channel = CreateChannel(target, InsecureChannelCredentials());
auto client = KVStoreClient(channel);
auto asyncClient = KVStoreAsyncClient(channel);

KVStoreClient* clients[] {&client, &asyncClient};

TEST(KVTest, SetAndGet) {
  for (auto client : clients) {
      auto status = client->set("Tyler", "Brock");
      auto response = client->get("Tyler");

      EXPECT_EQ(status, true);
      EXPECT_STREQ(response.c_str(), "Brock");
  }
}

TEST(KVTest, Incr) {
  for (auto client : clients) {
      auto status = client->set("lol", "1");
      status = client->incr("lol", 2);
      auto response = client->get("lol");

      EXPECT_EQ(status, true);
      EXPECT_STREQ(response.c_str(), "3");
  }
}
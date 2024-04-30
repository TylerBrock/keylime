#include <string>
#include <thread>
#include <vector>

#include <gtest/gtest.h>

#include "kvclient.hpp"
#include "kvclientasync.hpp"

using std::string;
using grpc::CreateChannel;
using grpc::InsecureChannelCredentials;

using namespace kv;

class KVTestFixture : public testing::Test {
protected:
  void SetUp() override {
    string target = "localhost:50051";
    auto creds = InsecureChannelCredentials();
    auto channel = CreateChannel(target, creds);
    auto client = new KVStoreClient(channel);
    auto asyncClient = new KVStoreAsyncClient(channel);
    clients[0] = client;
    clients[1] = asyncClient;
  };
  void TearDown() override {
    delete clients[0];
    delete clients[1];
  }
  KVStoreClient* clients[2];
};

TEST_F(KVTestFixture, SetAndGet) {
  for (auto client : clients) {
    auto status = client->set("Tyler", "Brock");
    auto response = client->get("Tyler");

    EXPECT_EQ(status, true);
    EXPECT_STREQ(response.c_str(), "Brock");
  }
}

TEST_F(KVTestFixture, Incr) {
  for (auto client : clients) {
    auto status = client->set("lol", "1");
    status = client->incr("lol", 2);
    auto response = client->get("lol");

    EXPECT_EQ(status, true);
    EXPECT_STREQ(response.c_str(), "3");
  }
}

TEST_F(KVTestFixture, Unset) {
  for (auto client : clients) {
    auto status = client->set("lol", "1");
    auto response = client->get("lol");
    EXPECT_EQ(status, true);
    EXPECT_STREQ(response.c_str(), "1");

    status = client->unset("lol");
    response = client->get("lol");
    EXPECT_EQ(status, true);
    EXPECT_STREQ(response.c_str(), "");
  }
}

TEST_F(KVTestFixture, ConcurrentIncr) {
  std::mutex mtx;

  for (auto client : clients) {
    const std::string str("lol");
    client->set(str, "0");
    auto threads = std::vector<std::thread>();
    for (int i=0; i<1000; i++) {
      threads.emplace_back(std::thread([&client, &str, &mtx]{
        std::scoped_lock<std::mutex> lock(mtx);
        client->incr(str, 1);
      }));
    }
    for (auto& thread : threads) thread.join();
    auto result = client->get(str);
    EXPECT_EQ(result, "1000");
  }
}
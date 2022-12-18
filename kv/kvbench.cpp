#include <string>

#include <benchmark/benchmark.h>

#include "kvclient.hpp"

using namespace kv;

using grpc::CreateChannel;
using grpc::InsecureChannelCredentials;

static void BM_KVClientSet(benchmark::State &state) {
  // Perform setup here
  const string target = "localhost:50051";

  auto channel = CreateChannel(target, InsecureChannelCredentials());
  auto client = KVStoreClient(channel);

  for (auto _ : state) {
    // This code gets timed
    auto status = client.set("Tyler", "Brock");
  }
}

static void BM_KVClientGet(benchmark::State &state) {
  // Perform setup here
  const string target = "localhost:50051";

  auto channel = CreateChannel(target, InsecureChannelCredentials());
  auto client = KVStoreClient(channel);
  auto status = client.set("Tyler", "Brock");

  for (auto _ : state) {
    // This code gets timed
    auto response = client.get("Tyler");
  }
}

// Register the function as a benchmark
BENCHMARK(BM_KVClientSet);
BENCHMARK(BM_KVClientGet);

// Run the benchmark
BENCHMARK_MAIN();
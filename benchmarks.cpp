#include <benchmark/benchmark.h>
#include "KeyValueStore.h"
#include <string>

// Global instance of our store to use in all benchmarks
static KeyValueStore kvs;

// --- Benchmark for the SET operation ---
static void BM_Set(benchmark::State& state) {
  int i = 0;
  for (auto _ : state) {
    // This loop is managed by the benchmark library.
    // It will run as many times as needed to get a stable measurement.
    kvs.set("key" + std::to_string(i++), "some_value");
  }
}
// Register the function as a benchmark
BENCHMARK(BM_Set);

// --- Benchmark for the GET operation ---
// This fixture sets up the store with data before running the benchmark
class GetBenchmark : public benchmark::Fixture {
public:
  void SetUp(const ::benchmark::State& state) {
    if (kvs.count() == 0) {
        for (int i = 0; i < 10000; ++i) {
            kvs.set("key" + std::to_string(i), "some_value");
        }
    }
  }
};

BENCHMARK_F(GetBenchmark, BM_Get)(benchmark::State& state) {
  int i = 0;
  for (auto _ : state) {
    // The key might not exist, which is a valid test case
    kvs.get("key" + std::to_string(i++));
  }
}

// --- Benchmark for the INCR operation ---
static void BM_Incr(benchmark::State& state) {
    for (auto _ : state) {
        kvs.incr("atomic_counter");
    }
}
BENCHMARK(BM_Incr);


// Boilerplate main function to run the benchmarks
BENCHMARK_MAIN();
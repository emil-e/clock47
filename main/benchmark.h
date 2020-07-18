#pragma once

#include <cstdint>
#include <time.h>

namespace benchmark {

inline std::uint64_t microtime() {
  struct timeval tv_now;
  gettimeofday(&tv_now, NULL);
  return (std::uint64_t)tv_now.tv_sec * 1000000L + (std::uint64_t)tv_now.tv_usec;
}

#define MEASURE_BEGIN(id) const auto id##__begin = benchmark::microtime()
#define MEASURE_END(id)                                                                            \
  ESP_LOGI("benchmark", "Timed '" #id "': %lld", benchmark::microtime() - id##__begin)

} // namespace benchmark

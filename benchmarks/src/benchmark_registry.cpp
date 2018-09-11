#include "benchmark_registry.hpp"

std::unordered_map< std::string, benchmark_function >& get_benchmark_registry()
{
  static std::unordered_map< std::string, benchmark_function > registry;
  return registry;
}

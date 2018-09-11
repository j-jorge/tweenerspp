#ifndef TWEENERS_BENCHMARK_BENCHMARK_REGISTRY_H
#define TWEENERS_BENCHMARK_BENCHMARK_REGISTRY_H

#include <cassert>
#include <string>
#include <unordered_map>

class options;

typedef void (*benchmark_function)( const options& );
std::unordered_map< std::string, benchmark_function >& get_benchmark_registry();

#define register_benchmark_concat(a, b) register_benchmark_concat_1(a, b)
#define register_benchmark_concat_1(a, b) register_benchmark_concat_2(~, a ## b)
#define register_benchmark_concat_2(p, res) res

#define register_benchmark( name, function )                            \
  static bool register_benchmark_concat( benchmark_, __COUNTER__ ) =    \
    []() -> bool                                                        \
  {                                                                     \
    std::unordered_map< std::string, benchmark_function >&              \
    benchmark_registry                                                  \
    ( get_benchmark_registry() );                                       \
    assert                                                              \
    ( benchmark_registry.find( name ) == benchmark_registry.end() );    \
    benchmark_registry[ name ] = function;                              \
    return true;                                                        \
  }()

#endif

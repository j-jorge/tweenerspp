#ifndef TWEENERS_BENCHMARK_OPTIONS_H
#define TWEENERS_BENCHMARK_OPTIONS_H

#include <optional>
#include <string>
#include <vector>

class options
{
public:
  int initial_slot_count;
  std::vector< float > durations;
  float update_step;
  std::vector< std::string > benchmarks;
  bool quit_now;
};

std::optional< options > parse_arguments( int argc, char** argv );
void print_options( const options& options );

#endif

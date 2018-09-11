#include "options.hpp"
#include "benchmark_registry.hpp"

int main( int argc, char** argv )
{
  std::optional< options > options( parse_arguments( argc, argv ) );

  if ( !options )
    return 1;

  if ( options->quit_now )
    return 0;

  print_options( *options );

  int result( 0 );
  
  const std::unordered_map< std::string, benchmark_function >&
    benchmark_registry
    ( get_benchmark_registry() );
  
  for ( const std::string& benchmark_name : options->benchmarks )
    {
      const auto benchmark_runner( benchmark_registry.find( benchmark_name ) );

      if ( benchmark_runner == benchmark_registry.end() )
        {
          printf
            ( "# -- Benchmark '%s' not found. --\n", benchmark_name.c_str() );
          result = 1;
        }
      else
        {
          printf( "# -- Running benchmark '%s' --\n", benchmark_name.c_str() );
          benchmark_runner->second( *options );
          printf( "# -- Done benchmark '%s' --\n", benchmark_name.c_str() );
        }
    }
  
  return result;
}

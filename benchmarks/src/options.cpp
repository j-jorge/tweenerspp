#include "options.hpp"
#include "benchmark_registry.hpp"

#include <cassert>
#include <cstdio>
#include <cstdlib>
#include <cstring>

static void print_help( std::FILE* stream, char* arg_zero )
{
  std::fputs( "Usage: ", stream );
  std::fputs( arg_zero, stream );
  std::fputs
    ( " [OPTION]...\n"
      "Run a benchmark on the update of the tweeners.\n\n"
      "The tweeners are created by instantiating a given count, then queuing\n"
      "a second tweener to half of them, then a third to half of the seconds,\n"
      "and so on until there is no more tweener to create.\n\n"
      
      "OPTION can be:\n"
      "  --durations NUM...    The durations of the tweeners. These durations\n"
      "                        are applied to the tweeners as they are\n"
      "                        created, looping back to the first entry if\n"
      "                        needed. Default value is {5, 20, 10, 0, 30}.\n"
      "  --help                Display this message and exit.\n"
      "  --initial-count NUM   The number of tweeners to create for the first\n"
      "                        iteration. Default value is 100000.\n"
      "  --update NUM          The time step passed to the tweener system\n"
      "                        update. Default value is 3.\n"
      "  --benchmarks NAME...  Run the given benchmarks. The default is to\n"
      "                        run all benchmarks. Available NAMES are:\n",
      stream );

  const std::unordered_map< std::string, benchmark_function >&
    benchmark_registry
    ( get_benchmark_registry() );

  for( const auto& entry : benchmark_registry )
    {
      std::fputs( "                         - ", stream );
      std::fputs( entry.first.c_str(), stream );
      std::fputs( "\n", stream );
    }
}

std::optional< options > parse_arguments( int argc, char** argv )
{
  options result;
  int fail_count( 0 );
  
  result.initial_slot_count = 100000;
  result.update_step = 3;
  result.quit_now = false;

  for ( int i( 1 ); ( i != argc ); )
    if ( std::strcmp( argv[ i ], "--help" ) == 0 )
      {
        print_help( stdout, argv[ 0 ] );
        result.quit_now = true;
        ++i;
      }
    else if ( std::strcmp( argv[ i ], "--initial-count" ) == 0 )
      {
        result.initial_slot_count = std::atoi( argv[ i + 1 ] );
        i += 2;
      }
    else if ( std::strcmp( argv[ i ], "--update" ) == 0 )
      {
        result.update_step = std::atof( argv[ i + 1 ] );
        i += 2;
      }
    else if ( std::strcmp( argv[ i ], "--durations" ) == 0 )
      for ( ++i;
            ( i != argc ) && ( std::strncmp( argv[ i ], "--", 2 ) != 0 );
            ++i )
        result.durations.emplace_back( std::atof( argv[ i ] ) );
    else if ( std::strcmp( argv[ i ], "--benchmarks" ) == 0 )
      for ( ++i;
            ( i != argc ) && ( std::strncmp( argv[ i ], "--", 2 ) != 0 );
            ++i )
        result.benchmarks.emplace_back( argv[ i ] );
    else
      {
        if ( fail_count == 0 )
          {
            print_help( stderr, argv[ 0 ] );
            std::fputs( "\n", stderr );
          }

        ++fail_count;
        std::fputs( "Unrecognized argument: '", stderr );
        std::fputs( argv[ i ], stderr );
        std::fputs( "'.\n", stderr );

        ++i;
      }

  if ( fail_count != 0 )
    return std::nullopt;
  
  if ( result.durations.empty() )
    result.durations = { 5, 20, 10, 0, 30 };

  if ( result.benchmarks.empty() )
    for ( const auto& entry : get_benchmark_registry() )
      result.benchmarks.emplace_back( entry.first );
  
  return std::move( result );
}

void print_options( const options& options )
{
  assert( !options.durations.empty() );
  assert( !options.benchmarks.empty() );
  
  printf
    ( "# initial count is %d.\n"
      "# durations are {%g",
      options.initial_slot_count,
      options.durations[ 0 ] );

  const std::size_t duration_count( options.durations.size() );

  for ( std::size_t i( 1 ); i != duration_count; ++i )
    printf( ", %g", options.durations[ i ] );

  printf
    ( "}\n"
      "# update step is %g.\n"
      "# running benchmarks: '%s'",
      options.update_step,
      options.benchmarks[ 0 ].c_str() );

  const std::size_t benchmark_count( options.benchmarks.size() );

  for ( std::size_t i( 1 ); i != benchmark_count; ++i )
    printf( ", '%s'", options.benchmarks[ i ].c_str() );

  printf( "\n" );
}

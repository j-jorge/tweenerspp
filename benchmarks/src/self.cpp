#include "benchmark_registry.hpp"
#include "options.hpp"

#include "tweeners/builder.hpp"
#include "tweeners/easing.hpp"
#include "tweeners/system.hpp"

#include <chrono>

void default_self_benchmark( const options& options )
{
  std::size_t slot_count( options.initial_slot_count );

  int running( 0 );
  int total( 0 );
  int update_count( 0 );
  
  auto on_update( [ &update_count ]( float ) -> void { ++update_count; } );
  auto on_start
    ( [ &running, &total ]() -> void
      {
        ++running;
        ++total;
      } );
  auto on_done( [ &running ]() -> void { --running; } );
  
  tweeners::system system;
  system.reserve( slot_count * 2, slot_count * 2, slot_count );
  
  const std::size_t duration_count( options.durations.size() );
  
  std::vector< tweeners::system::id_type > slots( slot_count );

  for ( std::size_t i( 0 ); i != slot_count; ++i )
    slots[ i ] =
      tweeners::builder()
      .range_transform
      ( 0, 100, options.durations[ i % duration_count ], on_update,
        &tweeners::easing::linear< float > )
      .on_start( on_start )
      .on_done( on_done )
      .build( system );

  std::vector< tweeners::system::id_type > previous_slots;

  for ( slot_count /= 2 ; slot_count != 0; slot_count /= 2 )
    {
      previous_slots.swap( slots );
      slots.resize( slot_count );
      
      for ( std::size_t i( 0 ); i != slot_count; ++i )
        slots[ i ] =
          tweeners::builder()
          .range_transform
          ( 0, 100, options.durations[ i % duration_count ], on_update,
            &tweeners::easing::linear< float > )
          .on_start( on_start )
          .on_done( on_done )
          .after( previous_slots[ ( i / 2 ) * 4 ] )
          .build( system );
    }

  const std::chrono::nanoseconds start
    ( std::chrono::steady_clock::now().time_since_epoch() );
  
  do
    {
      system.update( options.update_step );
    }
  while( running != 0 );

  const std::chrono::nanoseconds end
    ( std::chrono::steady_clock::now().time_since_epoch() );

  printf( "# %d updates\n", update_count );
  printf
    ( "%llu # self-default\n",
      static_cast< unsigned long long >( ( end - start ).count() ) );
}

register_benchmark( "self-default", &default_self_benchmark );

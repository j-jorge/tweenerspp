#include "benchmark_registry.hpp"
#include "options.hpp"

#include <claw/tween/single_tweener.hpp>
#include <claw/tween/easing/easing_linear.hpp>
#include <claw/tween/tweener_group.hpp>
#include <claw/tween/tweener_sequence.hpp>

#include <chrono>

void claw_benchmark( const options& options )
{
  std::size_t slot_count( options.initial_slot_count );

  int update_count( 0 );
  auto on_update( [ &update_count ]( double ) -> void { ++update_count; } );
  
  const std::size_t duration_count( options.durations.size() );
  
  std::vector< claw::tween::tweener_sequence > sequences( slot_count );
  std::vector< std::size_t > indices( slot_count );

  for ( std::size_t i( 0 ); i != slot_count; ++i )
    {
      indices[ i ] = i;
      claw::tween::single_tweener tweener
        ( 0, 100, options.durations[ i % duration_count ], on_update,
          claw::tween::easing_linear::ease_in );
      sequences[ i ].insert( tweener );
    }

  std::vector< std::size_t > previous_indices;

  for ( slot_count /= 2 ; slot_count != 0; slot_count /= 2 )
    {
      previous_indices.swap( indices );
      indices.resize( slot_count );
      
      for ( std::size_t i( 0 ); i != slot_count; ++i )
        {
          claw::tween::single_tweener tweener
            ( 0, 100, options.durations[ i % duration_count ], on_update,
              claw::tween::easing_linear::ease_in );
          
          const std::size_t index( previous_indices[ ( i / 2 ) * 4 ] );
          indices[ i ] = index;
          sequences[ index ].insert( tweener );
        }
    }

  claw::tween::tweener_group group;

  for ( const claw::tween::tweener_sequence& sequence : sequences )
    group.insert( sequence );
  
  const std::chrono::nanoseconds start
    ( std::chrono::steady_clock::now().time_since_epoch() );

  while( !group.is_finished() )
    {
      group.update( options.update_step );
    }

  const std::chrono::nanoseconds end
    ( std::chrono::steady_clock::now().time_since_epoch() );

  printf( "# %d updates\n", update_count );
  printf
    ( "%llu # claw\n",
      static_cast< unsigned long long >( ( end - start ).count() ) );
}

register_benchmark( "claw", &claw_benchmark );

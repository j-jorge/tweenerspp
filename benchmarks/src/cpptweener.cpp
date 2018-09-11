/**
 * \file
 *
 * \brief Benchmark of the original Cpptweener library.
 *
 * This benchmark is flawed due to some bugs in the library. It is present for
 * completeness.
 *
 * Cpptweener handles the completion in a singular way: when a TweenerParam is
 * over, the library removes the first TweenerParam having the same duration,
 * easing function and whose properties are similar to the ones of the completed
 * TweenerParam in the sense where they have the same range. Neither the delay
 * of the TweenerParam nor the output variable of the properties
 * (i.e. TweenerProperty::ptrValue) are compared.
 *
 * Consequently some TweenerParam do not start or never reach their completion
 * point.
 */
#include "benchmark_registry.hpp"
#include "options.hpp"

#include <CppTweener.h>

#include <cassert>
#include <chrono>

struct cpptweener_tracker
{
  int update_count;
  int running_count;
};
  
static void cpptweener_update( void* ptr )
{
  ++static_cast< cpptweener_tracker* >( ptr )->update_count;
}

static void cpptweener_complete( void* ptr )
{
  assert( static_cast< cpptweener_tracker* >( ptr )->running_count > 0 );
  --static_cast< cpptweener_tracker* >( ptr )->running_count;
}

void cpptweener_benchmark( const options& options )
{
  std::size_t slot_count( options.initial_slot_count );
  cpptweener_tracker tracker{ 0, static_cast< int >( slot_count ) };
  tween::Tweener tweener;
  
  const std::size_t duration_count( options.durations.size() );
  
  std::vector< float > end_dates( slot_count );
  std::vector< float > values( slot_count * 2, 0 );
  std::size_t value_index( 0 );
  
  for ( std::size_t i( 0 ); i != slot_count; ++i )
    {
      const float duration( options.durations[ i % duration_count ] );
      end_dates[ i ] = duration_count;

      tween::TweenerParam param( duration, tween::LINEAR, tween::EASE_IN, 0 );
      param.onStepCallBack = &cpptweener_update;
      param.onCompleteCallBack = &cpptweener_complete;
      param.ptr = &tracker;

      param.addProperty( &values[ value_index ], 100 );
      ++value_index;
      
      tweener.addTween( param );
    }

  std::vector< float > previous_end_dates;

  for ( slot_count /= 2 ; slot_count != 0; slot_count /= 2 )
    {
      tracker.running_count += slot_count;
      
      previous_end_dates.swap( end_dates );
      end_dates.resize( slot_count );
      
      for ( std::size_t i( 0 ); i != slot_count; ++i )
        {
          const float delay( previous_end_dates[ ( i / 2 ) * 4 ] );
          const float duration( options.durations[ i % duration_count ] );
          end_dates[ i ] = delay + duration;

          tween::TweenerParam param
            ( duration, tween::LINEAR, tween::EASE_IN, delay );
          param.onStepCallBack = &cpptweener_update;
          param.onCompleteCallBack = &cpptweener_complete;
          param.ptr = &tracker;

          param.addProperty( &values[ value_index ], 100 );
          ++value_index;

          tweener.addTween( param );
        }
    }

  const std::chrono::nanoseconds start
    ( std::chrono::steady_clock::now().time_since_epoch() );

  float current_date( 0 );
  
  while( !tweener.tweens.empty() )
    {
      tweener.step( current_date );
      current_date += options.update_step;
    }

  const std::chrono::nanoseconds end
    ( std::chrono::steady_clock::now().time_since_epoch() );

  printf( "# %d updates\n", tracker.update_count );
  printf( "# %d running\n", tracker.running_count );
  printf
    ( "%llu # cpptweener\n",
      static_cast< unsigned long long >( ( end - start ).count() ) );
}

register_benchmark( "cpptweener", &cpptweener_benchmark );

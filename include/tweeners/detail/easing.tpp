#ifndef TWEENER_EASING_TPP
#define TWEENER_EASING_TPP

#include <tweeners/detail/debug.hpp>

#include <cmath>

#define tweeners_debug_check_easing_bounds( t )        \
  do                                            \
    {                                           \
      tweeners_debug_assert( Float( 0 ) <= t );  \
      tweeners_debug_assert( t <= Float( 1 ) );  \
    }                                           \
  while( false )

template< typename Float, typename Easing >
Float tweeners::easing::ease_out( Float t, Easing&& function )
{
  tweeners_debug_check_easing_bounds( t );

  // Should I wrap literals in Float() in case where Float is not implicitly
  // constructible from int?
  return 1 - function( 1 - t );
}

template< typename Float, typename Easing >
Float tweeners::easing::ease_in_out( Float t, Easing&& function )
{
  tweeners_debug_check_easing_bounds( t );

  if ( t <= Float( 0.5 ) )
    return function( 2 * t ) / 2;
      
  return Float( 0.5 ) + ease_out( 2 * t - 1, function ) / 2;
}
    
template< typename Float = float >
Float tweeners::easing::none( Float t )
{
  tweeners_debug_check_easing_bounds( t );
  return 0;
}
    
template< typename Float = float >
Float tweeners::easing::linear( Float t )
{
  tweeners_debug_check_easing_bounds( t );
  return t;
}
    
template< typename Float = float >
Float tweeners::easing::sine( Float t )
{
  tweeners_debug_check_easing_bounds( t );
  return 1 - std::cos( t * Float( M_PI ) / 2 );
}

template< typename Float = float >
Float tweeners::easing::quad( Float t )
{
  tweeners_debug_check_easing_bounds( t );
  return t * t;
}

template< typename Float = float >
Float tweeners::easing::cubic( Float t )
{
  tweeners_debug_check_easing_bounds( t );
  return t * t * t;
}
    
template< typename Float = float >
Float tweeners::easing::quart( Float t )
{
  tweeners_debug_check_easing_bounds( t );
  return t * t * t * t;
}

template< typename Float = float >
Float tweeners::easing::quint( Float t )
{
  tweeners_debug_check_easing_bounds( t );
  return t * t * t * t * t;
}

template< typename Float = float >
Float tweeners::easing::circ( Float t )
{
  tweeners_debug_check_easing_bounds( t );
  return 1 - std::sqrt( 1 - t * t );
}

template< typename Float = float >
Float tweeners::easing::expo( Float t )
{
  tweeners_debug_check_easing_bounds( t );
  return t == 0 ? 0 : std::pow( 2, 10 * ( t - 1 ) );
}

template< typename Float = float >
Float tweeners::easing::elastic( Float t )
{
  tweeners_debug_check_easing_bounds( t );

  const Float pi( M_PI );
  const Float v( t - 1 );
  const Float p( 0.3 );

  return -std::pow( 2, 10 * v ) * std::sin( ( v - p / 4 ) * 2 * pi / p );
}

template< typename Float = float >
Float tweeners::easing::bounce( Float t )
{
  tweeners_debug_check_easing_bounds( t );

  const Float v( 1 - t );
  Float c;
  Float d;

  if ( v < Float( 1 / 2.75 ) )
    {
      c = v;
      d = 0;
    }
  else if ( v < Float( 2 / 2.75 ) )
    {
      c = v - Float( 1.5 / 2.75 );
      d = Float( 0.75 );
    }
  else if ( v < Float( 2.5 / 2.75 ) )
    {
      c = v - Float( 2.25 / 2.75 );
      d = Float( 0.9375 );
    }
  else
    {
      c = v - Float( 2.625 / 2.75 );
      d = Float( 0.984375 );
    }
      
  return 1 - ( Float( 7.5625 ) * c * c + d );
}

template< typename Float = float >
Float tweeners::easing::back( Float t )
{
  tweeners_debug_check_easing_bounds( t );

  const Float s( 1.70158 );
  return t * t * ( ( s + 1 ) * t - s );
}

#undef tweeners_debug_check_easing_bounds

#endif

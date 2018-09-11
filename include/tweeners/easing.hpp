#ifndef TWEENERS_EASING_HPP
#define TWEENERS_EASING_HPP

namespace tweeners
{
  namespace easing
  {
    template< typename Float, typename Easing >
    Float ease_out( Float t, Easing&& function );

    template< typename Float, typename Easing >
    Float ease_in_out( Float t, Easing&& function );
    
    template< typename Float = float >
    Float none( Float t );
    
    template< typename Float = float >
    Float linear( Float t );
    
    template< typename Float = float >
    Float sine( Float t );
    
    template< typename Float = float >
    Float quad( Float t );
    
    template< typename Float = float >
    Float cubic( Float t );
    
    template< typename Float = float >
    Float quart( Float t );
    
    template< typename Float = float >
    Float quint( Float t );
    
    template< typename Float = float >
    Float circ( Float t );
    
    template< typename Float = float >
    Float expo( Float t );
    
    template< typename Float = float >
    Float elastic( Float t );

    template< typename Float = float >
    Float bounce( Float t );

    template< typename Float = float >
    Float back( Float t );
  }
}

#include "tweeners/detail/easing.tpp"

#endif

#include "tweeners/builder.hpp"
#include "tweeners/easing.hpp"
#include "tweeners/system.hpp"

#include <complex>

#include <gtest/gtest.h>

TEST( system, assign_complex_value )
{
  typedef std::complex< float > complex;
  
  complex value;

  tweeners::system<> system;
  tweeners::builder<>()
    .range_transform
    ( complex( 0, 100 ), complex( 100, 0 ), 10, value,
      &tweeners::easing::linear< float > )
    .build( system );

  system.update( 1 );
  EXPECT_FLOAT_EQ( 10, value.real() );
  EXPECT_FLOAT_EQ( 90, value.imag() );

  system.update( 3 );
  EXPECT_FLOAT_EQ( 40, value.real() );
  EXPECT_FLOAT_EQ( 60, value.imag() );
}

#include "tweeners/builder.hpp"
#include "tweeners/easing.hpp"
#include "tweeners/system.hpp"

#include <gtest/gtest.h>

TEST( system, zero_duration_single )
{
  int value;

  tweeners::system<> system;
  tweeners::builder<>()
    .range_transform( 0, 100, 0, value, &tweeners::easing::linear< float > )
    .build( system );

  system.update( 1 );
  EXPECT_EQ( 100, value );
}

TEST( system, zero_duration_in_sequence )
{
  int value_1, value_2, value_3;
  tweeners::system<> system;
  
  const int slot_1
    ( tweeners::builder<>()
      .range_transform( 0, 100, 1, value_1, &tweeners::easing::linear< float > )
      .build( system ) );

  const int slot_2
    ( tweeners::builder<>()
      .range_transform( 0, 10, 0, value_2, &tweeners::easing::linear< float > )
      .after( slot_1 )
      .build( system ) );

  tweeners::builder<>()
    .range_transform( 2, 4, 1, value_3, &tweeners::easing::linear< float > )
    .after( slot_2 )
    .build( system );

  system.update( 1 );
  EXPECT_EQ( 100, value_1 );
  EXPECT_EQ( 10, value_2 );
  EXPECT_EQ( 2, value_3 );

  system.update( 1 );
  EXPECT_EQ( 4, value_3 );
}

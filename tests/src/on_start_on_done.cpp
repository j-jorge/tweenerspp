#include "tweeners/builder.hpp"
#include "tweeners/easing.hpp"
#include "tweeners/system.hpp"

#include <gtest/gtest.h>

TEST( system, on_start_on_done )
{
  int value;
  int update_count( 0 );
  int start_count( 0 );
  int done_count( 0 );

  const auto on_start
    ( [ &update_count, &start_count ]() -> void
      {
        EXPECT_EQ( 0, update_count );
        ++start_count;
      } );
  
  const auto on_done
    ( [
        &value,
        &start_count,
        &update_count,
        &done_count
      ]() -> void
      {
        EXPECT_EQ( 1, start_count );
        // system.update() will be called 6 times by the end of the test.
        EXPECT_EQ( 6, update_count );
        EXPECT_EQ( 0, done_count );
        ++done_count;
      } );

  const auto update
    ( [ &value, &start_count, &update_count, &done_count ]( int v ) -> void
      {
        EXPECT_EQ( 1, start_count );
        EXPECT_EQ( 0, done_count );
        ++update_count;
        value = v;
      } );

  tweeners::system<> system;
  tweeners::builder<>()
    .range_transform
    ( 0, 100, 10, update, &tweeners::easing::linear< float > )
    .on_start( on_start )
    .on_done( on_done )
    .build( system );

  EXPECT_EQ( 0, start_count );
  EXPECT_EQ( 0, update_count );
  EXPECT_EQ( 0, done_count );
  
  system.update( 1 );
  EXPECT_EQ( 1, start_count );
  EXPECT_EQ( 1, update_count );
  EXPECT_EQ( 0, done_count );
  EXPECT_EQ( 10, value );

  system.update( 1 );
  EXPECT_EQ( 1, start_count );
  EXPECT_EQ( 2, update_count );
  EXPECT_EQ( 0, done_count );
  EXPECT_EQ( 20, value );

  system.update( 1 );
  EXPECT_EQ( 1, start_count );
  EXPECT_EQ( 3, update_count );
  EXPECT_EQ( 0, done_count );
  EXPECT_EQ( 30, value );

  system.update( 2 );
  EXPECT_EQ( 1, start_count );
  EXPECT_EQ( 4, update_count );
  EXPECT_EQ( 0, done_count );
  EXPECT_EQ( 50, value );

  system.update( 3 );
  EXPECT_EQ( 1, start_count );
  EXPECT_EQ( 5, update_count );
  EXPECT_EQ( 0, done_count );
  EXPECT_EQ( 80, value );

  system.update( 3 );
  EXPECT_EQ( 1, start_count );
  EXPECT_EQ( 6, update_count );
  EXPECT_EQ( 1, done_count );
  EXPECT_EQ( 100, value );
}

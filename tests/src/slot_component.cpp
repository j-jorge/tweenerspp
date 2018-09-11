#include "tweeners/detail/slot_component.hpp"

#include <gtest/gtest.h>

TEST( slot_component, insert )
{
  constexpr int guard( 42 );
  tweeners::detail::slot_component< int, int > values( guard );

  values.add_one_slot_at_end();
  EXPECT_EQ( guard, values[ 0 ] );
  
  values.add_one_slot_at_end();
  EXPECT_EQ( guard, values[ 1 ] );

  values.emplace( 1, 11 );
  EXPECT_EQ( guard, values[ 0 ] );
  EXPECT_EQ( 11, values[ 1 ] );

  values.emplace( 0, 22 );
  EXPECT_EQ( 22, values[ 0 ] );
  EXPECT_EQ( 11, values[ 1 ] );
}

TEST( slot_component, erase )
{
  constexpr int guard( 42 );
  tweeners::detail::slot_component< int, int > values( guard );

  for ( std::size_t i( 0 ); i != 20; ++i )
    values.add_one_slot_at_end();

  for ( std::size_t i( 0 ); i != 10; ++i )
    values.emplace( i * 2, i );

  EXPECT_EQ( 0, values[ 0 ] );
  EXPECT_EQ( guard, values[ 1 ] );
  EXPECT_EQ( 1, values[ 2 ] );
  EXPECT_EQ( guard, values[ 3 ] );
  EXPECT_EQ( 2, values[ 4 ] );
  EXPECT_EQ( guard, values[ 5 ] );
  EXPECT_EQ( 3, values[ 6 ] );
  EXPECT_EQ( guard, values[ 7 ] );
  EXPECT_EQ( 4, values[ 8 ] );
  EXPECT_EQ( guard, values[ 9 ] );
  EXPECT_EQ( 5, values[ 10 ] );
  EXPECT_EQ( guard, values[ 11 ] );
  EXPECT_EQ( 6, values[ 12 ] );
  EXPECT_EQ( guard, values[ 13 ] );
  EXPECT_EQ( 7, values[ 14 ] );
  EXPECT_EQ( guard, values[ 15 ] );
  EXPECT_EQ( 8, values[ 16 ] );
  EXPECT_EQ( guard, values[ 17 ] );
  EXPECT_EQ( 9, values[ 18 ] );
  EXPECT_EQ( guard, values[ 19 ] );

  const int to_remove[] = { 10, 2, 14, 18, 6 };
  values.erase( std::begin( to_remove ), std::end( to_remove ) );
  
  EXPECT_EQ( 0, values[ 0 ] );
  EXPECT_EQ( guard, values[ 1 ] );
  EXPECT_EQ( guard, values[ 2 ] );
  EXPECT_EQ( guard, values[ 3 ] );
  EXPECT_EQ( 2, values[ 4 ] );
  EXPECT_EQ( guard, values[ 5 ] );
  EXPECT_EQ( guard, values[ 6 ] );
  EXPECT_EQ( guard, values[ 7 ] );
  EXPECT_EQ( 4, values[ 8 ] );
  EXPECT_EQ( guard, values[ 9 ] );
  EXPECT_EQ( guard, values[ 10 ] );
  EXPECT_EQ( guard, values[ 11 ] );
  EXPECT_EQ( 6, values[ 12 ] );
  EXPECT_EQ( guard, values[ 13 ] );
  EXPECT_EQ( guard, values[ 14 ] );
  EXPECT_EQ( guard, values[ 15 ] );
  EXPECT_EQ( 8, values[ 16 ] );
  EXPECT_EQ( guard, values[ 17 ] );
  EXPECT_EQ( guard, values[ 18 ] );
  EXPECT_EQ( guard, values[ 19 ] );
}

TEST( slot_component, has_value )
{
  constexpr int guard( 42 );
  tweeners::detail::slot_component< int, int > values( guard );

  values.add_one_slot_at_end();
  values.add_one_slot_at_end();

  EXPECT_FALSE( values.has_value( 0 ) );
  EXPECT_FALSE( values.has_value( 1 ) );

  values.emplace( 1, 11 );

  EXPECT_FALSE( values.has_value( 0 ) );
  EXPECT_TRUE( values.has_value( 1 ) );
}

TEST( slot_component, get_existing )
{
  constexpr int guard( 42 );
  tweeners::detail::slot_component< int, int > values( guard );

  values.add_one_slot_at_end();
  values.add_one_slot_at_end();

  EXPECT_EQ( guard, values[ 1 ] );

  values.emplace( 1, 11 );

  EXPECT_EQ( 11, values.get_existing( 1 ) );
}

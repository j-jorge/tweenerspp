#include "tweeners/system.hpp"

#include "test_helper.hpp"

#include <gtest/gtest.h>

TEST( tweeners, start_twice )
{
  test_helper helper;
  
  tweener_tracker& tracker( helper.insert( tweeners::system::not_an_id ) );

  helper.system.update( 10 );

  EXPECT_EQ( 1, tracker.start_count );
  EXPECT_EQ( 1, tracker.update_count );
  EXPECT_EQ( 1, tracker.done_count );
  EXPECT_EQ( 100, tracker.value );

  tracker.value = 42;

  helper.system.start_slot( tracker.slot );
  
  helper.system.update( 2 );

  EXPECT_EQ( 2, tracker.start_count );
  EXPECT_EQ( 2, tracker.update_count );
  EXPECT_EQ( 1, tracker.done_count );
  EXPECT_EQ( 20, tracker.value );
  
  helper.system.update( 8 );

  EXPECT_EQ( 2, tracker.start_count );
  EXPECT_EQ( 3, tracker.update_count );
  EXPECT_EQ( 2, tracker.done_count );
  EXPECT_EQ( 100, tracker.value );
}

TEST( tweeners, start_twice_sequence )
{
  test_helper helper;
  
  tweener_tracker& tracker_1( helper.insert( tweeners::system::not_an_id ) );
  tweener_tracker& tracker_2( helper.insert( tracker_1.slot ) );
  tweener_tracker& tracker_3( helper.insert( tracker_1.slot ) );

  helper.system.update( 20 );

  EXPECT_EQ( 1, tracker_1.start_count );
  EXPECT_EQ( 1, tracker_1.update_count );
  EXPECT_EQ( 1, tracker_1.done_count );
  EXPECT_EQ( 100, tracker_1.value );

  EXPECT_EQ( 1, tracker_2.start_count );
  EXPECT_EQ( 1, tracker_2.update_count );
  EXPECT_EQ( 1, tracker_2.done_count );
  EXPECT_EQ( 100, tracker_2.value );

  EXPECT_EQ( 1, tracker_3.start_count );
  EXPECT_EQ( 1, tracker_3.update_count );
  EXPECT_EQ( 1, tracker_3.done_count );
  EXPECT_EQ( 100, tracker_3.value );
  
  tracker_1.value = 42;
  tracker_2.value = 42;
  tracker_3.value = 42;

  helper.system.start_slot( tracker_1.slot );
  
  helper.system.update( 2 );

  EXPECT_EQ( 2, tracker_1.start_count );
  EXPECT_EQ( 2, tracker_1.update_count );
  EXPECT_EQ( 1, tracker_1.done_count );
  EXPECT_EQ( 20, tracker_1.value );

  EXPECT_EQ( 1, tracker_2.start_count );
  EXPECT_EQ( 1, tracker_2.update_count );
  EXPECT_EQ( 1, tracker_2.done_count );
  EXPECT_EQ( 42, tracker_2.value );

  EXPECT_EQ( 1, tracker_3.start_count );
  EXPECT_EQ( 1, tracker_3.update_count );
  EXPECT_EQ( 1, tracker_3.done_count );
  EXPECT_EQ( 42, tracker_3.value );
  
  helper.system.update( 10 );

  EXPECT_EQ( 2, tracker_1.start_count );
  EXPECT_EQ( 3, tracker_1.update_count );
  EXPECT_EQ( 2, tracker_1.done_count );
  EXPECT_EQ( 100, tracker_1.value );

  EXPECT_EQ( 2, tracker_2.start_count );
  EXPECT_EQ( 2, tracker_2.update_count );
  EXPECT_EQ( 1, tracker_2.done_count );
  EXPECT_EQ( 20, tracker_2.value );

  EXPECT_EQ( 2, tracker_3.start_count );
  EXPECT_EQ( 2, tracker_3.update_count );
  EXPECT_EQ( 1, tracker_3.done_count );
  EXPECT_EQ( 20, tracker_3.value );
  
  helper.system.update( 8 );

  EXPECT_EQ( 2, tracker_1.start_count );
  EXPECT_EQ( 3, tracker_1.update_count );
  EXPECT_EQ( 2, tracker_1.done_count );
  EXPECT_EQ( 100, tracker_1.value );

  EXPECT_EQ( 2, tracker_2.start_count );
  EXPECT_EQ( 3, tracker_2.update_count );
  EXPECT_EQ( 2, tracker_2.done_count );
  EXPECT_EQ( 100, tracker_2.value );

  EXPECT_EQ( 2, tracker_3.start_count );
  EXPECT_EQ( 3, tracker_3.update_count );
  EXPECT_EQ( 2, tracker_3.done_count );
  EXPECT_EQ( 100, tracker_3.value );
}

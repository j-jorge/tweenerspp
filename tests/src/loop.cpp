#include "tweeners/system.hpp"

#include "test_helper.hpp"

#include <gtest/gtest.h>

TEST( tweeners, loop_on_self )
{
  test_helper helper;
  
  tweener_tracker& tracker( helper.insert( tweeners::system::not_an_id ) );

  helper.system.play_in_sequence( tracker.slot, tracker.slot );

  helper.system.update( 5 );

  EXPECT_EQ( 1, tracker.start_count );
  EXPECT_EQ( 1, tracker.update_count );
  EXPECT_EQ( 0, tracker.done_count );
  EXPECT_EQ( 50, tracker.value );

  helper.system.update( 8 );

  EXPECT_EQ( 2, tracker.start_count );

  // one update to reach the end of the tweener, one more to update the next
  // (which is the same as tracker in this case).
  EXPECT_EQ( 3, tracker.update_count );
  EXPECT_EQ( 1, tracker.done_count );
  EXPECT_EQ( 30, tracker.value );
}

TEST( tweeners, loop_on_previous )
{
  test_helper helper;
  
  tweener_tracker& tracker_1( helper.insert( tweeners::system::not_an_id ) );
  tweener_tracker& tracker_2( helper.insert( tracker_1.slot ) );

  helper.system.play_in_sequence( tracker_2.slot, tracker_1.slot );

  helper.system.update( 15 );

  EXPECT_EQ( 1, tracker_1.start_count );
  EXPECT_EQ( 1, tracker_1.update_count );
  EXPECT_EQ( 1, tracker_1.done_count );
  EXPECT_EQ( 100, tracker_1.value );

  EXPECT_EQ( 1, tracker_2.start_count );
  EXPECT_EQ( 1, tracker_2.update_count );
  EXPECT_EQ( 0, tracker_2.done_count );
  EXPECT_EQ( 50, tracker_2.value );

  helper.system.update( 10 );

  EXPECT_EQ( 2, tracker_1.start_count );
  EXPECT_EQ( 2, tracker_1.update_count );
  EXPECT_EQ( 1, tracker_1.done_count );
  EXPECT_EQ( 50, tracker_1.value );

  EXPECT_EQ( 1, tracker_2.start_count );
  EXPECT_EQ( 2, tracker_2.update_count );
  EXPECT_EQ( 1, tracker_2.done_count );
  EXPECT_EQ( 100, tracker_2.value );
}

TEST( tweeners, loop_all_the_way_back )
{
  test_helper helper;
  
  tweener_tracker& tracker_1( helper.insert( tweeners::system::not_an_id ) );
  tweener_tracker& tracker_2( helper.insert( tracker_1.slot ) );
  tweener_tracker& tracker_3( helper.insert( tracker_2.slot ) );

  helper.system.play_in_sequence( tracker_3.slot, tracker_1.slot );

  helper.system.update( 25 );

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
  EXPECT_EQ( 0, tracker_3.done_count );
  EXPECT_EQ( 50, tracker_3.value );

  helper.system.update( 10 );

  EXPECT_EQ( 2, tracker_1.start_count );
  EXPECT_EQ( 2, tracker_1.update_count );
  EXPECT_EQ( 1, tracker_1.done_count );
  EXPECT_EQ( 50, tracker_1.value );

  EXPECT_EQ( 1, tracker_2.start_count );
  EXPECT_EQ( 1, tracker_2.update_count );
  EXPECT_EQ( 1, tracker_2.done_count );
  EXPECT_EQ( 100, tracker_2.value );

  EXPECT_EQ( 1, tracker_3.start_count );
  EXPECT_EQ( 2, tracker_3.update_count );
  EXPECT_EQ( 1, tracker_3.done_count );
  EXPECT_EQ( 100, tracker_3.value );
}

/**
 * \file
 *
 * The tests in this file create a sequence made of a tweener followed by two
 * tweeners in parallel. The first tweener is removed during various events of
 * one of these parallel tweeners.
 */
#include "tweeners/system.hpp"

#include "test_helper.hpp"

#include <gtest/gtest.h>

TEST( tweeners, sequence_remove_predecessor_before_update )
{
  test_helper helper;
  
  tweener_tracker& tracker_1( helper.insert( tweeners::system<>::not_an_id ) );
  tweener_tracker& tracker_2( helper.insert( tracker_1.slot ) );
  tweener_tracker& tracker_3( helper.insert( tracker_1.slot ) );

  helper.system.update( 10 );
  helper.system.remove_slot( tracker_1.slot );
  helper.system.update( 5 );

  EXPECT_EQ( 1, tracker_1.start_count );
  EXPECT_EQ( 1, tracker_1.update_count );
  EXPECT_EQ( 1, tracker_1.done_count );
  EXPECT_EQ( 100, tracker_1.value );

  EXPECT_EQ( 1, tracker_2.start_count );
  EXPECT_EQ( 2, tracker_2.update_count );
  EXPECT_EQ( 0, tracker_2.done_count );
  EXPECT_EQ( 50, tracker_2.value );

  EXPECT_EQ( 1, tracker_3.start_count );
  EXPECT_EQ( 2, tracker_3.update_count );
  EXPECT_EQ( 0, tracker_3.done_count );
  EXPECT_EQ( 50, tracker_3.value );
}

TEST( tweeners, sequence_remove_predecessor_in_start_small_step )
{
  test_helper helper;
  
  tweener_tracker& tracker_1( helper.insert( tweeners::system<>::not_an_id ) );
  tweener_tracker& tracker_2( helper.insert( tracker_1.slot ) );
  tweener_tracker& tracker_3( helper.insert( tracker_1.slot ) );

  tracker_2.on_start =
    [ &helper, &tracker_1 ]() -> void
    {
      helper.system.remove_slot( tracker_1.slot );
    };

  helper.system.update( 10 );
  helper.system.update( 5 );

  EXPECT_EQ( 1, tracker_1.start_count );
  EXPECT_EQ( 1, tracker_1.update_count );
  EXPECT_EQ( 1, tracker_1.done_count );
  EXPECT_EQ( 100, tracker_1.value );

  EXPECT_EQ( 1, tracker_2.start_count );
  EXPECT_EQ( 2, tracker_2.update_count );
  EXPECT_EQ( 0, tracker_2.done_count );
  EXPECT_EQ( 50, tracker_2.value );

  EXPECT_EQ( 1, tracker_3.start_count );
  EXPECT_EQ( 2, tracker_3.update_count );
  EXPECT_EQ( 0, tracker_3.done_count );
  EXPECT_EQ( 50, tracker_3.value );
}

TEST( tweeners, sequence_remove_predecessor_in_start )
{
  test_helper helper;
  
  tweener_tracker& tracker_1( helper.insert( tweeners::system<>::not_an_id ) );
  tweener_tracker& tracker_2( helper.insert( tracker_1.slot ) );
  tweener_tracker& tracker_3( helper.insert( tracker_1.slot ) );

  tracker_2.on_start =
    [ &helper, &tracker_1 ]() -> void
    {
      helper.system.remove_slot( tracker_1.slot );
    };
  
  helper.system.update( 15 );

  EXPECT_EQ( 1, tracker_1.start_count );
  EXPECT_EQ( 1, tracker_1.update_count );
  EXPECT_EQ( 1, tracker_1.done_count );
  EXPECT_EQ( 100, tracker_1.value );

  EXPECT_EQ( 1, tracker_2.start_count );
  EXPECT_EQ( 1, tracker_2.update_count );
  EXPECT_EQ( 0, tracker_2.done_count );
  EXPECT_EQ( 50, tracker_2.value );

  EXPECT_EQ( 1, tracker_3.start_count );
  EXPECT_EQ( 1, tracker_3.update_count );
  EXPECT_EQ( 0, tracker_3.done_count );
  EXPECT_EQ( 50, tracker_3.value );
}

TEST( tweeners, sequence_remove_predecessor_in_update )
{
  test_helper helper;
  
  tweener_tracker& tracker_1( helper.insert( tweeners::system<>::not_an_id ) );
  tweener_tracker& tracker_2( helper.insert( tracker_1.slot ) );
  tweener_tracker& tracker_3( helper.insert( tracker_1.slot ) );

  tracker_2.on_update =
    [ &helper, &tracker_1 ]() -> void
    {
      helper.system.remove_slot( tracker_1.slot );
    };
  
  helper.system.update( 15 );

  EXPECT_EQ( 1, tracker_1.start_count );
  EXPECT_EQ( 1, tracker_1.update_count );
  EXPECT_EQ( 1, tracker_1.done_count );
  EXPECT_EQ( 100, tracker_1.value );

  EXPECT_EQ( 1, tracker_2.start_count );
  EXPECT_EQ( 1, tracker_2.update_count );
  EXPECT_EQ( 0, tracker_2.done_count );
  EXPECT_EQ( 50, tracker_2.value );

  EXPECT_EQ( 1, tracker_3.start_count );
  EXPECT_EQ( 1, tracker_3.update_count );
  EXPECT_EQ( 0, tracker_3.done_count );
  EXPECT_EQ( 50, tracker_3.value );
}

TEST( tweeners, sequence_remove_predecessor_in_done )
{
  test_helper helper;
  
  tweener_tracker& tracker_1( helper.insert( tweeners::system<>::not_an_id ) );
  tweener_tracker& tracker_2( helper.insert( tracker_1.slot ) );
  tweener_tracker& tracker_3( helper.insert( tracker_1.slot ) );

  tracker_2.on_done =
    [ &helper, &tracker_1 ]() -> void
    {
      helper.system.remove_slot( tracker_1.slot );
    };
  
  helper.system.update( 15 );
  helper.system.update( 5 );

  EXPECT_EQ( 1, tracker_1.start_count );
  EXPECT_EQ( 1, tracker_1.update_count );
  EXPECT_EQ( 1, tracker_1.done_count );
  EXPECT_EQ( 100, tracker_1.value );

  EXPECT_EQ( 1, tracker_2.start_count );
  EXPECT_EQ( 2, tracker_2.update_count );
  EXPECT_EQ( 1, tracker_2.done_count );
  EXPECT_EQ( 100, tracker_2.value );

  EXPECT_EQ( 1, tracker_3.start_count );
  EXPECT_EQ( 2, tracker_3.update_count );
  EXPECT_EQ( 1, tracker_3.done_count );
  EXPECT_EQ( 100, tracker_3.value );
}

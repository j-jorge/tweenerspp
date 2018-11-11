/**
 * \file
 *
 * The tests in this file create a sequence made of a tweener followed by two
 * tweeners in parallel. One of these parallel tweeners is then removed during
 * the process of the other.
 *
 * Since the update order of parallel tweeners is not guaranteed, the removed
 * tweener may or may not have been updated before removal.
 */
#include "tweeners/system.hpp"

#include "test_helper.hpp"

#include <gtest/gtest.h>

TEST( tweeners, sequence_remove_from_siblings_in_start_small_step )
{
  test_helper helper;
  
  tweener_tracker& tracker_1( helper.insert( tweeners::system::not_an_id ) );
  tweener_tracker& tracker_2( helper.insert( tracker_1.slot ) );
  tweener_tracker& tracker_3( helper.insert( tracker_1.slot ) );

  tracker_2.on_start =
    [ &helper, &tracker_3 ]() -> void
    {
      helper.system.remove_slot( tracker_3.slot );
    };

  helper.system.update( 5 );
  helper.system.update( 10 );

  EXPECT_EQ( 1, tracker_1.start_count );
  EXPECT_EQ( 2, tracker_1.update_count );
  EXPECT_EQ( 1, tracker_1.done_count );
  EXPECT_EQ( 100, tracker_1.value );

  EXPECT_EQ( 1, tracker_2.start_count );
  EXPECT_EQ( 1, tracker_2.update_count );
  EXPECT_EQ( 0, tracker_2.done_count );
  EXPECT_EQ( 50, tracker_2.value );

  if ( tracker_3.start_count == 0 )
    {
      EXPECT_EQ( 0, tracker_3.update_count );
      EXPECT_EQ( 0, tracker_3.done_count );
    }
  else
    {
      EXPECT_EQ( 1, tracker_3.start_count );
      EXPECT_EQ( 0, tracker_3.update_count );
      EXPECT_EQ( 0, tracker_3.done_count );
    }
}

TEST( tweeners, sequence_remove_from_siblings_in_start )
{
  test_helper helper;
  
  tweener_tracker& tracker_1( helper.insert( tweeners::system::not_an_id ) );
  tweener_tracker& tracker_2( helper.insert( tracker_1.slot ) );
  tweener_tracker& tracker_3( helper.insert( tracker_1.slot ) );

  tracker_2.on_start =
    [ &helper, &tracker_3 ]() -> void
    {
      helper.system.remove_slot( tracker_3.slot );
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

  if ( tracker_3.start_count == 0 )
    {
      EXPECT_EQ( 0, tracker_3.update_count );
      EXPECT_EQ( 0, tracker_3.done_count );
    }
  else
    {
      EXPECT_EQ( 1, tracker_3.start_count );
      EXPECT_EQ( 0, tracker_3.update_count );
      EXPECT_EQ( 0, tracker_3.done_count );
    }
}

TEST( tweeners, sequence_remove_from_siblings_in_update )
{
  test_helper helper;
  
  tweener_tracker& tracker_1( helper.insert( tweeners::system::not_an_id ) );
  tweener_tracker& tracker_2( helper.insert( tracker_1.slot ) );
  tweener_tracker& tracker_3( helper.insert( tracker_1.slot ) );

  tracker_2.on_update =
    [ &helper, &tracker_3 ]() -> void
    {
      helper.system.remove_slot( tracker_3.slot );
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

  if ( tracker_3.update_count == 1 )
    EXPECT_EQ( 50, tracker_3.value );
  else
    EXPECT_EQ( 0, tracker_3.update_count );

  EXPECT_EQ( 0, tracker_3.done_count );
}

TEST( tweeners, sequence_remove_from_siblings_in_done )
{
  test_helper helper;
  
  tweener_tracker& tracker_1( helper.insert( tweeners::system::not_an_id ) );
  tweener_tracker& tracker_2( helper.insert( tracker_1.slot ) );
  tweener_tracker& tracker_3( helper.insert( tracker_1.slot ) );

  tracker_2.on_done =
    [ &helper, &tracker_3 ]() -> void
    {
      helper.system.remove_slot( tracker_3.slot );
    };
  
  helper.system.update( 20 );

  EXPECT_EQ( 1, tracker_1.start_count );
  EXPECT_EQ( 1, tracker_1.update_count );
  EXPECT_EQ( 1, tracker_1.done_count );
  EXPECT_EQ( 100, tracker_1.value );

  EXPECT_EQ( 1, tracker_2.start_count );
  EXPECT_EQ( 1, tracker_2.update_count );
  EXPECT_EQ( 1, tracker_2.done_count );
  EXPECT_EQ( 100, tracker_1.value );

  EXPECT_EQ( 1, tracker_3.start_count );
  EXPECT_EQ( 1, tracker_3.update_count );
  EXPECT_EQ( 1, tracker_3.done_count );
  EXPECT_EQ( 100, tracker_3.value );
}


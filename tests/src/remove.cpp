#include "tweeners/builder.hpp"
#include "tweeners/easing.hpp"
#include "tweeners/system.hpp"

#include "tweener_tracker.hpp"

#include <gtest/gtest.h>

TEST( system, remove_before_update )
{
  int value( -1 );
  bool started( false );
  bool done( false );
  
  tweeners::system system;

  const tweeners::system::id_type slot
    ( tweeners::builder()
      .range_transform( 0, 100, 10, value, &tweeners::easing::linear< float > )
      .on_start( [ &started ]() -> void { started = true; } )
      .on_done( [ &done ]() -> void { done = true; } )
      .build( system ) );

  system.remove_slot( slot );
  system.update( 1 );
  
  EXPECT_EQ( -1, value );
  EXPECT_FALSE( started );
  EXPECT_FALSE( done );
}

TEST( system, remove_self_in_start )
{
  int value( -1 );
  int start_count( 0 );
  bool done( false );
  
  tweeners::system system;
  tweeners::system::id_type slot;

  auto remove
    ( [ &system, &slot, &start_count ]() -> void
      {
        system.remove_slot( slot );
        ++start_count;
      } );
  
  slot =
    tweeners::builder()
    .range_transform( 0, 100, 10, value, &tweeners::easing::linear< float > )
    .on_start( remove )
    .on_done( [ &done ]() -> void { done = true; } )
    .build( system );

  system.update( 1 );
  
  EXPECT_EQ( -1, value );
  EXPECT_EQ( 1, start_count );
  EXPECT_FALSE( done );

  system.update( 1 );
  
  EXPECT_EQ( -1, value );
  EXPECT_EQ( 1, start_count );
  EXPECT_FALSE( done );
}

TEST( system, remove_self_in_stop )
{
  int value;
  int start_count( 0 );
  int update_count( 0 );
  int done_count( 0 );
  
  tweeners::system system;
  tweeners::system::id_type slot;

  auto remove
    ( [ &system, &slot, &done_count ]() -> void
      {
        system.remove_slot( slot );
        ++done_count;
      } );

  auto update
    ( [ &value, &update_count ]( int v ) -> void
      {
        value = v;
        ++update_count;
      } );
  
  slot =
    tweeners::builder()
    .range_transform( 0, 100, 10, update, &tweeners::easing::linear< float > )
    .on_start( [ &start_count ]() -> void { ++start_count; } )
    .on_done( remove )
    .build( system );

  system.update( 1 );
  
  EXPECT_EQ( 10, value );
  EXPECT_EQ( 1, start_count );
  EXPECT_EQ( 1, update_count );
  EXPECT_EQ( 0, done_count );

  system.update( 100 );
  
  EXPECT_EQ( 100, value );
  EXPECT_EQ( 1, start_count );
  EXPECT_EQ( 2, update_count );
  EXPECT_EQ( 1, done_count );

  system.update( 100 );
  
  EXPECT_EQ( 100, value );
  EXPECT_EQ( 1, start_count );
  EXPECT_EQ( 2, update_count );
  EXPECT_EQ( 1, done_count );
}

TEST( system, remove_self_in_update )
{
  int value;
  int start_count( 0 );
  int update_count( 0 );
  int done_count( 0 );
  
  tweeners::system system;
  tweeners::system::id_type slot;

  auto update
    ( [ &system, &slot, &value, &update_count ]( int v ) -> void
      {
        system.remove_slot( slot );
        ++update_count;
        value = v;
      } );

  slot =
    tweeners::builder()
    .range_transform( 0, 100, 10, update, &tweeners::easing::linear< float > )
    .on_start( [ &start_count ]() -> void { ++start_count; } )
    .on_done( [ &done_count ]() -> void { ++done_count; } )
    .build( system );

  system.update( 1 );
  
  EXPECT_EQ( 10, value );
  EXPECT_EQ( 1, start_count );
  EXPECT_EQ( 1, update_count );
  EXPECT_EQ( 0, done_count );

  system.update( 100 );
  
  EXPECT_EQ( 10, value );
  EXPECT_EQ( 1, start_count );
  EXPECT_EQ( 1, update_count );
  EXPECT_EQ( 0, done_count );
}

TEST( system, remove_self_loop_in_done )
{
  int value;
  int start_count( 0 );
  int update_count( 0 );
  int done_count( 0 );
  
  tweeners::system system;
  tweeners::system::id_type slot;

  auto update
    ( [ &value, &update_count ]( int v ) -> void
      {
        ++update_count;
        value = v;
      } );

  auto done
    ( [ &system, &slot, &done_count ]() -> void
      {
        system.remove_slot( slot );
        ++done_count;
      } );

  slot =
    tweeners::builder()
    .range_transform( 0, 100, 10, update, &tweeners::easing::linear< float > )
    .on_start( [ &start_count ]() -> void { ++start_count; } )
    .on_done( done )
    .build( system );

  system.play_in_sequence( slot, slot );
  
  system.update( 1 );
  
  EXPECT_EQ( 10, value );
  EXPECT_EQ( 1, start_count );
  EXPECT_EQ( 1, update_count );
  EXPECT_EQ( 0, done_count );

  system.update( 10 );
  
  EXPECT_EQ( 100, value );
  EXPECT_EQ( 1, start_count );
  EXPECT_EQ( 2, update_count );
  EXPECT_EQ( 1, done_count );

  system.update( 1 );
  
  EXPECT_EQ( 100, value );
  EXPECT_EQ( 1, start_count );
  EXPECT_EQ( 2, update_count );
  EXPECT_EQ( 1, done_count );
}

TEST( system, remove_with_self_loop_then_replace_slot )
{
  tweener_tracker tracker_1;
  tweener_tracker tracker_2;
  tweener_tracker tracker_3;
  
  tracker_1.slot = -1;
  tracker_3.slot = -2;
  
  tweeners::system system;

  auto remove_1_insert_2
    ( [ &system, &tracker_1, &tracker_2, &tracker_3 ]() -> void
      {
        tracker_1.done();
        
        auto remove_2_insert_3
          ( [ &system, &tracker_2, &tracker_3 ]() -> void
            {
              tracker_2.done();
        
              system.remove_slot( tracker_2.slot );
              tracker_3.slot =
                tweeners::builder()
                .range_transform
                ( 0, 100, 10,
                  std::bind
                  ( &tweener_tracker::update, &tracker_3,
                    std::placeholders::_1 ),
                  &tweeners::easing::linear< float > )
                .on_start( std::bind( &tweener_tracker::start, &tracker_3 ) )
                .on_done( std::bind( &tweener_tracker::done, &tracker_3 ) )
                .build( system );
            } );

        system.remove_slot( tracker_1.slot );
        tracker_2.slot =
          tweeners::builder()
          .range_transform
          ( 0, 100, 10,
            std::bind
            ( &tweener_tracker::update, &tracker_2, std::placeholders::_1 ),
            &tweeners::easing::linear< float > )
          .on_start( std::bind( &tweener_tracker::start, &tracker_2 ) )
          .on_done( remove_2_insert_3 )
          .build( system );

        system.play_in_sequence( tracker_2.slot, tracker_2.slot );
      } );
  
  tracker_1.slot =
    tweeners::builder()
    .range_transform
    ( 0, 100, 10,
      std::bind( &tweener_tracker::update, &tracker_1, std::placeholders::_1 ),
      &tweeners::easing::linear< float > )
    .on_start( std::bind( &tweener_tracker::start, &tracker_1 ) )
    .on_done( remove_1_insert_2 )
    .build( system );
  system.play_in_sequence( tracker_1.slot, tracker_1.slot );

  printf( "+++\n" );
  system.update( 10 );
  printf( "+++\n" );
  
  EXPECT_EQ( 100, tracker_1.value );
  EXPECT_EQ( 1, tracker_1.start_count );
  EXPECT_EQ( 1, tracker_1.update_count );
  EXPECT_EQ( 1, tracker_1.done_count );
  
  EXPECT_EQ( 0, tracker_2.start_count );
  EXPECT_EQ( 0, tracker_2.update_count );
  EXPECT_EQ( 0, tracker_2.done_count );
  
  EXPECT_EQ( 0, tracker_3.start_count );
  EXPECT_EQ( 0, tracker_3.update_count );
  EXPECT_EQ( 0, tracker_3.done_count );
  printf( "---\n" );
  system.update( 10 );
  printf( "---\n" );
  
  EXPECT_EQ( 100, tracker_1.value );
  EXPECT_EQ( 1, tracker_1.start_count );
  EXPECT_EQ( 1, tracker_1.update_count );
  EXPECT_EQ( 1, tracker_1.done_count );
  
  EXPECT_EQ( 100, tracker_2.value );
  EXPECT_EQ( 1, tracker_2.start_count );
  EXPECT_EQ( 1, tracker_2.update_count );
  EXPECT_EQ( 1, tracker_2.done_count );
  
  EXPECT_EQ( 0, tracker_3.start_count );
  EXPECT_EQ( 0, tracker_3.update_count );
  EXPECT_EQ( 0, tracker_3.done_count );
  
  EXPECT_EQ( tracker_1.slot, tracker_3.slot );
  
  printf( "===\n" );
  system.update( 10 );
  printf( "===\n" );

  EXPECT_EQ( 100, tracker_1.value );
  EXPECT_EQ( 1, tracker_1.start_count );
  EXPECT_EQ( 1, tracker_1.update_count );
  EXPECT_EQ( 1, tracker_1.done_count );
  
  EXPECT_EQ( 100, tracker_2.value );
  EXPECT_EQ( 1, tracker_2.start_count );
  EXPECT_EQ( 1, tracker_2.update_count );
  EXPECT_EQ( 1, tracker_2.done_count );
  
  EXPECT_EQ( 100, tracker_3.value );
  EXPECT_EQ( 1, tracker_3.start_count );
  EXPECT_EQ( 1, tracker_3.update_count );
  EXPECT_EQ( 1, tracker_3.done_count );
}

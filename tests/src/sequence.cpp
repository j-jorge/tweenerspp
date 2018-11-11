#include "tweeners/builder.hpp"
#include "tweeners/easing.hpp"
#include "tweeners/system.hpp"

#include <gtest/gtest.h>

namespace
{
  struct tracker
  {
    int value;
    int update_count = 0;
    int start_count = 0;
    int done_count = 0;

    void count_start()
    {
      EXPECT_EQ( 0, update_count );
      EXPECT_EQ( done_count, start_count );
      ++start_count;
    }
    
    void update( int v )
    {
      EXPECT_EQ( 1, start_count );
      EXPECT_EQ( 0, done_count );
      ++update_count;
      value = v;
    }
    
    void count_done()
    {
      EXPECT_GE( 1, start_count );
      EXPECT_EQ( start_count - 1, done_count );
      ++done_count;
    }
  };
}

TEST( system, sequence )
{
  tracker tracker_1;
  tracker tracker_2;
  
  tweeners::system system;
  tweeners::builder builder;

  const tweeners::system::id_type slot_1
    ( builder.range_transform
      ( 0, 100, 10,
        std::bind( &tracker::update, &tracker_1, std::placeholders::_1 ),
        &tweeners::easing::linear< float > )
      .on_start( std::bind( &tracker::count_start, &tracker_1 ) )
      .on_done( std::bind( &tracker::count_done, &tracker_1 ) )
      .build( system ) );

  const tweeners::system::id_type slot_2
    ( builder.range_transform
      ( 0, 100, 10,
        std::bind( &tracker::update, &tracker_2, std::placeholders::_1 ),
        &tweeners::easing::linear< float > )
      .after( slot_1 )
      .on_start( std::bind( &tracker::count_start, &tracker_2 ) )
      .on_done( std::bind( &tracker::count_done, &tracker_2 ) )
      .build( system ) );

  EXPECT_EQ( 0, tracker_1.start_count );
  EXPECT_EQ( 0, tracker_1.update_count );
  EXPECT_EQ( 0, tracker_1.done_count );
  EXPECT_EQ( 0, tracker_2.start_count );
  EXPECT_EQ( 0, tracker_2.update_count );
  EXPECT_EQ( 0, tracker_2.done_count );
  
  system.update( 1 );
  EXPECT_EQ( 1, tracker_1.start_count );
  EXPECT_EQ( 1, tracker_1.update_count );
  EXPECT_EQ( 0, tracker_1.done_count );
  EXPECT_EQ( 10, tracker_1.value );
  EXPECT_EQ( 0, tracker_2.start_count );
  EXPECT_EQ( 0, tracker_2.update_count );
  EXPECT_EQ( 0, tracker_2.done_count );

  system.update( 8 );
  EXPECT_EQ( 1, tracker_1.start_count );
  EXPECT_EQ( 2, tracker_1.update_count );
  EXPECT_EQ( 0, tracker_1.done_count );
  EXPECT_EQ( 90, tracker_1.value );
  EXPECT_EQ( 0, tracker_2.start_count );
  EXPECT_EQ( 0, tracker_2.update_count );
  EXPECT_EQ( 0, tracker_2.done_count );

  system.update( 4 );
  EXPECT_EQ( 1, tracker_1.start_count );
  EXPECT_EQ( 3, tracker_1.update_count );
  EXPECT_EQ( 1, tracker_1.done_count );
  EXPECT_EQ( 100, tracker_1.value );
  EXPECT_EQ( 1, tracker_2.start_count );
  EXPECT_EQ( 1, tracker_2.update_count );
  EXPECT_EQ( 0, tracker_2.done_count );
  EXPECT_EQ( 30, tracker_2.value );

  system.update( 4 );
  EXPECT_EQ( 1, tracker_1.start_count );
  EXPECT_EQ( 3, tracker_1.update_count );
  EXPECT_EQ( 1, tracker_1.done_count );
  EXPECT_EQ( 100, tracker_1.value );
  EXPECT_EQ( 1, tracker_2.start_count );
  EXPECT_EQ( 2, tracker_2.update_count );
  EXPECT_EQ( 0, tracker_2.done_count );
  EXPECT_EQ( 70, tracker_2.value );

  system.update( 3 );
  EXPECT_EQ( 1, tracker_1.start_count );
  EXPECT_EQ( 3, tracker_1.update_count );
  EXPECT_EQ( 1, tracker_1.done_count );
  EXPECT_EQ( 100, tracker_1.value );
  EXPECT_EQ( 1, tracker_2.start_count );
  EXPECT_EQ( 3, tracker_2.update_count );
  EXPECT_EQ( 1, tracker_2.done_count );
  EXPECT_EQ( 100, tracker_2.value );
}

TEST( system, sequence_of_three_with_zero )
{
  tracker tracker_1;
  tracker tracker_2;
  tracker tracker_3;
  
  tweeners::system system;
  tweeners::builder builder;

  const tweeners::system::id_type slot_1
    ( builder.range_transform
      ( 0, 1, 1,
        std::bind( &tracker::update, &tracker_1, std::placeholders::_1 ),
        &tweeners::easing::linear< float > )
      .on_start( std::bind( &tracker::count_start, &tracker_1 ) )
      .on_done( std::bind( &tracker::count_done, &tracker_1 ) )
      .build( system ) );

  const tweeners::system::id_type slot_2
    ( builder.range_transform
      ( 2, 4, 1,
        std::bind( &tracker::update, &tracker_2, std::placeholders::_1 ),
        &tweeners::easing::linear< float > )
      .after( slot_1 )
      .on_start( std::bind( &tracker::count_start, &tracker_2 ) )
      .on_done( std::bind( &tracker::count_done, &tracker_2 ) )
      .build( system ) );

  builder.range_transform
    ( 3, 9, 1,
      std::bind( &tracker::update, &tracker_3, std::placeholders::_1 ),
      &tweeners::easing::linear< float > )
      .after( slot_2 )
      .on_start( std::bind( &tracker::count_start, &tracker_3 ) )
      .on_done( std::bind( &tracker::count_done, &tracker_3 ) )
      .build( system );

  EXPECT_EQ( 0, tracker_1.start_count );
  EXPECT_EQ( 0, tracker_1.update_count );
  EXPECT_EQ( 0, tracker_1.done_count );
  EXPECT_EQ( 0, tracker_2.start_count );
  EXPECT_EQ( 0, tracker_2.update_count );
  EXPECT_EQ( 0, tracker_2.done_count );
  EXPECT_EQ( 0, tracker_3.start_count );
  EXPECT_EQ( 0, tracker_3.update_count );
  EXPECT_EQ( 0, tracker_3.done_count );
  
  system.update( 1 );
  EXPECT_EQ( 1, tracker_1.start_count );
  EXPECT_EQ( 1, tracker_1.update_count );
  EXPECT_EQ( 1, tracker_1.done_count );
  EXPECT_EQ( 1, tracker_1.value );
  EXPECT_EQ( 1, tracker_2.start_count );
  EXPECT_EQ( 1, tracker_2.update_count );
  EXPECT_EQ( 0, tracker_2.done_count );
  EXPECT_EQ( 0, tracker_3.start_count );
  EXPECT_EQ( 0, tracker_3.update_count );
  EXPECT_EQ( 0, tracker_3.done_count );

  system.update( 1 );
  EXPECT_EQ( 1, tracker_1.start_count );
  EXPECT_EQ( 1, tracker_1.update_count );
  EXPECT_EQ( 1, tracker_1.done_count );
  EXPECT_EQ( 1, tracker_1.value );
  EXPECT_EQ( 1, tracker_2.start_count );
  EXPECT_EQ( 2, tracker_2.update_count );
  EXPECT_EQ( 1, tracker_2.done_count );
  EXPECT_EQ( 1, tracker_3.start_count );
  EXPECT_EQ( 1, tracker_3.update_count );
  EXPECT_EQ( 0, tracker_3.done_count );

  system.update( 1 );
  EXPECT_EQ( 1, tracker_1.start_count );
  EXPECT_EQ( 1, tracker_1.update_count );
  EXPECT_EQ( 1, tracker_1.done_count );
  EXPECT_EQ( 1, tracker_1.value );
  EXPECT_EQ( 1, tracker_2.start_count );
  EXPECT_EQ( 2, tracker_2.update_count );
  EXPECT_EQ( 1, tracker_2.done_count );
  EXPECT_EQ( 1, tracker_3.start_count );
  EXPECT_EQ( 2, tracker_3.update_count );
  EXPECT_EQ( 1, tracker_3.done_count );
}

TEST( system, sequence_of_three )
{
  tracker tracker_1;
  tracker tracker_2;
  tracker tracker_3;
  
  tweeners::system system;
  tweeners::builder builder;

  const tweeners::system::id_type slot_1
    ( builder.range_transform
      ( 0, 2, 2,
        std::bind( &tracker::update, &tracker_1, std::placeholders::_1 ),
        &tweeners::easing::linear< float > )
      .on_start( std::bind( &tracker::count_start, &tracker_1 ) )
      .on_done( std::bind( &tracker::count_done, &tracker_1 ) )
      .build( system ) );

  const tweeners::system::id_type slot_2
    ( builder.range_transform
      ( 0, 20, 2,
        std::bind( &tracker::update, &tracker_2, std::placeholders::_1 ),
        &tweeners::easing::linear< float > )
      .after( slot_1 )
      .on_start( std::bind( &tracker::count_start, &tracker_2 ) )
      .on_done( std::bind( &tracker::count_done, &tracker_2 ) )
      .build( system ) );

  builder.range_transform
    ( 0, 200, 2,
      std::bind( &tracker::update, &tracker_3, std::placeholders::_1 ),
      &tweeners::easing::linear< float > )
      .after( slot_2 )
      .on_start( std::bind( &tracker::count_start, &tracker_3 ) )
      .on_done( std::bind( &tracker::count_done, &tracker_3 ) )
      .build( system );

  EXPECT_EQ( 0, tracker_1.start_count );
  EXPECT_EQ( 0, tracker_1.update_count );
  EXPECT_EQ( 0, tracker_1.done_count );
  EXPECT_EQ( 0, tracker_2.start_count );
  EXPECT_EQ( 0, tracker_2.update_count );
  EXPECT_EQ( 0, tracker_2.done_count );
  EXPECT_EQ( 0, tracker_3.start_count );
  EXPECT_EQ( 0, tracker_3.update_count );
  EXPECT_EQ( 0, tracker_3.done_count );
  
  system.update( 1 );
  EXPECT_EQ( 1, tracker_1.start_count );
  EXPECT_EQ( 1, tracker_1.update_count );
  EXPECT_EQ( 0, tracker_1.done_count );
  EXPECT_EQ( 1, tracker_1.value );
  EXPECT_EQ( 0, tracker_2.start_count );
  EXPECT_EQ( 0, tracker_2.update_count );
  EXPECT_EQ( 0, tracker_2.done_count );
  EXPECT_EQ( 0, tracker_3.start_count );
  EXPECT_EQ( 0, tracker_3.update_count );
  EXPECT_EQ( 0, tracker_3.done_count );

  system.update( 2 );
  EXPECT_EQ( 1, tracker_1.start_count );
  EXPECT_EQ( 2, tracker_1.update_count );
  EXPECT_EQ( 1, tracker_1.done_count );
  EXPECT_EQ( 2, tracker_1.value );
  EXPECT_EQ( 1, tracker_2.start_count );
  EXPECT_EQ( 1, tracker_2.update_count );
  EXPECT_EQ( 0, tracker_2.done_count );
  EXPECT_EQ( 10, tracker_2.value );
  EXPECT_EQ( 0, tracker_3.start_count );
  EXPECT_EQ( 0, tracker_3.update_count );
  EXPECT_EQ( 0, tracker_3.done_count );

  system.update( 2 );
  EXPECT_EQ( 1, tracker_1.start_count );
  EXPECT_EQ( 2, tracker_1.update_count );
  EXPECT_EQ( 1, tracker_1.done_count );
  EXPECT_EQ( 2, tracker_1.value );
  EXPECT_EQ( 1, tracker_2.start_count );
  EXPECT_EQ( 2, tracker_2.update_count );
  EXPECT_EQ( 1, tracker_2.done_count );
  EXPECT_EQ( 20, tracker_2.value );
  EXPECT_EQ( 1, tracker_3.start_count );
  EXPECT_EQ( 1, tracker_3.update_count );
  EXPECT_EQ( 0, tracker_3.done_count );
  EXPECT_EQ( 100, tracker_3.value );

  system.update( 2 );
  EXPECT_EQ( 1, tracker_1.start_count );
  EXPECT_EQ( 2, tracker_1.update_count );
  EXPECT_EQ( 1, tracker_1.done_count );
  EXPECT_EQ( 2, tracker_1.value );
  EXPECT_EQ( 1, tracker_2.start_count );
  EXPECT_EQ( 2, tracker_2.update_count );
  EXPECT_EQ( 1, tracker_2.done_count );
  EXPECT_EQ( 20, tracker_2.value );
  EXPECT_EQ( 1, tracker_3.start_count );
  EXPECT_EQ( 2, tracker_3.update_count );
  EXPECT_EQ( 1, tracker_3.done_count );
  EXPECT_EQ( 200, tracker_3.value );
}

#include "tweeners/builder.hpp"
#include "tweeners/easing.hpp"
#include "tweeners/system.hpp"

#include <gtest/gtest.h>

TEST( system, assign_address )
{
  int value;

  tweeners::system system;
  tweeners::builder()
    .range_transform
    ( 0, 100, 10, value, &tweeners::easing::linear< float > )
    .build( system );

  system.update( 1 );
  EXPECT_EQ( 10, value );

  system.update( 1 );
  EXPECT_EQ( 20, value );

  system.update( 1 );
  EXPECT_EQ( 30, value );

  system.update( 2 );
  EXPECT_EQ( 50, value );

  system.update( 3 );
  EXPECT_EQ( 80, value );

  system.update( 3 );
  EXPECT_EQ( 100, value );
}

TEST( system, update_callback )
{
  int value;
  int calls( 0 );
  
  const auto update
    ( [ &value, &calls ]( int v ) -> void
      {
        value = v;
        ++calls;
      } );
  
  tweeners::system system;
  tweeners::builder()
    .range_transform
    ( 0, 100, 10, update, &tweeners::easing::linear< float > )
    .build( system );

  system.update( 1 );
  EXPECT_EQ( 1, calls );
  EXPECT_EQ( 10, value );

  system.update( 1 );
  EXPECT_EQ( 2, calls );
  EXPECT_EQ( 20, value );

  system.update( 1 );
  EXPECT_EQ( 3, calls );
  EXPECT_EQ( 30, value );

  system.update( 2 );
  EXPECT_EQ( 4, calls );
  EXPECT_EQ( 50, value );

  system.update( 3 );
  EXPECT_EQ( 5, calls );
  EXPECT_EQ( 80, value );

  system.update( 3 );
  EXPECT_EQ( 6, calls );
  EXPECT_EQ( 100, value );
}

TEST( system, update_object )
{
  int value;
  int calls( 0 );
  
  struct update
  {
    int& value;
    int& calls;
    
    void operator()( int v ) const
    {
      value = v;
      ++calls;
    };
  };
  
  tweeners::system system;
  tweeners::builder()
    .range_transform
    ( 0, 100, 10, update{ value, calls }, &tweeners::easing::linear< float > )
    .build( system );

  system.update( 1 );
  EXPECT_EQ( 1, calls );
  EXPECT_EQ( 10, value );

  system.update( 1 );
  EXPECT_EQ( 2, calls );
  EXPECT_EQ( 20, value );

  system.update( 1 );
  EXPECT_EQ( 3, calls );
  EXPECT_EQ( 30, value );

  system.update( 2 );
  EXPECT_EQ( 4, calls );
  EXPECT_EQ( 50, value );

  system.update( 3 );
  EXPECT_EQ( 5, calls );
  EXPECT_EQ( 80, value );

  system.update( 3 );
  EXPECT_EQ( 6, calls );
  EXPECT_EQ( 100, value );
}

TEST( system, lambda_transform_assign_address )
{
  int calls( 0 );
  int value;
  
  const auto transform
    ( [ &calls ]( float r ) -> float
      {
        ++calls;
        
        static int result( 0 );
        result = ( result + 1 ) %2;
        return result;
      } );
  
  tweeners::system system;
  tweeners::builder()
    .range_transform( 0, 100, 10, value, transform )
    .build( system );

  system.update( 1 );
  EXPECT_EQ( 1, calls );
  EXPECT_EQ( 100, value );

  system.update( 1 );
  EXPECT_EQ( 2, calls );
  EXPECT_EQ( 0, value );

  system.update( 1 );
  EXPECT_EQ( 3, calls );
  EXPECT_EQ( 100, value );

  system.update( 2 );
  EXPECT_EQ( 4, calls );
  EXPECT_EQ( 0, value );

  system.update( 3 );
  EXPECT_EQ( 5, calls );
  EXPECT_EQ( 100, value );

  system.update( 3 );
  EXPECT_EQ( 6, calls );
  EXPECT_EQ( 0, value );
}

TEST( system, lambda_transform_update_callback )
{
  int update_calls( 0 );
  int value;
  
  const auto update
    ( [ &value, &update_calls ]( int v ) -> void
      {
        value = v;
        ++update_calls;
      } );

  int transform_calls( 0 );
  
  const auto transform
    ( [ &transform_calls ]( float r ) -> float
      {
        ++transform_calls;
        
        static int result( 0 );
        result = ( result + 1 ) %2;
        return result;
      } );
  
  tweeners::system system;
  tweeners::builder()
    .range_transform( 0, 100, 10, update, transform )
    .build( system );

  system.update( 1 );
  EXPECT_EQ( 1, update_calls );
  EXPECT_EQ( 1, transform_calls );
  EXPECT_EQ( 100, value );

  system.update( 1 );
  EXPECT_EQ( 2, update_calls );
  EXPECT_EQ( 2, transform_calls );
  EXPECT_EQ( 0, value );

  system.update( 1 );
  EXPECT_EQ( 3, update_calls );
  EXPECT_EQ( 3, transform_calls );
  EXPECT_EQ( 100, value );

  system.update( 2 );
  EXPECT_EQ( 4, update_calls );
  EXPECT_EQ( 4, transform_calls );
  EXPECT_EQ( 0, value );

  system.update( 3 );
  EXPECT_EQ( 5, update_calls );
  EXPECT_EQ( 5, transform_calls );
  EXPECT_EQ( 100, value );

  system.update( 3 );
  EXPECT_EQ( 6, update_calls );
  EXPECT_EQ( 6, transform_calls );
  EXPECT_EQ( 0, value );
}

TEST( system, lambda_transform_update_object )
{
  int update_calls( 0 );
  int value;
  
  struct update
  {
    int& value;
    int& calls;
    
    void operator()( int v ) const
    {
      value = v;
      ++calls;
    };
  };

  int transform_calls( 0 );
  
  const auto transform
    ( [ &transform_calls ]( float r ) -> float
      {
        ++transform_calls;
        
        static int result( 0 );
        result = ( result + 1 ) %2;
        return result;
      } );
  
  tweeners::system system;
  tweeners::builder()
    .range_transform
    ( 0, 100, 10, update{ value, update_calls }, transform )
    .build( system );

  system.update( 1 );
  EXPECT_EQ( 1, update_calls );
  EXPECT_EQ( 1, transform_calls );
  EXPECT_EQ( 100, value );

  system.update( 1 );
  EXPECT_EQ( 2, update_calls );
  EXPECT_EQ( 2, transform_calls );
  EXPECT_EQ( 0, value );

  system.update( 1 );
  EXPECT_EQ( 3, update_calls );
  EXPECT_EQ( 3, transform_calls );
  EXPECT_EQ( 100, value );

  system.update( 2 );
  EXPECT_EQ( 4, update_calls );
  EXPECT_EQ( 4, transform_calls );
  EXPECT_EQ( 0, value );

  system.update( 3 );
  EXPECT_EQ( 5, update_calls );
  EXPECT_EQ( 5, transform_calls );
  EXPECT_EQ( 100, value );

  system.update( 3 );
  EXPECT_EQ( 6, update_calls );
  EXPECT_EQ( 6, transform_calls );
  EXPECT_EQ( 0, value );
}

TEST( system, object_transform_assign_address )
{
  int calls( 0 );
  int value;
  
  struct transform
  {
    int& calls;
    
    float operator()( float r ) const
    {
      ++calls;
        
      static int result( 0 );
      result = ( result + 1 ) %2;
      return result;
    }
  };
  
  tweeners::system system;
  tweeners::builder()
    .range_transform( 0, 100, 10, value, transform{ calls } )
    .build( system );

  system.update( 1 );
  EXPECT_EQ( 1, calls );
  EXPECT_EQ( 100, value );

  system.update( 1 );
  EXPECT_EQ( 2, calls );
  EXPECT_EQ( 0, value );

  system.update( 1 );
  EXPECT_EQ( 3, calls );
  EXPECT_EQ( 100, value );

  system.update( 2 );
  EXPECT_EQ( 4, calls );
  EXPECT_EQ( 0, value );

  system.update( 3 );
  EXPECT_EQ( 5, calls );
  EXPECT_EQ( 100, value );

  system.update( 3 );
  EXPECT_EQ( 6, calls );
  EXPECT_EQ( 0, value );
}

TEST( system, object_transform_update_callback )
{
  int update_calls( 0 );
  int value;
  
  const auto update
    ( [ &value, &update_calls ]( int v ) -> void
      {
        value = v;
        ++update_calls;
      } );

  int transform_calls( 0 );
  
  struct transform
  {
    int& calls;
    
    float operator()( float r ) const
    {
      ++calls;
        
      static int result( 0 );
      result = ( result + 1 ) %2;
      return result;
    }
  };
  
  tweeners::system system;
  tweeners::builder()
    .range_transform
    ( 0, 100, 10, update, transform{ transform_calls } )
    .build( system );

  system.update( 1 );
  EXPECT_EQ( 1, update_calls );
  EXPECT_EQ( 1, transform_calls );
  EXPECT_EQ( 100, value );

  system.update( 1 );
  EXPECT_EQ( 2, update_calls );
  EXPECT_EQ( 2, transform_calls );
  EXPECT_EQ( 0, value );

  system.update( 1 );
  EXPECT_EQ( 3, update_calls );
  EXPECT_EQ( 3, transform_calls );
  EXPECT_EQ( 100, value );

  system.update( 2 );
  EXPECT_EQ( 4, update_calls );
  EXPECT_EQ( 4, transform_calls );
  EXPECT_EQ( 0, value );

  system.update( 3 );
  EXPECT_EQ( 5, update_calls );
  EXPECT_EQ( 5, transform_calls );
  EXPECT_EQ( 100, value );

  system.update( 3 );
  EXPECT_EQ( 6, update_calls );
  EXPECT_EQ( 6, transform_calls );
  EXPECT_EQ( 0, value );
}

TEST( system, object_transform_update_object )
{
  int update_calls( 0 );
  int value;
  
  struct update
  {
    int& value;
    int& calls;
    
    void operator()( int v ) const
    {
      value = v;
      ++calls;
    };
  };

  int transform_calls( 0 );
  
  struct transform
  {
    int& calls;
    
    float operator()( float r ) const
    {
      ++calls;
        
      static int result( 0 );
      result = ( result + 1 ) %2;
      return result;
    }
  };
  
  tweeners::system system;
  tweeners::builder()
    .range_transform
    ( 0, 100, 10, update{ value, update_calls}, transform{ transform_calls } )
    .build( system );

  system.update( 1 );
  EXPECT_EQ( 1, update_calls );
  EXPECT_EQ( 1, transform_calls );
  EXPECT_EQ( 100, value );

  system.update( 1 );
  EXPECT_EQ( 2, update_calls );
  EXPECT_EQ( 2, transform_calls );
  EXPECT_EQ( 0, value );

  system.update( 1 );
  EXPECT_EQ( 3, update_calls );
  EXPECT_EQ( 3, transform_calls );
  EXPECT_EQ( 100, value );

  system.update( 2 );
  EXPECT_EQ( 4, update_calls );
  EXPECT_EQ( 4, transform_calls );
  EXPECT_EQ( 0, value );

  system.update( 3 );
  EXPECT_EQ( 5, update_calls );
  EXPECT_EQ( 5, transform_calls );
  EXPECT_EQ( 100, value );

  system.update( 3 );
  EXPECT_EQ( 6, update_calls );
  EXPECT_EQ( 6, transform_calls );
  EXPECT_EQ( 0, value );
}

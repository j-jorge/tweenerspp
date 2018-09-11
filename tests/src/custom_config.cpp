#include "tweeners/builder.hpp"
#include "tweeners/easing.hpp"
#include "tweeners/system.hpp"

#include <type_traits>

#include <gtest/gtest.h>

template< typename Signature >
class callable
{
public:
  typedef typename std::function< Signature >::result_type result_type;
  
public:
  callable() = default;
  
  template
  <
    typename F,
    class = typename std::enable_if
      <
        !std::is_same< F, callable< Signature > >::value
      >::type
  >
  callable( F function )
    : m_function( function )
  {
    
  }

  template< typename... Arg >
  result_type operator()( Arg&&... arg ) const
  {
    return m_function( std::forward< Arg >( arg )... );
  }

  operator bool() const { return !!m_function; }
  
private:
  std::function< Signature > m_function;
};
  
struct custom_config
{
  using duration_type = int;
  using id_type = std::uint8_t;
  using float_type = double;

  template< typename S >
  using function_type = callable< S >;
};

TEST( system, custom_config )
{
  int value;

  tweeners::system< custom_config > system;
  tweeners::builder< custom_config >()
    .range_transform( 0, 100, 10, value, &tweeners::easing::linear< float > )
    .build( system );

  system.update( 1 );
  EXPECT_FLOAT_EQ( 10, value );

  system.update( 3 );
  EXPECT_FLOAT_EQ( 40, value );
}

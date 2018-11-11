#include "test_helper.hpp"

#include "tweeners/builder.hpp"
#include "tweeners/easing.hpp"

#include <cassert>

tweener_tracker&
test_helper::insert( tweeners::system::id_type previous )
{
  assert( m_next_tracker < m_trackers.size() );
  
  tweener_tracker& result( m_trackers[ m_next_tracker ] );
  ++m_next_tracker;

  auto start( std::bind( &tweener_tracker::start, &result ) );
  auto update
    ( std::bind( &tweener_tracker::update, &result, std::placeholders::_1 ) );
  auto done( std::bind( &tweener_tracker::done, &result ) );

  result.slot =
    tweeners::builder()
    .range_transform( 0, 100, 10, update, &tweeners::easing::linear< float > )
    .on_start( start )
    .on_done( done )
    .after( previous )
    .build( system );

  return result;
}


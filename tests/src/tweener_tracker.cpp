#include "tweener_tracker.hpp"

#include <cassert>

void tweener_tracker::start()
{
  ++start_count;

  if ( on_start )
    on_start();
}
    
void tweener_tracker::update( int v )
{
  ++update_count;
  value = v;

  if ( on_update )
    on_update();
}
    
void tweener_tracker::done()
{
  ++done_count;

  if ( on_done )
    on_done();
}

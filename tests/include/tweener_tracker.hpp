#ifndef TWEENER_TRACKER_HPP
#define TWEENER_TRACKER_HPP

#include "tweeners/system.hpp"

struct tweener_tracker
{
  tweeners::system<>::id_type slot;
  int value;
  int update_count = 0;
  int start_count = 0;
  int done_count = 0;

  std::function< void() > on_start;
  std::function< void() > on_update;
  std::function< void() > on_done;
    
  void start();
  void update( int v );
  void done();
};

#endif

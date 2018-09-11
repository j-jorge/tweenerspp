#ifndef TEST_HELPER_HPP
#define TEST_HELPER_HPP

#include "tweener_tracker.hpp"

#include <array>

class test_helper
{
public:
  tweener_tracker& insert( tweeners::system<>::id_type previous );

  tweeners::system<> system;
  
private:
  std::array< tweener_tracker, 3 > m_trackers;
  std::size_t m_next_tracker = 0;
};

#endif

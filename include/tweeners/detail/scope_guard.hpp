#ifndef TWEENERS_DETAIL_SCOPE_GUARD_HPP
#define TWEENERS_DETAIL_SCOPE_GUARD_HPP

namespace tweeners
{
  namespace detail
  {
    /**
     * \brief Calls a function when entering and leaving the scope.
     *
     * scope_guard calls the provided function when it is constructed and when
     * it is destroyed.
     */
    template< typename Check >
    class scope_guard
    {
      // Naming issue? From there it does not look like a guard, it is more like
      // the "scope exit" paradigm but also with "scope enter".
      
    public:
      explicit scope_guard( Check p )
        : m_check( std::move( p ) )
      {
        m_check();
      }
      
      ~scope_guard() { m_check(); }

    private:
      Check m_check;
    };
  }
}

#endif

#ifndef TWEENERS_DETAIL_DEBUG_HPP
#define TWEENERS_DETAIL_DEBUG_HPP
/** \file */

/**
 * \def TWEENERS_DEBUG
 *
 * \brief Define this macro to enable assertions in the library's internals.
 *
 * These assertions are here to enforce the assumptions made by the developers
 * of this Tweeners library. They can safely be disabled in client code.
 */
#ifdef TWEENERS_DEBUG
  #include <tweeners/detail/scope_guard.hpp>
  #include <cassert>

  #define tweeners_debug_assert( b ) assert( b )

  #define tweeners_debug_declare_scope_guard_impl( predicate, counter )   \
    auto scope_guard_predicate_ ## counter( predicate );                  \
    const tweeners::detail::scope_guard                                 \
    <                                                                   \
      decltype( scope_guard_predicate_ ## counter )                     \
    >                                                                   \
    scope_guard_ ## counter( std::move( scope_guard_predicate_ ## counter ) )

  #define tweeners_debug_declare_scope_guard( predicate ) \
    tweeners_debug_declare_scope_guard_impl( predicate, __COUNTER__ )

#else

  #define tweeners_debug_assert( b ) do {} while ( false )
  #define tweeners_debug_declare_scope_guard( predicate ) \
    do {} while ( false )
  
#endif

#endif

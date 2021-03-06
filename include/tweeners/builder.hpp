#ifndef TWEENERS_BUILDER_HPP
#define TWEENERS_BUILDER_HPP

#include <tweeners/config.hpp>

namespace tweeners
{
  template< typename Config >
  class system_base;

  /**
   * \brief Convenience object to insert new tweeners in a tweeners::system.
   *
   * The intent of this class is to allow client code to ignore the call-order
   * requirements of tweeners::system.
   *
   * builder stores the information relative to the tweener to create and
   * ensures that the functions in tweeners::system are called in the expected
   * order to actually create the tweener.
   */
  template< typename Config = config<> >
  class builder_base
  {
  public:
    using duration_type = typename Config::duration_type;
    using id_type = typename Config::id_type;
    using float_type = typename Config::float_type;

    template< typename Signature >
    using function_type = typename Config::template function_type< Signature >;

  public:
    builder_base();
    
    template< typename T, typename Transform >
    builder_base& range_transform
    ( T from, T to, duration_type duration, T& target,
      Transform transform );

    template< typename T, typename Update, typename Transform >
    builder_base& range_transform
    ( T from, T to, duration_type duration,
      Update update_callback,
      Transform transform );

    builder_base& on_start( function_type< void() > callback );
    builder_base& on_done( function_type< void() > callback );
    builder_base& after( id_type slot_id );

    id_type build( system_base< Config >& system );

  private:
    duration_type m_duration;
    function_type< void( float_type ) > m_update;
    function_type< float_type( float_type ) > m_transform;

    function_type< void() > m_on_start;
    function_type< void() > m_on_done;

    id_type m_previous;
  };

  using builder = builder_base<>;
}  

#include <tweeners/detail/builder.tpp>

#endif

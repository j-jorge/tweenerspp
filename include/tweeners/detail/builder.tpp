#ifndef TWEENERS_BUILDER_TPP
#define TWEENERS_BUILDER_TPP

#include <tweeners/contract.hpp>
#include <tweeners/system.hpp>

template< typename Config >
tweeners::builder_base< Config >::builder_base()
  : m_previous( system_base< Config >::not_an_id )
{

}

/**
 * \brief Configure a tweener to update a variable from the client code.
 *
 * \param from The value to assign to the variable when the tweener starts.
 *
 * \param to The value of the variable when the tweener ends.
 *
 * \param duration How long it takes to iterate from \p from to \p to.
 *
 * \param target The variable receiving the updates as the tweener
 *        progresses. Obviously the target must outlive the tweeners::system.
 *
 * \param transform The curve to follow to go from \p from to \p to. \sa
 * tweeners::easing.
 */
template< typename Config >
template< typename T, typename Transform >
tweeners::builder_base< Config >&
tweeners::builder_base< Config >::range_transform
( T from, T to, duration_type duration, T& target, Transform transform )
{
  auto update
    ( [ &target ]( T value ) -> void
      {
        target = value;
      } );

  // Should I std::move() the arguments or will the compiler do it for me?
  return range_transform( from, to, duration, update, transform );
}

/**
 * \brief Configure a tweener to update a variable from the client code.
 *
 * \param from The value to assign to the variable when the tweener starts.
 *
 * \param to The value of the variable when the tweener ends.
 *
 * \param duration How long it takes to iterate from \p from to \p to.
 *
 * \param target The function receiving the updates as the tweener progresses.
 *
 * \param transform The curve to follow to go from \p from to \p to. \sa
 * tweeners::easing.
 */
template< typename Config >
template< typename T, typename Update, typename Transform >
tweeners::builder_base< Config >&
tweeners::builder_base< Config >::range_transform
( T from, T to, duration_type duration, Update update_callback,
  Transform transform )
{
  m_update =
    [ update_callback, from, to ]( float_type ratio ) -> void
    {
      update_callback( from + ratio * ( to - from ) );
    };
  // Again, std::move()?
  m_duration = duration;
  m_transform = transform;
  
  tweeners_confirm_contract
    ( m_transform, "tweeners::builder: The transform function is not valid." );
  
  return *this;
}

/**
 * \brief Sets the function to call when the tweener starts (optional).
 */
template< typename Config >
tweeners::builder_base< Config >& tweeners::builder_base< Config >::on_start
( function_type< void() > callback )
{
  m_on_start = callback;
  return *this;
}

/**
 * \brief Sets the function to call when the tweener has completed its progress
 *        (optional).
 */
template< typename Config >
tweeners::builder_base< Config >&
tweeners::builder_base< Config >::on_done( function_type< void() > callback )
{
  m_on_done = callback;
  return *this;
}

/**
 * \brief Sets the identifier of the tweener after which the tweener will be
 *        sequenced (optional).
 */
template< typename Config >
tweeners::builder_base< Config >&
tweeners::builder_base< Config >::after( id_type slot_id )
{
  m_previous = slot_id;
  return *this;
}

/**
 * \brief Actually create a new tweener in a system.
 *
 * The tweener is created with the parameters passed to the other functions of
 * the builder.
 *
 * \return The identifier of the created tweener.
 */
template< typename Config >
typename Config::id_type tweeners::builder_base< Config >::build
( system_base< Config >& system )
{
  tweeners_confirm_contract
    ( m_update,
      "tweeners::builder: update function is not set. Did you call"
      " range_transform()?" );
  tweeners_confirm_contract
    ( m_transform,
      "tweeners::builder: are you trying to insert the same tweener twice?" );
  
  const id_type slot
    ( system.configure_slot
      ( std::move( m_duration ), std::move( m_update ),
        std::move( m_transform ) ) );

  if ( m_on_start )
    system.on_slot_start( slot, std::move( m_on_start ) );

  if ( m_on_done )
    system.on_slot_done( slot, std::move( m_on_done ) );

  if ( m_previous == tweeners::system_base< Config >::not_an_id )
    system.start_slot( slot );
  else
    system.play_in_sequence( m_previous, slot );
  
  return slot;
}

#endif

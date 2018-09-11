#ifndef TWEENERS_DETAIL_SLOT_COMPONENT_TPP
#define TWEENERS_DETAIL_SLOT_COMPONENT_TPP

#include <tweeners/detail/debug.hpp>

#define tweeners_debug_slot_component_invariant() \
  tweeners_debug_declare_scope_guard              \
  ( [ this ]() -> void { check_invariants(); } )

template< typename T, typename Id >
tweeners::detail::slot_component< T, Id >::slot_component
( T default_value )
  : m_values( { std::move( default_value ) } ),
    m_slot_from_value_index( { {} } )
{
  tweeners_debug_slot_component_invariant();
}

/**
 * \brief Allocate enough storage for the given number of slots and values.
 *
 * \param slot_count The number of slots.
 *
 * \param value_count The number of values. (i.e. The number of slots expected
 *        not to keep the default value.
 */
template< typename T, typename Id >
void
tweeners::detail::slot_component< T, Id >::reserve
( std::size_t slot_count, std::size_t value_count )
{
  tweeners_debug_slot_component_invariant();

  m_values.reserve( value_count );
  m_slot_from_value_index.reserve( value_count );

  m_value_index_from_slot.reserve( slot_count );
}

/**
 * \brief Add… one slot… at the container's… end.
 *
 * The The default value is assigned to the created entry. Conceptually this is
 * the value of the slot identified by the value m_value_index_from_slot.size()
 * just prior the call.
 */
template< typename T, typename Id >
void tweeners::detail::slot_component< T, Id >::add_one_slot_at_end()
{
  tweeners_debug_slot_component_invariant();
  
  m_value_index_from_slot.emplace_back( 0 );
}

/**
 * \brief Assign a value to a given slot.
 *
 * \param slot_id The slot to which the value is assigned.
 *
 * \param value The arguments to pass to the constructor of `T` to construct the
 *        value assigned to slot_id.
 *
 * The storage for the slot must have been created with a prior call to
 * add_one_slot_at_end().
 */
template< typename T, typename Id >
template< typename... Args >
void tweeners::detail::slot_component< T, Id >::emplace
( Id slot_id, Args&&... value )
{
  tweeners_debug_slot_component_invariant();

  tweeners_debug_assert( slot_id >= 0 );
  tweeners_debug_assert( slot_id < m_value_index_from_slot.size() );
  
  const std::size_t value_index( m_values.size() );

  m_values.emplace_back( std::forward< Args >( value )... );
  m_slot_from_value_index.emplace_back( slot_id );
  
  m_value_index_from_slot[ slot_id ] = value_index;
}

/**
 * \brief Tell if a slot has received a value with emplace.
 *
 * \param slot_id The slot to test.
 *
 * \return true if `emplace( slot_id, ... )` has been called, false otherwise.
 *
 * The storage for the slot must have been created with a prior call to
 * add_one_slot_at_end().
 */
template< typename T, typename Id >
bool tweeners::detail::slot_component< T, Id >::has_value( Id slot_id ) const
{
  tweeners_debug_slot_component_invariant();
  
  tweeners_debug_assert( slot_id >= 0 );
  tweeners_debug_assert( slot_id < m_value_index_from_slot.size() );

  return m_value_index_from_slot[ slot_id  ] != 0;
}

/**
 * \brief Return the non-default value associated with a given slot.
 *
 * \param slot_id The slot of which the value is returned.
 *
 * \pre slot_id must have a specific value assigned to it by a call to
 *      `emplace( slot_id, ... )`
 *
 * The storage for the slot must have been created with a prior call to
 * add_one_slot_at_end().
 */
template< typename T, typename Id >
T& tweeners::detail::slot_component< T, Id >::get_existing( Id slot_id )
{
  tweeners_debug_slot_component_invariant();
  
  tweeners_debug_assert( slot_id >= 0 );
  tweeners_debug_assert( slot_id < m_value_index_from_slot.size() );
  tweeners_debug_assert( m_value_index_from_slot[ slot_id ] != 0 );

  return m_values[ m_value_index_from_slot[ slot_id  ] ];
}

/**
 * \brief Return the value associated with a given slot.
 *
 * \param slot_id The slot whose value is returned.
 *
 * If the slot has a custom value, this value is returned. Otherwise the
 * function returns the default value.
 *
 * The storage for the slot must have been created with a prior call to
 * add_one_slot_at_end().
 */
template< typename T, typename Id >
const T&
tweeners::detail::slot_component< T, Id >::operator[]( Id slot_id ) const
{
  tweeners_debug_slot_component_invariant();

  tweeners_debug_assert( slot_id >= 0 );
  tweeners_debug_assert( slot_id < m_value_index_from_slot.size() );

  return m_values[ m_value_index_from_slot[ slot_id ] ];
}

/**
 * \brief Erase the non-default values associated with the slots represented in
 *        the given range.
 *
 * Non-default values of each slot id dereferenced by iterators in the range
 * [first, last) are destroyed then the default value is assigned to the slot. 
 */
template< typename T, typename Id >
template< typename Iterator >
void tweeners::detail::slot_component< T, Id >::erase
( Iterator first, Iterator last )
{
  tweeners_debug_slot_component_invariant();

  std::size_t values_size( m_values.size() );

  for ( ; first != last; ++first )
    {
      tweeners_debug_assert( *first >= 0 );
      tweeners_debug_assert( *first < m_value_index_from_slot.size() );

      std::size_t& value_index( m_value_index_from_slot[ *first ] );

      if ( value_index != 0 )
        {
          --values_size;

          m_values[ value_index ] = std::move( m_values[ values_size ] );

          const std::size_t moved_slot
            ( m_slot_from_value_index[ values_size ] );
          m_value_index_from_slot[ moved_slot ] = value_index;
          m_slot_from_value_index[ value_index ] = moved_slot;
          
          value_index = 0;
        }
    }

  m_values.erase( m_values.begin() + values_size, m_values.end() );
  m_slot_from_value_index.erase
    ( m_slot_from_value_index.begin() + values_size,
      m_slot_from_value_index.end() );
}

template< typename T, typename Id >
void tweeners::detail::slot_component< T, Id >::check_invariants() const
{
  const std::size_t value_count( m_values.size() );
  const std::size_t slot_count( m_value_index_from_slot.size() );
  
  tweeners_debug_assert( value_count == m_slot_from_value_index.size() );
  
  for ( std::size_t slot( 0 ); slot != slot_count; ++slot )
    {
      const std::size_t value_index( m_value_index_from_slot[ slot ] );
      
      tweeners_debug_assert( value_index < value_count );

      tweeners_debug_assert
        ( ( value_index == 0 )
          || ( m_slot_from_value_index[ value_index ] == slot ) );
    }

  for ( std::size_t value_index( 1 ); value_index != value_count;
        ++value_index )
    tweeners_debug_assert
      ( m_slot_from_value_index[ value_index ] < slot_count );
}

#undef tweeners_debug_slot_component_invariant

#endif

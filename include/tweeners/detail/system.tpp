#ifndef TWEENERS_SYSTEM_TPP
#define TWEENERS_SYSTEM_TPP

#include <tweeners/contract.hpp>
#include <tweeners/detail/debug.hpp>

#include <algorithm>
#include <chrono>

#define tweeners_debug_validate_id( id )                        \
  do                                                            \
    {                                                           \
      tweeners_debug_assert( ( id ) != not_an_id );             \
      tweeners_debug_assert( ( id ) >= 0 );                     \
      tweeners_debug_assert( ( id ) < m_slot_states.size() );   \
    }                                                           \
  while ( false )

#define tweeners_debug_not_in_container( value, container )     \
  tweeners_debug_assert                                         \
  ( std::find( container.begin(), container.end(), value )      \
    == container.end() );                                       \

#define tweeners_debug_system_invariant()         \
  tweeners_debug_declare_scope_guard              \
  ( [ this ]() -> void { check_invariants(); } )

namespace tweeners
{
  namespace detail
  {
    template< typename Float, typename T >
    Float to_float( T value )
    {
      return value;
    }

    template< typename Float, typename Rep, typename Period >
    Float to_float
    ( const std::chrono::duration< Rep, Period >& value )
    {
      return value.count();
    }
  }
}

template< typename Config >
tweeners::system_base< Config >::system_base()
  : m_start_functions( []() -> void {} ),
    m_done_functions( []() -> void {} ),
    m_successors( {} )
{
  tweeners_debug_system_invariant();
}

/**
 * \brief Allocate memory for a given run configuration.
 *
 * \param slot_count The number of slots for which to allocate memory.
 *
 * \param value_count_per_component The number of values to allocate for each
 *        slot component. See it as an estimate of. how many slots are expected
 *        to have a start callback, an end callback or being continued by
 *        another slot (see system::play_in_sequence).
 *
 * \param simultaneous_count The number of slots that are expected to be running
 *        simultaneously.
 */
template< typename Config >
void tweeners::system_base< Config >::reserve
( std::size_t slot_count, std::size_t value_count_per_component,
  std::size_t simultaneous_count )
{
  tweeners_debug_system_invariant();

  m_slot_states.reserve( slot_count );
  m_slot.reserve( slot_count );
  m_current_dates.reserve( slot_count );
  
  m_start_functions.reserve( slot_count, value_count_per_component );
  m_done_functions.reserve( slot_count, value_count_per_component );
  m_successors.reserve( slot_count, value_count_per_component );

  m_start_queue.reserve( simultaneous_count );
  m_done_queue.reserve( simultaneous_count );
  m_dead_queue.reserve( simultaneous_count );
  m_sequence_queue.reserve( simultaneous_count );
  m_available_ids.reserve( simultaneous_count );
  m_need_update.reserve( simultaneous_count );
}

/**
 * \brief Prepare a slot for being run.
 *
 * \param duration How long the interpolation takes.
 *
 * \param update The function to call at every update. Its single argument
 *        receives the ratio of the time since the start of the tweener, as
 *        transformed by the following transform argument.
 *
 * \param transform The transformation to apply to the ratio of time since the
 *        beginning of the tweener. See file tweeners/easing.hpp for predefined
 *        functions.
 *
 * \return The identifier to the created slot.
 *
 * Note that this function prepares the tweener but does not start it. See \sa
 * start_slot and \a tweeners::builder.
 */
template< typename Config >
typename tweeners::system_base< Config >::id_type
tweeners::system_base< Config >::configure_slot
( duration_type duration, update_function update, transform_function transform )
{
  tweeners_debug_system_invariant();

  const id_type id( create_slot() );

  tweener_state& tweener( m_slot[ id ] );
  
  tweener.end_date = duration;
  tweener.previous = not_an_id;
  tweener.on_update = std::move( update );
  tweener.transform = std::move( transform );

  m_slot_states[ id ] = slot_state::ready;
  
  return id;
}

/**
 * \brief Starts a slot previously configured with configure_slot.
 *
 * \param slot_id The identifier of the slot previously configured with
 *        configure_slot.
 *
 * The slot does not actually start during this calls but becomes scheduled to
 * run in the next updates.
 *
 * See \sa play_in_sequence to schedule a slot after another one.
 */
template< typename Config >
void tweeners::system_base< Config >::start_slot( id_type slot_id )
{
  tweeners_debug_system_invariant();

  tweeners_confirm_contract
    ( is_valid_slot_id( slot_id ),
      "system::start_slot(): slot does not exist." );
  tweeners_confirm_contract
    ( m_slot_states[ slot_id ] == slot_state::ready,
      "system::start_slot(): slot should be in ready state." );
  tweeners_confirm_contract
    ( std::find( m_start_queue.begin(), m_start_queue.end(), slot_id )
      == m_start_queue.end(),
      "system::start_slot(): slot is already started." );
  tweeners_confirm_contract
    ( m_slot[ slot_id ].previous == not_an_id,
      "system::start_slot(): slot is already scheduled after another one." );

  m_current_dates[ slot_id ] = duration_type();
  m_start_queue.emplace_back( slot_id );
}

/**
 * \brief Assign a function to be called when the given slot starts running.
 *
 * \param slot_id The identifier of the slot to link with the function.
 * \param callback The function to call when the slot starts.
 *
 * The function will be called the next and every time the slot starts running
 * (either scheduled with start_slot or sequenced after another running slot).
 *
 * See \sa configure_slot to create a slot_id.
 */
template< typename Config >
void tweeners::system_base< Config >::on_slot_start
( id_type slot_id, void_function callback )
{
  tweeners_debug_system_invariant();

  tweeners_confirm_contract
    ( is_valid_slot_id( slot_id ),
      "system::on_slot_start(): slot does not exist." );

  m_start_functions.emplace( slot_id, std::move( callback ) );
}

/**
 * \brief Assign a function to be called when the given slot stops running.
 *
 * \param slot_id The identifier of the slot to link with the function.
 * \param callback The function to call when the slot's running time is over.
 *
 * The function will be called the next and every time the slot reach its end.
 *
 * See \sa configure_slot to create a slot_id.
 */
template< typename Config >
void tweeners::system_base< Config >::on_slot_done
( id_type slot_id, void_function callback )
{
  tweeners_debug_system_invariant();

  tweeners_confirm_contract
    ( is_valid_slot_id( slot_id ),
      "system::on_slot_done(): slot does not exist." );
    
  m_done_functions.emplace( slot_id, std::move( callback ) );
}

/**
 * \brief Schedule a slot to be started when a given slot is done.
 *
 * \param first The identifier of the first slot to run.
 * \param second The identifier of the slot to run when the first is completed.
 *
 * Each slot can have an arbitrary number of successors but a single
 * predecessor. Creating loops with this function is allowed and work as
 * expected. By definition no slot from the loop can have a predecessor that
 * is not in the loop, which means that a sequence 1, 2, 3, 2, 3… is not
 * feasible.
 *
 * The behavior when there are several actively running slots in a sequence is
 * undefined. If it works, be sure that this is unintentional.
 *
 * \sa start_slot to start the update of a given slot.
 */
template< typename Config >
void tweeners::system_base< Config >::play_in_sequence
( id_type first, id_type second )
{
  tweeners_debug_system_invariant();

  tweeners_confirm_contract
    ( m_slot[ second ].previous == not_an_id,
      "system::play_in_sequence(): second slot is already in a sequence." );
  tweeners_confirm_contract
    ( is_valid_slot_id( first ),
      "system::play_in_sequence(): first slot does not exist." );
  tweeners_confirm_contract
    ( is_valid_slot_id( second ),
      "system::play_in_sequence(): second slot does not exist." );

  if ( !m_successors.has_value( first ) )
    m_successors.emplace( first );

  m_successors.get_existing( first ).emplace_back( second );
  m_slot[ second ].previous = first;
}

/**
 * \brief Completely remove a slot from the system.
 *
 * \param slot_id The slot to remove.
 *
 * Removing a slot can be done at any moment, even during its start/update/done
 * callbacks and is immediately effective. The data associated with the slot
 * will actually be removed during a subsequent update. Please be aware of this
 * if you have bound something in the callback's arguments.
 *
 * Neither the predecessor or the successors of the removed slot are removed by
 * this call. The successors not started yet won't start unless you call
 * start_slot() on them.
 */
template< typename Config >
void tweeners::system_base< Config >::remove_slot( id_type slot_id )
{
  tweeners_debug_system_invariant();

  tweeners_confirm_contract
    ( is_valid_slot_id( slot_id ),
      "system::remove_slot(): slot does not exist." );

  m_slot_states[ slot_id ] = slot_state::dead;
  m_dead_queue.emplace_back( slot_id );
}

/**
 * \brief Update the state of the system by moving the time forward for a given
 *        duration.
 *
 * \param step The elapsed duration since the last update.
 *
 * The update starts all slots that have been scheduled by the calls to
 * start_slots() since the previous update. Each running slot then moves forward
 * the given duration, triggering the update callback passed to
 * configure_slot(). If a slot reaches its end, its successors start and are
 * updated by the remaining amount of time not consumed by the slot. The done
 * and start callbacks are executed accordingly.
 *
 * The tweeners existing in the system are considered late by the given duration
 * and are updated accordingly. Tweeners inserted during the update (e.g. in a
 * callback of an existing tweener) are not started during this update and are
 * postponed until the next.
 *
 * There is no guarantee on the order in which the slots are processed
 * relatively to each other.
 */
template< typename Config >
void tweeners::system_base< Config >::update( duration_type step )
{
  // update_running_slots() may start new slots in sequences. These slots,
  // and only them, must be updated during the same update call. Slots created
  // in the callbacks of start_queued_slots() and stop_completed_slots() will
  // wait until the next update.

  tweeners_debug_system_invariant();

  remove_dead_slots();
  
  start_slots( m_start_queue );
  update_current_date( step );

  bool done( false );
  std::size_t update_from( 0 );

  while( !done )
  {
    update_running_slots( update_from );
    stop_completed_slots();

    if ( m_sequence_queue.empty() )
      done = true;
    else
      {
        update_from = m_need_update.size();
        start_slots( m_sequence_queue );
      }
  }
}

/**
 * \brief Find or create a slot id available to build a new slot.
 *
 * No matter if the returned slot is recycled or freshly created, its components
 * are the same.
 */
template< typename Config >
typename tweeners::system_base< Config >::id_type
tweeners::system_base< Config >::create_slot()
{
  tweeners_debug_system_invariant();

  id_type result;
  
  if ( m_available_ids.empty() )
    {
      result = m_slot.size();
      m_slot.emplace_back();
      m_slot_states.emplace_back( slot_state::available );
      m_current_dates.emplace_back();
      m_start_functions.add_one_slot_at_end();
      m_done_functions.add_one_slot_at_end();
      m_successors.add_one_slot_at_end();
    }
  else
    {
      result = m_available_ids.back();
      m_available_ids.pop_back();
    }
  
  tweeners_debug_assert( result < m_slot.size() );
  tweeners_debug_assert( result < m_slot_states.size() );
  tweeners_debug_assert( m_slot_states[ result ] == slot_state::available );
  tweeners_debug_assert( result < m_current_dates.size() );
  tweeners_debug_assert( !m_start_functions.has_value( result ) );
  tweeners_debug_assert( !m_done_functions.has_value( result ) );
  tweeners_debug_assert( !m_successors.has_value( result ) );

  return result;
}

/**
 * \brief Tells if the given slot id is an acceptable value for input in the
 *        public interface.
 *
 * The intent of this function is to validate the inputs of start_slot() and
 * similar functions. For internal debugging, use the
 * tweeners_debug_validate_id() macro.
 */
template< typename Config >
bool tweeners::system_base< Config >::is_valid_slot_id( id_type slot_id ) const
{
  tweeners_debug_system_invariant();

  return ( slot_id != not_an_id )
    && ( slot_id >= 0 )
    && ( static_cast< std::size_t >( slot_id ) < m_slot_states.size() )
    && ( m_slot_states[ slot_id ] != slot_state::available );
}

template< typename Config >
void tweeners::system_base< Config >::update_current_date( duration_type step )
{
  tweeners_debug_system_invariant();

  for ( id_type slot_id : m_need_update )
    m_current_dates[ slot_id ] += step;
}

template< typename Config >
void
tweeners::system_base< Config >::start_slots( std::vector< id_type >& queue )
{
  tweeners_debug_system_invariant();

  std::vector< id_type > slots;
  slots.swap( queue );

  const auto begin( slots.begin() );
  auto end( slots.end() );
  
  m_need_update.insert( m_need_update.end(), begin, end );

  for ( auto it( begin ); it != end; )
    {
      tweeners_debug_validate_id( *it );

      slot_state& state( m_slot_states[ *it ] );
      tweeners_debug_assert
        ( ( state == slot_state::ready ) || ( state == slot_state::dead ) );

      // slot_id may represent a dead tweener in the following situation:
      // 1. slot_id is the successor of a slot S,
      // 2. and S is done in the current update,
      // 3. and slot_id is removed in S.on_done.
      //
      // By 2. m_sequence_queue contains slot_id and by 3. slot_id is flagged
      // as dead.
      //
      // start_slots( m_sequence_queue ) is called in the above state.
      if ( state == slot_state::ready )
        {
          state = slot_state::running;
          ++it;
        }
      else
        {
          --end;
          *it = *end;
        }
    }

  for ( auto it( begin ); it != end; ++it )
    m_start_functions[ *it ]();
}

template< typename Config >
void tweeners::system_base< Config >::update_running_slots( std::size_t from )
{
  tweeners_debug_system_invariant();

  const std::size_t end( m_need_update.size() );
  tweeners_debug_assert( from <= end );
  
  for ( std::size_t i( from ); i != end; ++i )
    {
      const id_type slot_id( m_need_update[ i ] );
      const slot_state state( m_slot_states[ slot_id ] );
      
      // An update may trigger a call to remove_slot which will flag the
      // scheduled slot as dead, in which case we do not want to update it.

      tweeners_debug_assert
        ( ( state == slot_state::running ) || ( state == slot_state::dead ) );
      
      if ( state == slot_state::running )
        update_tweener( slot_id );
    }
}

template< typename Config >
void tweeners::system_base< Config >::update_tweener( id_type slot_id )
{
  tweeners_debug_system_invariant();

  tweeners_debug_validate_id( slot_id );
  tweeners_debug_assert( m_slot_states[ slot_id ] == slot_state::running );
  
  tweener_state& tweener( m_slot[ slot_id ] );
  const duration_type end_date( tweener.end_date );
  const duration_type current_date( m_current_dates[ slot_id ] );
  float_type date_ratio;

  if ( current_date >= end_date )
    {
      complete_slot( slot_id, current_date - end_date );
      date_ratio = tweener.transform( 1 );
    }
  else
    date_ratio =
      tweener.transform
      ( detail::to_float< float_type >( current_date )
        / detail::to_float< float_type >( end_date ) );
  
  tweener.on_update( date_ratio );
}

/**
 * \brief Flag the given slot as done and schedule its successors for the
 *        update.
 *
 * \param slot_id The completed slot.
 *
 * \param successors_current_date How much extra time has been received in the
 *        update of slot_id (i.e. the date to assign to m_current_dates[ s ] for
 *        each sucessor of slot_id).
 */        
template< typename Config >
void tweeners::system_base< Config >::complete_slot
( id_type slot_id, duration_type successors_current_date )
{
  m_done_queue.emplace_back( slot_id );
  m_slot_states[ slot_id ] = slot_state::done;

  const successor_vector& successors( m_successors[ slot_id ] );

  for ( id_type next : successors )
    {
      (void)next;
      tweeners_debug_assert
        ( m_slot_states[ next ] != slot_state::available );
      tweeners_debug_assert( m_slot_states[ next ] != slot_state::running );
    }
  
  m_sequence_queue.insert
    ( m_sequence_queue.end(), successors.begin(), successors.end() );
  
  for ( id_type next : successors )
      m_current_dates[ next ] = successors_current_date;
}

template< typename Config >
void tweeners::system_base< Config >::stop_completed_slots()
{
  tweeners_debug_system_invariant();

  std::vector< id_type > slots;
  slots.swap( m_done_queue );

  const auto begin( slots.begin() );
  auto end( slots.end() );

  std::sort( begin, end );
  remove_ids( m_need_update, begin, end );

  for ( auto it( begin ); it != end; )
    {
      slot_state& state( m_slot_states[ *it ] );

      if ( state == slot_state::dead )
        {
          --end;
          *it = *end;
        }
      else
        {
          state = slot_state::ready;
          ++it;
        }
    }

  for ( auto it( begin ); it != end; ++it )
    m_done_functions[ *it ]();
}

template< typename Config >
void tweeners::system_base< Config >::remove_dead_slots()
{
  tweeners_debug_system_invariant();

  const auto begin( m_dead_queue.begin() );
  const auto end( m_dead_queue.end() );

  std::sort( begin, end );
  
  for ( auto it( begin ); it != end; ++it )
    {
      const id_type slot_id( *it );
      
      tweener_state& tweener( m_slot[ slot_id ] );
      tweener.transform = decltype( tweener.transform )();
      tweener.on_update = decltype( tweener.on_update )();

      if ( tweener.previous != not_an_id )
        remove_from_predecessor_successors( tweener.previous, slot_id );
    }

  for ( auto it( begin ); it != end; ++it )
    m_slot_states[ *it ] = slot_state::available;

  for ( auto it( begin ); it != end; ++it )
    for ( id_type successor : m_successors[ *it ] )
      m_slot[ successor ].previous = not_an_id;
  
  m_start_functions.erase( begin, end );
  m_done_functions.erase( begin, end );
  m_successors.erase( begin, end );

  remove_ids( m_start_queue, begin, end );
  remove_ids( m_done_queue, begin, end );
  remove_ids( m_sequence_queue, begin, end );
  remove_ids( m_need_update, begin, end );

  m_available_ids.insert( m_available_ids.end(), begin, end );
  m_dead_queue.clear();
}

template< typename Config >
void tweeners::system_base< Config >::remove_from_predecessor_successors
( id_type predecessor_id, id_type successor_id )
{
  tweeners_debug_system_invariant();

  tweeners_debug_validate_id( predecessor_id );
  tweeners_debug_validate_id( successor_id );

  successor_vector& successors( m_successors.get_existing( predecessor_id ) );
  tweeners_debug_assert( !successors.empty() );
  
  const auto begin( successors.begin() );
  auto end( successors.end() );
  --end;

  const auto it( std::find( begin, end, successor_id ) );
  tweeners_debug_assert( ( it != end ) || ( *end == successor_id ) );
  
  *it = *end;
  successors.pop_back();
}

template< typename Config >
void tweeners::system_base< Config >::remove_ids
( std::vector< id_type >& ids, id_iterator first,
  const id_iterator& last ) const
{
  tweeners_debug_system_invariant();

  if ( first == last )
    return;
  
  tweeners_debug_assert( std::is_sorted( first, last ) );

  const auto ids_begin( ids.begin() );
  const auto ids_end( ids.end() );

  auto last_iterator( first );
  id_type last_id( *last_iterator );

  // Since the range [first, last) is sorted, we can remember where was the last
  // searched value and search the new one before of after in the range,
  // according to how they compare.
  auto predicate
    ( [ &first, &last, &last_id, &last_iterator ]( id_type slot_id ) -> bool
      {
        bool out_of_range;
        
        if ( slot_id >= last_id )
          {
            last_iterator = std::lower_bound( last_iterator, last, slot_id );
            out_of_range = last_iterator == last;
          }
        else
          {
            last_iterator = std::lower_bound( first, last_iterator, slot_id );
            out_of_range = false;
          }
        
        last_id = slot_id;

        return !out_of_range && ( *last_iterator == slot_id );
      } );
  
  ids.erase( std::remove_if( ids_begin, ids_end, predicate ), ids_end );
}
  
template< typename Config >
void tweeners::system_base< Config >::check_invariants() const
{
  check_update_queue_invariants();
  check_sequences_invariants();
  check_available_ids();
}

/**
 * \brief Validate the slots scheduled for an update (i.e. the content of
 * m_need_update).
 *
 * - The slots scheduled for an update must be valid.
 * - A slot scheduled for an update must be scheduled only once.
 */
template< typename Config >
void tweeners::system_base< Config >::check_update_queue_invariants() const
{
  const auto update_begin( m_need_update.begin() );
  const auto update_end( m_need_update.end() );

  for ( id_type slot_id : m_need_update )
    {
      tweeners_debug_validate_id( slot_id );

      tweeners_debug_assert
        ( std::count( update_begin, update_end, slot_id ) == 1 );
    }
}

/**
 * \brief Validate the consistency of m_successors and tweener::previous.
 *
 * Slots in the `available` or `dead` state are in the process of being
 * recycled, thus we accept that they do not match the requirements here.
 *
 * - If a slot B is in the successors of A then A is the predecessor of B.
 * - If a slot A is the predecessor of B then B is a successor of A.
 */
template< typename Config >
void tweeners::system_base< Config >::check_sequences_invariants() const
{
  const std::size_t allocated_slot_count( m_slot_states.size() );

  for ( std::size_t slot_id( 0 ); slot_id != allocated_slot_count; ++slot_id )
    if ( ( m_slot_states[ slot_id ] != slot_state::available )
         && ( m_slot_states[ slot_id ] != slot_state::dead ) )
      {
        for ( id_type successor : m_successors[ slot_id ] )
          tweeners_debug_assert( m_slot[ successor ].previous == slot_id );

        const id_type previous( m_slot[ slot_id ].previous );

        if ( previous != not_an_id )
          {
            const auto& previous_successors( m_successors[ previous ] );
            const auto begin( previous_successors.begin() );
            const auto end( previous_successors.end() );
            
            tweeners_debug_assert( std::find( begin, end, slot_id ) != end );
          }
      }
}

/**
 * \brief Validate that the ids in m_available_ids are not used anywhere else.
 */
template< typename Config >
void tweeners::system_base< Config >::check_available_ids() const
{
  for ( id_type slot_id : m_available_ids )
    {
      tweeners_debug_validate_id( slot_id );
      
      tweeners_debug_not_in_container( slot_id, m_need_update );
      tweeners_debug_not_in_container( slot_id, m_start_queue );
      tweeners_debug_not_in_container( slot_id, m_done_queue );
      tweeners_debug_not_in_container( slot_id, m_dead_queue );
      tweeners_debug_not_in_container( slot_id, m_sequence_queue );

      tweeners_debug_assert( slot_id < m_slot.size() );
      tweeners_debug_assert( slot_id < m_slot_states.size() );
      tweeners_debug_assert
        ( m_slot_states[ slot_id ] == slot_state::available );
      tweeners_debug_assert( slot_id < m_current_dates.size() );
      tweeners_debug_assert( !m_start_functions.has_value( slot_id ) );
      tweeners_debug_assert( !m_done_functions.has_value( slot_id ) );
      tweeners_debug_assert( !m_successors.has_value( slot_id ) );
    }

  const std::size_t allocated_slot_count( m_slot_states.size() );

  for ( std::size_t slot_id( 0 ); slot_id != allocated_slot_count; ++slot_id )
    if ( ( m_slot_states[ slot_id ] != slot_state::available )
         && ( m_slot_states[ slot_id ] != slot_state::dead )
         && ( m_slot[ slot_id ].previous != not_an_id ) )
      tweeners_debug_not_in_container
        ( m_slot[ slot_id ].previous, m_available_ids );
}

#undef tweeners_debug_not_in_container
#undef tweeners_debug_system_invariant
#undef tweeners_debug_validate_id

#endif

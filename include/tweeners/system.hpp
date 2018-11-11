#ifndef TWEENERS_SYSTEM_HPP
#define TWEENERS_SYSTEM_HPP

#include <tweeners/config.hpp>
#include <tweeners/detail/slot_component.hpp>

#include <vector>

namespace tweeners
{
  /**
   * \brief The system handles the progression of the tweeners.
   *
   * A tweener is basically the process of updating a value in a range following
   * a predefined curve over time.
   *
   * The updates on the value can be either assigned to a predefined variable
   * or retrieved via an update callback.
   *
   * Tweeners can be played either simultaneously or in sequence. Also, a
   * callback can optionally be attached to a tweener for being notified when
   * the tweener starts and ends.
   */
  template< typename Config = config<> >
  class system_base
  {
  public:
    using duration_type = typename Config::duration_type;
    using id_type = typename Config::id_type;
    using float_type = typename Config::float_type;

    template< typename Signature >
    using function_type = typename Config::template function_type< Signature >;

    using update_function = function_type< void( float_type ) >;
    using transform_function = function_type< float_type( float_type ) >;

    using void_function = function_type< void() >;

  public:
    system_base();

    void reserve
    ( std::size_t slot_count, std::size_t value_count_per_component,
      std::size_t simultaneous_count );
    
    id_type configure_slot
    ( duration_type duration, update_function update,
      transform_function transform );

    void start_slot( id_type slot_id );

    void on_slot_start( id_type slot_id, void_function callback );
    void on_slot_done( id_type slot_id, void_function callback );

    void play_in_sequence( id_type first, id_type second );
    
    void remove_slot( id_type slot_id );

    void update( duration_type step );

  public:
    static constexpr const id_type not_an_id = -1;

  private:
    enum class slot_state : char
      {
        ready,
        running,
        done,
        dead,
        available
      };

    struct tweener_state
    {
      duration_type end_date;

      id_type previous;
      
      transform_function transform;
      function_type< void( float_type ) > on_update;
    };

    typedef std::vector< id_type > successor_vector;
    
    using id_iterator = typename std::vector< id_type >::iterator;
    
  private:
    id_type create_slot();
    bool is_valid_slot_id( id_type slot_id ) const;
    
    void update_current_date( duration_type step );
    void start_slots( std::vector< id_type >& queue );
    void update_running_slots( std::size_t from );
    void update_tweener( id_type slot_id );
    void complete_slot
    ( id_type slot_id, duration_type successors_current_date );
    void stop_completed_slots();

    void remove_dead_slots();
    void remove_from_predecessor_successors
    ( id_type predecessor_id, id_type successor_id );

    void remove_ids
    ( std::vector< id_type >& ids, id_iterator first,
      const id_iterator& last ) const;

    void check_invariants() const;
    void check_update_queue_invariants() const;
    void check_sequences_invariants() const;
    void check_available_ids() const;
    
  private:
    /**
     * \brief Slots scheduled to start during the next update.
     *
     * \sa start_slot.
     */
    std::vector< id_type > m_start_queue;

    /** \brief Slots completed during the current update. */
    std::vector< id_type > m_done_queue;

    /**
     * \brief Slots scheduled to be removed during the next update.
     *
     * \sa remove_slot.
     */
    std::vector< id_type > m_dead_queue;

    /**
     * \brief Slots that must be started during the current update.
     *
     * They are successors of a slot that have completed its time during the
     * current update.
     */
    std::vector< id_type > m_sequence_queue;
    
    /**
     * \brief Ids that can be used for the upcoming slot creation.
     *
     * A slot id is a valid direct index in m_slot_states, m_slot,
     * m_current_dates.
     */
    std::vector< id_type > m_available_ids;

    /**
     * \name Slot properties
     *
     * These properties can be seen as member variables of a slot. The are
     * grouped by type in order to efficiently iterate on the same property of
     * all slots.
     */
    ///@{

    std::vector< slot_state > m_slot_states;
    std::vector< tweener_state > m_slot;
    std::vector< duration_type > m_current_dates;

    detail::slot_component< void_function, id_type > m_start_functions;
    detail::slot_component< void_function, id_type > m_done_functions;
    detail::slot_component< successor_vector, id_type > m_successors;

    ///@}

    /** \brief Slots that will be updated in the next update. */
    std::vector< id_type > m_need_update;
  };

  using system = system_base<>;
}

#include <tweeners/detail/system.tpp>

#endif

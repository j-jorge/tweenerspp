#ifndef TWEENERS_DETAIL_SLOT_COMPONENT_HPP
#define TWEENERS_DETAIL_SLOT_COMPONENT_HPP

#include <vector>

namespace tweeners
{
  namespace detail
  {
    /**
     * \brief Associative container with fallback value for non existent keys.
     *
     * The intent of this container is to associate an optional value to a
     * tweener slot in tweeners::system and store them in a cache-friendly way
     * for efficient iteration.
     *
     * Instances of this container is supposed to contain as many entries
     * (created with add_one_slot_at_end()) than there are slots in
     * tweeners::system.
     *
     * The slots whose component are stored in slot_component are expected to
     * be identified continuously from zero to n.
     */
    template< typename T, typename Id >
    class slot_component
    {
    public:
      explicit slot_component( T default_value );

      void reserve( std::size_t slot_count, std::size_t value_count );

      void add_one_slot_at_end();
    
      template< typename... Args >
      void emplace( Id slot_id, Args&&... value );
    
      bool has_value( Id slot_id ) const;
      T& get_existing( Id slot_id );

      const T& operator[]( Id i ) const;

      template< typename Iterator >
      void erase( Iterator first, Iterator last );

    private:
      void check_invariants() const;
      
    private:
      /**
       * \brief The values asosciated with the slots.
       *
       * m_values[ 0 ] is the default value.
       */
      std::vector< T > m_values;

      /**
       * \brief The index in m_values of the value associated with each slot.
       *
       * The slots having no associated value are assigned the index 0.
       */
      std::vector< std::size_t > m_value_index_from_slot;

      /**
       * \brief Reverse lookup for m_value_index_from_slot.
       *
       * Given i, the entry m_slot_from_value_index[ i ] is the index in
       * m_value_index_from_slot to which is associated the value m_values[ i ].
       */
      std::vector< std::size_t > m_slot_from_value_index;
    };
  }
}

#include <tweeners/detail/slot_component.tpp>

#endif

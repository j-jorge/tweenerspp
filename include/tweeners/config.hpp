#ifndef TWEENERS_CONFIG_HPP
#define TWEENERS_CONFIG_HPP

#include <functional>

namespace tweeners
{
  /**
   * \brief Customizable types in the tweeners system.
   */
  template
  <
    typename Duration = float,
    typename Id = int,
    typename Float = float,
    template< typename Signature > class Function = std::function
  >
  struct config
  {
    /**
     * \brief The type used to store the durations in the tweeners.
     *
     * It must be convertible to float_type or std::chrono::duration.
     */
    using duration_type = Duration;

    /**
     * \brief The type used to identify a slot/tweener.
     *
     * It must be an integral type since it is used to index vectors.
     */
    using id_type = int;

    /**
     * \brief The default float-like type, used to pass the interpolated values
     *        in the tweeners' update callbacks.
     */
    using float_type = float;

    /**
     * \brief The type used to store a callable object with a signature S.
     */
    template< typename S >
    using function_type = Function< S >;
  };
}

#endif

#ifndef TWEENERS_CONTRACT_HPP
#define TWEENERS_CONTRACT_HPP

/** \file */

/**
 * \def tweeners_confirm_contract( e, m )
 *
 * \brief Customization point to validate the preconditions in the library.
 *
 * This macro is called to validate the user-provided inputs of the functions at
 * various places in the library. By default an exception is thrown if the
 * provided condition is not verified. Feel free to redefine this macro for
 * something that fits your needs better.
 *
 * \param e An expression that must evaluate to true.
 *
 * \param m The message to display if e is false.
 */ 
#ifndef tweeners_confirm_contract
#include <stdexcept>
#define tweeners_confirm_contract( e, m )       \
  do                                            \
    {                                           \
      if ( !(e) )                               \
        throw std::runtime_error( m );          \
    }                                           \
  while( false )
#endif

#endif

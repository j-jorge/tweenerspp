cmake_minimum_required( VERSION 3.10 )

project( tweeners )

set( source_root "${CMAKE_CURRENT_SOURCE_DIR}/.." )
set( CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/modules" )
set( CMAKE_CXX_STANDARD 11 )

include( GNUInstallDirs )
include( unity-build )
include( ExternalProject )
include( ProcessorCount )

option( TWEENERS_TESTING_ENABLED "Build the unit tests." ON )
option( TWEENERS_BENCHMARKING_ENABLED "Build the benchmarks." OFF )
option( TWEENERS_DEMO_ENABLED "Build the demo." OFF )
option( TWEENERS_CMAKE_PACKAGE_ENABLED "Build the CMake package." ON )
option( TWEENERS_DEBUG "Enable internal debug." OFF )

add_subdirectory( "products/core/" )

if( TWEENERS_TESTING_ENABLED )
  enable_testing()
  find_package( GTest REQUIRED )
  add_subdirectory( "products/tests/" )
endif()

if ( TWEENERS_BENCHMARKING_ENABLED )
  include( ExternalProject )
  find_package( Boost )
  add_subdirectory( "products/benchmarks/" )
endif()

if ( TWEENERS_DEMO_ENABLED )
  find_package( SDL2 REQUIRED )
  add_subdirectory( "products/demo/" )
endif()

if( TWEENERS_CMAKE_PACKAGE_ENABLED )
  add_subdirectory( "products/package/" )
endif()

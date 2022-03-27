ProcessorCount( processor_count )

if( NOT processor_count )
  set( processor_count 1 )
endif()

# -- Claw --

# The benchmark includes a measure of libclaw's tweeners. Since the library
# requires boost, we skip it if Boost is not available.

if( Boost_FOUND )
  set(claw_tween_library_path
    "${CMAKE_CURRENT_BINARY_DIR}/claw-prefix/src/claw-build/libclaw_tween${CMAKE_STATIC_LIBRARY_SUFFIX}")

  ExternalProject_Add(
    claw
    GIT_REPOSITORY "https://github.com/j-jorge/libclaw.git"
    GIT_TAG aaa735f48cb2e1212c3f5d1bf4e13b0879887384
    GIT_PROGRESS 1
    STEP_TARGETS build
    EXCLUDE_FROM_ALL 1
    BUILD_BYPRODUCTS ${claw_tween_library_path}
    CMAKE_ARGS
      -DCLAW_NO_JPEG=1
      -DCLAW_NO_PNG=1
      -DCLAW_INSTALL_CMAKE_MODULES=0
      -DCMAKE_BUILD_TYPE=release
      -DBOOST_ROOT="${BOOST_ROOT}"
    BUILD_COMMAND
      ${CMAKE_COMMAND} --build . --target claw_tween-static
      -- -j${processor_count}
    )

  set(
    optional_libraries
    ${optional_libraries}
    "${claw_tween_library_path}"
    )
  set(
    optional_include_directories
    ${optional_include_directories}
    "${CMAKE_CURRENT_BINARY_DIR}/claw-prefix/src/claw/"
    )
  set( optional_dependencies ${optional_dependencies} claw-build )
  set( optional_sources ${optional_sources} "claw.cpp" )
endif()


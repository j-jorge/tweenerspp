# -- Cpptweener --
# Maybe the oldest well known implementation of a tweener framework in C++.

set(cpptweener_library_path
  "${CMAKE_CURRENT_BINARY_DIR}/cpptweener-prefix/src/cpptweener-build/libcpptweener${CMAKE_STATIC_LIBRARY_SUFFIX}")

ExternalProject_Add(
  cpptweener
  GIT_REPOSITORY "https://github.com/devmario/CppTweener.git"
  GIT_TAG 88b70d2dc722b569dd8219c9aa17956631832a64
  GIT_PROGRESS 1
  GIT_SHALLOW 1
  STEP_TARGETS build
  EXCLUDE_FROM_ALL 1
  BUILD_BYPRODUCTS ${cpptweener_library_path}
  INSTALL_COMMAND ""
  CMAKE_ARGS
    -DCMAKE_BUILD_TYPE=release
  PATCH_COMMAND
    ${CMAKE_COMMAND} -E copy
    "${CMAKE_CURRENT_LIST_DIR}/CMakeLists-cpptweener.txt"
    "./CMakeLists.txt"
  )

set(
  optional_libraries
  ${optional_libraries}
  "${cpptweener_library_path}"
  )
set(
  optional_include_directories
  ${optional_include_directories}
  "${CMAKE_CURRENT_BINARY_DIR}/cpptweener-prefix/src/cpptweener/"
  )
set( optional_dependencies ${optional_dependencies} cpptweener-build )
set( optional_sources ${optional_sources} "cpptweener.cpp" )

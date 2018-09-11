set( benchmarks_executable_name benchmarks )

include( "claw.cmake" )
include( "cpptweener.cmake" )

add_unity_build_executable(
  TARGET ${benchmarks_executable_name}
  ROOT "${source_root}/benchmarks/src/"
  FILES
  "benchmark_registry.cpp"
  "main.cpp"
  "options.cpp"
  "self.cpp"
  ${optional_sources}
  )

add_dependencies(
  ${benchmarks_executable_name}
  ${optional_dependencies}
  )

set_target_properties(
  ${benchmarks_executable_name}
  PROPERTIES
  CXX_STANDARD 17
  )

target_link_libraries(
  ${benchmarks_executable_name}
  ${core_library_name}
  ${optional_libraries}
  )

target_include_directories(
  ${benchmarks_executable_name}
  PRIVATE
  ${source_root}/benchmarks/include
  ${optional_include_directories}
  )

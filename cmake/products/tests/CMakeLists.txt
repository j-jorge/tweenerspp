set( unit_tests_executable_name ${core_library_name}-tests )

add_unity_build_executable(
  TARGET ${unit_tests_executable_name}
  ROOT "${source_root}/tests/src/"
  FILES
  "complex_value.cpp"
  "custom_config.cpp"
  "loop.cpp"
  "on_start_on_done.cpp"
  "remove.cpp"
  "remove_next_from_sequence.cpp"
  "remove_predecessor_from_sequence.cpp"
  "remove_sibling_from_sequence.cpp"
  "start_twice.cpp"
  "start_update.cpp"
  "sequence.cpp"
  "slot_component.cpp"
  "test_helper.cpp"
  "tweener_tracker.cpp"
  "zero_duration.cpp"
  )

target_include_directories(
  ${unit_tests_executable_name}
  PRIVATE
  ${source_root}/tests/include
  )

target_link_libraries(
  ${unit_tests_executable_name}
  ${core_library_name}
  GTest::GTest
  GTest::Main
  )

gtest_discover_tests( ${unit_tests_executable_name} )

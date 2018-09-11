set( demo_executable_name demo )

add_executable(
  ${demo_executable_name}
  "${source_root}/demo/main.cpp"
  )

set_target_properties(
  ${demo_executable_name}
  PROPERTIES
  CXX_STANDARD 17
  )

target_link_libraries(
  ${demo_executable_name}
  ${core_library_name}
  ${SDL2_LIBRARIES}
  )

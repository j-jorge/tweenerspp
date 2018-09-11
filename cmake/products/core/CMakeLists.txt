set( core_library_name tweeners )
set( core_library_name ${core_library_name} PARENT_SCOPE )

add_library( ${core_library_name} INTERFACE )
  
target_include_directories(
  ${core_library_name}
  INTERFACE
  $<BUILD_INTERFACE:${source_root}/include>
  )

install(
  DIRECTORY ${source_root}/include/tweeners
  DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
  )

install(
  TARGETS ${core_library_name}
  EXPORT ${core_library_name}
  )

if( TWEENERS_DEBUG )
  target_compile_definitions( ${core_library_name} INTERFACE TWEENERS_DEBUG )
endif()

set(CXX_SOURCES)
auto_sources(files "*.cpp" "")
list(APPEND CXX_SOURCES ${files})

add_executable(json_export ${CXX_SOURCES})
target_link_libraries(json_export ${HHVM_LINK_LIBRARIES})
embed_systemlib(json_export "${CMAKE_CURRENT_SOURCE_DIR}/json_export" ${HPHP_HOME}/bin/systemlib.php)
add_dependencies(json_export systemlib)

#install(TARGETS hhvm DESTINATION bin)

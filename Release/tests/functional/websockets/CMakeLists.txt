if (NOT CPPREST_EXCLUDE_WEBSOCKETS)

  include_directories(utilities/include)
  
  set(SOURCES
    utilities/test_websocket_server.cpp
  )
  
  add_library(websockettest_utilities ${TEST_LIBRARY_TARGET_TYPE} ${SOURCES})
  target_compile_definitions(websockettest_utilities PRIVATE -DWEBSOCKETTESTUTILITY_EXPORTS)
  if (NOT TEST_LIBRARY_TARGET_TYPE STREQUAL "OBJECT")
    target_link_libraries(websockettest_utilities PRIVATE
      unittestpp
      common_utilities
      ${BOOST_LIBRARIES}
      ${Casablanca_LIBRARIES}
    )
  endif()

  if(CPPREST_WEBSOCKETS_IMPL STREQUAL "wspp")
    cpprest_find_boost()
    cpprest_find_zlib()
    cpprest_find_openssl()
    cpprest_find_websocketpp()

    target_include_directories(websockettest_utilities SYSTEM PRIVATE ${Boost_INCLUDE_DIR} ${OPENSSL_INCLUDE_DIR} ${WEBSOCKETPP_INCLUDE_DIR} ${ZLIB_INCLUDE_DIR})
    target_link_libraries(websockettest_utilities PRIVATE ${Boost_LIBRARIES} ${OPENSSL_LIBRARIES} ${ZLIB_LIBRARIES})
  endif()

  add_subdirectory(client)
endif()

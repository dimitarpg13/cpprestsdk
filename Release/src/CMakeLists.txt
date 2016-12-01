# Note: we are only listing a source file to suppress a CMake warning.
# The bulk of the sources will be incrementally added via `target_sources`.
add_library(cpprest http/client/http_client.cpp)

target_include_directories(cpprest
  PUBLIC
    $<INSTALL_INTERFACE:include> $<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/include>
  PRIVATE
    pch
)

if(CMAKE_CXX_COMPILER_ID MATCHES "Clang" OR CMAKE_CXX_COMPILER_ID MATCHES "GNU")
  if(WERROR)
    target_compile_definitions(cpprest PRIVATE -Werror)
  endif()
  target_compile_definitions(cpprest PRIVATE -pedantic)
elseif(CMAKE_CXX_COMPILER_ID MATCHES "MSVC")
  if(WERROR)
    target_compile_definitions(cpprest PRIVATE /WX)
  endif()
endif()

if (BUILD_SHARED_LIBS AND WIN32)
  target_compile_definitions(cpprest PRIVATE -D_ASYNCRT_EXPORT -D_PPLX_EXPORT -D_USRDLL)
endif()

target_compile_definitions(cpprest PRIVATE ${WARNINGS})

file(GLOB HEADERS_CPPREST "../include/cpprest/*.h" "../include/cpprest/*.hpp" "../include/cpprest/*.dat")
file(GLOB HEADERS_PPLX "../include/pplx/*.h" "../include/pplx/*.hpp")
file(GLOB HEADERS_DETAILS "../include/cpprest/details/*.h" "../include/pplx/*.hpp" "../include/pplx/*.dat")
source_group("Header Files\\cpprest" FILES ${HEADERS_CPPREST})
source_group("Header Files\\pplx" FILES ${HEADERS_PPLX})
source_group("Header Files\\cpprest\\details" FILES ${HEADERS_DETAILS})

if(CPPREST_INSTALL_HEADERS)
  install(FILES ${HEADERS_CPPREST} DESTINATION include/cpprest)
  install(FILES ${HEADERS_PPLX} DESTINATION include/pplx)
  install(FILES ${HEADERS_DETAILS} DESTINATION include/cpprest/details)
endif()

target_sources(cpprest PRIVATE
  ${HEADERS_CPPREST}
  ${HEADERS_PPLX}
  ${HEADERS_DETAILS}
  http/client/http_client_msg.cpp
  http/client/http_client_impl.h
  http/client/x509_cert_utilities.cpp
  http/common/http_helpers.cpp
  http/common/http_msg.cpp
  http/listener/http_listener.cpp
  http/listener/http_listener_msg.cpp
  http/listener/http_server_api.cpp
  http/oauth/oauth1.cpp
  http/oauth/oauth2.cpp
  json/json.cpp
  json/json_parsing.cpp
  json/json_serialization.cpp
  pplx/pplx.cpp
  uri/uri.cpp
  uri/uri_builder.cpp
  uri/uri_parser.cpp
  utilities/asyncrt_utils.cpp
  utilities/base64.cpp
  utilities/web_utilities.cpp
)

# Websockets component
if(CPPREST_WEBSOCKETS_IMPL STREQUAL "none")
  target_compile_definitions(cpprest PUBLIC -DCPPREST_EXCLUDE_WEBSOCKETS=1)
elseif(CPPREST_WEBSOCKETS_IMPL STREQUAL "winrt")
  target_sources(cpprest PRIVATE
    websockets/client/ws_msg.cpp
    websockets/client/ws_client.cpp
    websockets/client/ws_client_winrt.cpp
  )
elseif(CPPREST_WEBSOCKETS_IMPL STREQUAL "wspp")
  cpprest_find_zlib()
  cpprest_find_openssl()
  cpprest_find_boost()
  cpprest_find_websocketpp()

  target_include_directories(cpprest SYSTEM PRIVATE ${Boost_INCLUDE_DIR} ${OPENSSL_INCLUDE_DIR} ${WEBSOCKETPP_INCLUDE_DIR} ${ZLIB_INCLUDE_DIR})
  target_link_libraries(cpprest PRIVATE ${Boost_LIBRARIES} ${OPENSSL_LIBRARIES} ${ZLIB_LIBRARIES})

  target_sources(cpprest PRIVATE
    websockets/client/ws_msg.cpp
    websockets/client/ws_client.cpp
    websockets/client/ws_client_wspp.cpp
  )
else()
  message(FATAL_ERROR "Invalid implementation")
endif()

# Compression component
if(CPPREST_EXCLUDE_COMPRESSION)
  target_compile_definitions(cpprest PUBLIC -DCPPREST_EXCLUDE_COMPRESSION=1)
else()
  cpprest_find_zlib()
  target_include_directories(cpprest SYSTEM PRIVATE ${ZLIB_INCLUDE_DIRS})
  target_link_libraries(cpprest PRIVATE ${ZLIB_LIBRARIES})
endif()

# PPLX component
if(CPPREST_PPLX_IMPL STREQUAL "apple")
  find_library(COREFOUNDATION CoreFoundation "/")
  find_library(SECURITY Security "/")
  target_link_libraries(cpprest PRIVATE ${COREFOUNDATION} ${SECURITY})

  target_sources(cpprest PRIVATE
    pplx/threadpool.cpp
    pplx/pplxapple.cpp
  )
elseif(CPPREST_PPLX_IMPL STREQUAL "linux")
  find_library(Threads REQUIRED)
  target_link_libraries(cpprest PRIVATE ${CMAKE_THREAD_LIBS_INIT})

  target_sources(cpprest PRIVATE
    pplx/threadpool.cpp
    pplx/pplxlinux.cpp
  )
elseif(CPPREST_PPLX_IMPL STREQUAL "win")
  target_sources(cpprest PRIVATE pplx/pplxwin.cpp)
else()
  message(FATAL_ERROR "Invalid implementation")
endif()

# Http client component
if(CPPREST_HTTP_CLIENT_IMPL STREQUAL "asio")
  cpprest_find_boost()
  target_include_directories(cpprest SYSTEM PRIVATE ${Boost_INCLUDE_DIR})
  target_link_libraries(cpprest PRIVATE ${Boost_LIBRARIES})

  target_sources(cpprest PRIVATE http/client/http_client_asio.cpp)
elseif(CPPREST_HTTP_CLIENT_IMPL STREQUAL "winhttp")
  target_sources(cpprest PRIVATE http/client/http_client_winhttp.cpp)
  target_link_libraries(cpprest PRIVATE
    bcrypt.lib
    crypt32.lib
    httpapi.lib
    Winhttp.lib
  )
elseif(CPPREST_HTTP_CLIENT_IMPL STREQUAL "winrt")
  target_sources(cpprest PRIVATE http/client/http_client_winrt.cpp)
else()
  message(FATAL_ERROR "Invalid implementation")
endif()

# fileio streams component
if(CPPREST_FILEIO_IMPL STREQUAL "win32")
  target_sources(cpprest PRIVATE streams/fileio_win32.cpp)
elseif(CPPREST_FILEIO_IMPL STREQUAL "winrt")
  target_sources(cpprest PRIVATE streams/fileio_winrt.cpp)
elseif(CPPREST_FILEIO_IMPL STREQUAL "posix")
  target_sources(cpprest PRIVATE streams/fileio_posix.cpp)
else()
  message(FATAL_ERROR "Invalid implementation")
endif()

# http listener component
if(CPPREST_HTTP_LISTENER_IMPL STREQUAL "asio")
  target_sources(cpprest PRIVATE http/listener/http_server_asio.cpp)
elseif(CPPREST_HTTP_LISTENER_IMPL STREQUAL "httpsys")
  target_sources(cpprest PRIVATE http/listener/http_server_httpsys.cpp)
elseif(CPPREST_HTTP_LISTENER_IMPL STREQUAL "none")
else()
  message(FATAL_ERROR "Invalid implementation")
endif()

# Note: this section needs to be towards the end of the file, because it requires adding an intra-project dependency.
# This is done by loading the SOURCES property from the target thus far
if(MSVC)
  get_target_property(SOURCES cpprest SOURCES)

  target_compile_options(cpprest PRIVATE /Yustdafx.h /Zm200)
  set_source_files_properties(pch/stdafx.cpp PROPERTIES COMPILE_FLAGS "/Ycstdafx.h")

  if (NOT ${CMAKE_GENERATOR} MATCHES "Visual Studio .*")
    set_property(SOURCE pch/stdafx.cpp APPEND PROPERTY OBJECT_OUTPUTS "${CMAKE_CURRENT_BINARY_DIR}/stdafx.pch")
    set_property(SOURCE ${SOURCES} APPEND PROPERTY OBJECT_DEPENDS "${CMAKE_CURRENT_BINARY_DIR}/stdafx.pch")
  endif()

  target_sources(cpprest PRIVATE pch/stdafx.cpp pch/stdafx.h)
endif()

if(ANDROID)
  target_link_libraries(cpprest PRIVATE ${ANDROID_STL_FLAGS})
endif()

# Portions specific to cpprest binary versioning.
set (CPPREST_VERSION_MAJOR 2)
set (CPPREST_VERSION_MINOR 9)
set (CPPREST_VERSION_REVISION 0)

if(WIN32)
  set_target_properties(cpprest PROPERTIES
    OUTPUT_NAME "cpprest_${CPPREST_VERSION_MAJOR}_${CPPREST_VERSION_MINOR}")
elseif(ANDROID)
  # Do not use SOVERSION on android. It is completely unsupported (and causes problems).
  # Perhaps revisit in the future? (NDK r9d, 8/7/14)
else()
  set_target_properties(cpprest PROPERTIES
    SOVERSION ${CPPREST_VERSION_MAJOR}.${CPPREST_VERSION_MINOR})
endif()

install(
  TARGETS cpprest
  EXPORT cpprest-config
  RUNTIME DESTINATION bin
  LIBRARY DESTINATION lib
  ARCHIVE DESTINATION lib
)

install(EXPORT cpprest-config DESTINATION ${CPPREST_EXPORT_DIR})

function(cpprest_find_openssl)
  if(OPENSSL_FOUND)
    return()
  endif()

  if(IOS)
    set(IOS_SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/../Build_iOS")
    set(OPENSSL_FOUND 1 CACHE INTERNAL "")
    set(OPENSSL_INCLUDE_DIR "${IOS_SOURCE_DIR}/openssl/include" CACHE INTERNAL "")
    set(OPENSSL_LIBRARIES
      "${IOS_SOURCE_DIR}/openssl/lib/libcrypto.a"
      "${IOS_SOURCE_DIR}/openssl/lib/libssl.a"
      CACHE INTERNAL ""
      )
  elseif(ANDROID)
    set(OPENSSL_FOUND 1 CACHE INTERNAL "")
    if(ARM)
      set(OPENSSL_INCLUDE_DIR "${CMAKE_BINARY_DIR}/../openssl/armeabi-v7a/include" CACHE INTERNAL "")
      set(OPENSSL_LIBRARIES
        "${CMAKE_BINARY_DIR}/../openssl/armeabi-v7a/lib/libssl.a"
        "${CMAKE_BINARY_DIR}/../openssl/armeabi-v7a/lib/libcrypto.a"
        CACHE INTERNAL ""
        )
    else()
      set(OPENSSL_INCLUDE_DIR "${CMAKE_BINARY_DIR}/../openssl/x86/include" CACHE INTERNAL "")
      set(OPENSSL_LIBRARIES
        "${CMAKE_BINARY_DIR}/../openssl/x86/lib/libssl.a"
        "${CMAKE_BINARY_DIR}/../openssl/x86/lib/libcrypto.a"
        CACHE INTERNAL ""
        )
    endif()
  elseif(APPLE)
    if(NOT DEFINED OPENSSL_ROOT_DIR)
      # Prefer a homebrew version of OpenSSL over the one in /usr/lib
      file(GLOB OPENSSL_ROOT_DIR /usr/local/Cellar/openssl/*)
      # Prefer the latest (make the latest one first)
      list(REVERSE OPENSSL_ROOT_DIR)
    endif()
    # This should prevent linking against the system provided 0.9.8y
    set(_OPENSSL_VERSION "")
    find_package(OpenSSL 1.0.0 REQUIRED)
    set(OPENSSL_LIBRARIES ${OPENSSL_LIBRARIES} CACHE INTERNAL "")
  else()
    find_package(OpenSSL 1.0.0 REQUIRED)
    set(OPENSSL_LIBRARIES ${OPENSSL_LIBRARIES} CACHE INTERNAL "")
  endif()

endfunction()
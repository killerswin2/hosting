cmake_minimum_required(VERSION 3.24)

project(hosting)

file(GLOB SOURCES_LIBRARY "src/library/*.cpp" "src/library/*.hpp")
file(GLOB SOURCES_BINDING "src/binding/*.cpp" "src/binding/*.hpp")
file(GLOB SOURCES_NETHOST "src/nethostfxr/*.cpp" "src/nethostfxr/*.hpp")

if(APPLE)
    set(DOTNET_RUNTIME_VERSION "7.0.3")
    set(DOTNET_SDK_VERSION "7.0.201")
    set(DOTNET_SDK_ROOT_PATH "/usr/local/share/dotnet")
    set(DOTNET_SDK_PATH "${DOTNET_SDK_ROOT_PATH}/sdk/${DOTNET_SDK_VERSION}")
    set(DOTNET_SDK_RUNTIME_PATH "${DOTNET_SDK_ROOT_PATH}/shared/Microsoft.NETCore.App/${DOTNET_RUNTIME_VERSION}")
    set(DOTNET_SDK_NETHOST_PATH "${DOTNET_SDK_ROOT_PATH}/packs/Microsoft.NETCore.App.Host.osx-arm64/${DOTNET_RUNTIME_VERSION}/runtimes/osx-arm64/native")
    set(DOTNET_SDK_FXRHOST_PATH "${DOTNET_SDK_ROOT_PATH}/host/fxr/${DOTNET_RUNTIME_VERSION}")
endif()

message(${DOTNET_SDK_NETHOST_PATH})
message(${DOTNET_SDK_RUNTIME_PATH})
message(${DOTNET_SDK_PATH})
message(${DOTNET_SDK_ROOT_PATH})

add_executable(hosting "src/main.cpp" ${SOURCES_LIBRARY})
target_link_directories(hosting PUBLIC ${DOTNET_SDK_ROOT_PATH} PUBLIC ${DOTNET_SDK_PATH} PUBLIC ${DOTNET_SDK_RUNTIME_PATH} PUBLIC ${DOTNET_SDK_NETHOST_PATH} PUBLIC ${DOTNET_SDK_FXRHOST_PATH})
target_include_directories(hosting PUBLIC ${DOTNET_SDK_ROOT_PATH} PUBLIC ${DOTNET_SDK_PATH} PUBLIC ${DOTNET_SDK_RUNTIME_PATH} PUBLIC ${DOTNET_SDK_NETHOST_PATH})
target_link_libraries(hosting PUBLIC nethost PUBLIC hostfxr)

set_property(TARGET hosting PROPERTY CXX_STANDARD 17)


set(MY_DEPENDENCY_PATHS /path/one /path/two)
set(CMAKE_INSTALL_PREFIX ${CMAKE_BINARY_DIR}/../)

# Transfer the value of ${MY_DEPENDENCY_PATHS} into the install script
install(CODE "set(MY_DEPENDENCY_PATHS \"${MY_DEPENDENCY_PATHS}\")")

install(CODE [[
  file(GET_RUNTIME_DEPENDENCIES
    LIBRARIES $<TARGET_FILE:hosting>
    EXECUTABLES $<TARGET_FILE:hosting>
    RESOLVED_DEPENDENCIES_VAR _r_deps
    UNRESOLVED_DEPENDENCIES_VAR _u_deps
    DIRECTORIES ${MY_DEPENDENCY_PATHS}
  )
  foreach(_file ${_r_deps})
    file(INSTALL
      DESTINATION "${CMAKE_INSTALL_PREFIX}"
      TYPE SHARED_LIBRARY
      FOLLOW_SYMLINK_CHAIN
      FILES "${_file}"
    )
  endforeach()
  list(LENGTH _u_deps _u_length)
  if("${_u_length}" GREATER 0)
    message(WARNING "Unresolved dependencies detected!")
  endif()
]])
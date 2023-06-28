cmake_minimum_required(VERSION 3.24)

project(hosting)

file(GLOB SOURCES_LIBRARY "src/library/*.cpp" "src/library/*.hpp")
file(GLOB SOURCES_BINDING "src/binding/*.cpp" "src/binding/*.hpp")
file(GLOB SOURCES_NETHOST "src/nethostfxr/*.cpp" "src/nethostfxr/*.hpp")

if(APPLE)
    set(DOTNET_RUNTIME_VERSION "7.0.5")
    set(DOTNET_SDK_VERSION "7.0.203")
    set(DOTNET_SDK_ROOT_PATH "/usr/local/share/dotnet")
    set(DOTNET_SDK_PATH "${DOTNET_SDK_ROOT_PATH}/sdk/${DOTNET_SDK_VERSION}")
    set(DOTNET_SDK_RUNTIME_PATH "${DOTNET_SDK_ROOT_PATH}/shared/Microsoft.NETCore.App/${DOTNET_RUNTIME_VERSION}")
    set(DOTNET_SDK_NETHOST_PATH "${DOTNET_SDK_ROOT_PATH}/packs/Microsoft.NETCore.App.Host.osx-arm64/${DOTNET_RUNTIME_VERSION}/runtimes/osx-arm64/native")
    set(DOTNET_SDK_FXRHOST_PATH "${DOTNET_SDK_ROOT_PATH}/host/fxr/${DOTNET_RUNTIME_VERSION}")
endif()

if(WIN32)
    set(DOTNET_RUNTIME_VERSION "7.0.5")
    set(DOTNET_SDK_VERSION "7.0.203")
    set(DOTNET_SDK_ROOT_PATH "C:/Program Files/dotnet")
    set(DOTNET_SDK_PATH "${DOTNET_SDK_ROOT_PATH}/sdk/${DOTNET_SDK_VERSION}")
    set(DOTNET_SDK_RUNTIME_PATH "${DOTNET_SDK_ROOT_PATH}/shared/Microsoft.NETCore.App/${DOTNET_RUNTIME_VERSION}")
    set(DOTNET_SDK_NETHOST_PATH "${DOTNET_SDK_ROOT_PATH}/packs/Microsoft.NETCore.App.Host.win-x64/${DOTNET_RUNTIME_VERSION}/runtimes/win-x64/native")
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

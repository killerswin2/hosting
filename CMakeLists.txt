cmake_minimum_required(VERSION 3.25)

project(hosting)

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

add_executable(hosting "src/main.cpp")
target_link_directories(hosting PUBLIC ${DOTNET_SDK_ROOT_PATH} PUBLIC ${DOTNET_SDK_PATH} PUBLIC ${DOTNET_SDK_RUNTIME_PATH} PUBLIC ${DOTNET_SDK_NETHOST_PATH} PUBLIC ${DOTNET_SDK_FXRHOST_PATH})
target_include_directories(hosting PUBLIC ${DOTNET_SDK_ROOT_PATH} PUBLIC ${DOTNET_SDK_PATH} PUBLIC ${DOTNET_SDK_RUNTIME_PATH} PUBLIC ${DOTNET_SDK_NETHOST_PATH})
target_link_libraries(hosting PUBLIC nethost PUBLIC hostfxr)

set_property(TARGET hosting PROPERTY CXX_STANDARD 17)
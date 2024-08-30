
cmake_minimum_required(VERSION 3.6)
project(OHook CXX)
enable_language(C)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

if(MSVC)
    add_compile_options(/MP /MD /wd4099 /wd4309 /wd4369 /wd4244 /wd4005)
    add_definitions(-D_CRT_SECURE_NO_WARNINGS)
endif()

#MinHook
file(GLOB MINHOOK_SOURCES minhook/*.c minhook/hde/*.c)
file(GLOB MINHOOK_HEADERS minhook/*.h minhook/hde/*.h)
add_library(minhook STATIC ${MINHOOK_SOURCES} ${MINHOOK_HEADERS})
target_include_directories(minhook PUBLIC minhook)

# ImGUI
file(GLOB IMGUI_SOURCES ImGui/*.cpp ImGui/impls/*.cpp)
file(GLOB IMGUI_HEADERS ImGui/*.h ImGui/impls/*.h ImGui/fonts/*.h)
if(WIN32)
    file(GLOB IMGUI_PLATFORM_SOURCES ImGui/impls/windows/*.cpp)
    file(GLOB IMGUI_PLATFORM_HEADERS ImGui/impls/windows/*.h)
elseif(UNIX AND NOT APPLE)
    file(GLOB IMGUI_PLATFORM_SOURCES ImGui/impls/linux/*.cpp)
    file(GLOB IMGUI_PLATFORM_HEADERS ImGui/impls/linux/*.h)
endif()
list(APPEND IMGUI_SOURCES ${IMGUI_PLATFORM_SOURCES})
list(APPEND IMGUI_HEADERS ${IMGUI_PLATFORM_HEADERS})

add_library(imgui STATIC ${IMGUI_SOURCES} ${IMGUI_HEADERS})
target_include_directories(imgui PUBLIC ImGui)
target_compile_definitions(imgui PRIVATE IMGUI_DEFINE_MATH_OPERATORS)

# UniversalHookX
file(GLOB_RECURSE UNIVERSALHOOK_SOURCES UniversalHookX/*.cpp)
file(GLOB_RECURSE UNIVERSALHOOK_HEADERS UniversalHookX/*.hpp UniversalHookX/*.h)
add_library(UniversalHookX STATIC ${UNIVERSALHOOK_SOURCES} ${UNIVERSALHOOK_HEADERS})
target_include_directories(UniversalHookX PUBLIC UniversalHookX)
target_link_libraries(UniversalHookX PUBLIC imgui minhook)

# PaliaSDK
file(GLOB_RECURSE PALIASDK_SOURCES PaliaSDK/*.cpp)
file(GLOB_RECURSE PALIASDK_HEADERS PaliaSDK/*.hpp)
add_library(PaliaSDK STATIC ${PALIASDK_SOURCES} ${PALIASDK_HEADERS})
target_include_directories(PaliaSDK PUBLIC PaliaSDK)
target_precompile_headers(PaliaSDK PRIVATE PaliaSDK/SDK.hpp)

# JsonCPP
file(GLOB_RECURSE JSONCPP_SOURCES jsoncpp/dist/*.cpp)
file(GLOB_RECURSE JSONCPP_HEADERS jsoncpp/include/json/*.h)
add_library(jsoncpp STATIC ${JSONCPP_SOURCES} ${JSONCPP_HEADERS})
target_include_directories(jsoncpp PUBLIC jsoncpp/include)

# OHook DLL
if(EXISTS "${CMAKE_BINARY_DIR}/OHook.pdb")
   file(REMOVE ${CMAKE_BINARY_DIR}/OHook.pdb)
endif()
file(GLOB_RECURSE OHook_SOURCES OHook/*.cpp)
file(GLOB_RECURSE OHook_HEADERS OHook/*.h OHook/*.hpp)
add_library(OHook SHARED ${OHook_SOURCES} ${OHook_HEADERS})
target_include_directories(OHook PUBLIC OHook)
target_link_libraries(OHook PRIVATE PaliaSDK jsoncpp UniversalHookX minhook $<$<BOOL:${WIN32}>:ws2_32> $<$<BOOL:${WIN32}>:Winmm.lib>)

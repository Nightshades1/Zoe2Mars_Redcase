cmake_minimum_required(VERSION 4.2)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
project(Zoe2Mars_Redcase LANGUAGES CXX)

if(MSVC)
set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")
endif()

add_subdirectory(Zoe2Mars_Redcase)
add_subdirectory(Zoe2Mars_Redcase_Launcher)
add_subdirectory(StageDec)


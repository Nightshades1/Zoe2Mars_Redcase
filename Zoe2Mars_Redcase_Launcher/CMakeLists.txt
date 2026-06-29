set(LAUNCHER_INCLUDE  "include/resource.h")
set(LAUNCHER_SRC      "src/Zoe2Mars_Redcase_Launcher.cpp")
set(LAUNCHER_RES      "res/Zoe2Mars_Redcase_Launcher.rc" "res/icon.ico")

source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR}      FILES ${LAUNCHER_INCLUDE})
source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR}      FILES ${LAUNCHER_SRC})
source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR}      FILES ${LAUNCHER_RES})

find_path(DETOURS_INCLUDE_DIRS "detours/detours.h")
find_library(DETOURS_LIBRARY detours REQUIRED)

if(MSVC)
set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")
endif()

add_executable(Zoe2Mars_Redcase_Launcher
${LAUNCHER_INCLUDE}
${LAUNCHER_SRC} 
${LAUNCHER_RES}
)

target_include_directories(Zoe2Mars_Redcase_Launcher PRIVATE
    "${CMAKE_CURRENT_SOURCE_DIR}/include"
    ${DETOURS_INCLUDE_DIRS}
)

target_link_libraries(Zoe2Mars_Redcase_Launcher PRIVATE ${DETOURS_LIBRARY})
install(TARGETS Zoe2Mars_Redcase_Launcher DESTINATION "./")
if(CMAKE_BUILD_TYPE STREQUAL "Debug")
install(FILES "${CMAKE_CURRENT_BINARY_DIR}/Zoe2Mars_Redcase_Launcher.pdb" DESTINATION "./")
endif()
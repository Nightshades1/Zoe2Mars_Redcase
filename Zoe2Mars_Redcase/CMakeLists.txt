set(ZOE2_MARS_REDCASE_MOD_ASSETS 
"ModAssets/arial.ttf" 
"ModAssets/Settings.ini"
"ModAssets/DefaultOffsets.json"
"ModAssets/Sounds.json"
)
set(ZOE2_MARS_REDCASE_INCLUDE 
"include/pch.h"
"include/GameTypes/Jehuty/Jehuty.h"
"include/ModPanel.h"
"include/PtrTypedefs.h" 
"include/SoundTest.h")
set(ZOE2_MARS_REDCASE_SRC
"src/pch.cpp"
"src/dllmain.cpp"
"src/exports.def"
"src/Hooks.cpp"
"src/Mod.cpp"
"src/ModPanel.cpp" 
)
set(ZOE2_MARS_REDCASE_SHARED "${CMAKE_SOURCE_DIR}/Shared/AudioManager/AudioManager.cpp" "${CMAKE_SOURCE_DIR}/Shared/AudioManager/AudioManager.h")

# AudioKinetic
file(GLOB_RECURSE WWISE_SDK "${CMAKE_SOURCE_DIR}/third-party/Wwise_2017.2.10.6745/SDK/include/*.h")

source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR}        FILES  ${ZOE2_MARS_REDCASE_MOD_ASSETS})
source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR}        FILES  ${ZOE2_MARS_REDCASE_INCLUDE})
source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR}        FILES  ${ZOE2_MARS_REDCASE_SRC})
source_group(TREE ${CMAKE_SOURCE_DIR}                FILES  ${ZOE2_MARS_REDCASE_SHARED})
source_group(TREE ${CMAKE_SOURCE_DIR}                FILES  ${WWISE_SDK})

find_path(DETOURS_INCLUDE_DIRS "detours/detours.h")
find_library(DETOURS_LIBRARY detours REQUIRED)
find_package(imgui CONFIG REQUIRED)

if(MSVC)
set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")
set(CMAKE_MSVC_DEBUG_INFORMATION_FORMAT "$<$<CONFIG:Debug>:EditAndContinue>")
endif()

add_library(Zoe2Mars_Redcase64 SHARED
${ZOE2_MARS_REDCASE_MOD_ASSETS}
${ZOE2_MARS_REDCASE_INCLUDE}
${ZOE2_MARS_REDCASE_SRC}
${ZOE2_MARS_REDCASE_SHARED}
${WWISE_SDK}
)

target_include_directories(Zoe2Mars_Redcase64 PRIVATE
    "${CMAKE_CURRENT_SOURCE_DIR}/include"
    ${DETOURS_INCLUDE_DIRS}
    "${CMAKE_SOURCE_DIR}/Shared/AudioManager"
    "${CMAKE_SOURCE_DIR}/third-party/Wwise_2017.2.10.6745/SDK/include"
)

target_precompile_headers(Zoe2Mars_Redcase64 PRIVATE "include/pch.h")
target_link_libraries(Zoe2Mars_Redcase64 PRIVATE 
    imgui::imgui 
    ${DETOURS_LIBRARY}
)

# Preprocessor flag so we ensure that we build with the Demo Version offsets.
if(IS_DEMO_BUILD)
target_compile_definitions(Zoe2Mars_Redcase64 PRIVATE DEMO)
endif()

# .dll
install(FILES "${CMAKE_CURRENT_BINARY_DIR}/Zoe2Mars_Redcase64.dll" DESTINATION "./")
if(CMAKE_BUILD_TYPE STREQUAL "Debug")
install(FILES "${CMAKE_CURRENT_BINARY_DIR}/Zoe2Mars_Redcase64.pdb" DESTINATION "./")
endif()

# ZOE1 Main theme + Intro track
install(FILES "${CMAKE_SOURCE_DIR}/Assets/mod_zoe1.bnk" DESTINATION "./sound/Windows/English(US)")

# The mod assets
install(FILES ${ZOE2_MARS_REDCASE_MOD_ASSETS} DESTINATION "ModAssets")
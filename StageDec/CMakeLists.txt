set(STAGEDEC_INCLUDE
"include/pch.h"
"include/StageDAT.h"
"include/StageDec.h"
"include/Utility.h"
)
set(STAGEDEC_SRC
"src/pch.cpp"
"src/StageDAT.cpp"
"src/StageDec.cpp"
"src/Utility.cpp"
)
set(STAGEDEC_RES "res/strlist.txt")
set(SAMPLES_FILES
"${CMAKE_SOURCE_DIR}/Assets/Samples/Build_And_Copy(Auto - Verbose).bat"
"${CMAKE_SOURCE_DIR}/Assets/Samples/Build_And_Copy(Auto - Verbose)_DemoVersion.bat"
"${CMAKE_SOURCE_DIR}/Assets/Samples/Build_And_Copy(Auto).bat"
"${CMAKE_SOURCE_DIR}/Assets/Samples/Build_And_Copy(Auto)_DemoVersion.bat"
"${CMAKE_SOURCE_DIR}/Assets/Samples/Build_And_Copy(DataCnf - Verbose).bat"
"${CMAKE_SOURCE_DIR}/Assets/Samples/Build_And_Copy(DataCnf - Verbose)_DemoVersion.bat"
"${CMAKE_SOURCE_DIR}/Assets/Samples/Build_And_Copy(DataCnf).bat"
"${CMAKE_SOURCE_DIR}/Assets/Samples/Build_And_Copy(DataCnf)_DemoVersion.bat"
"${CMAKE_SOURCE_DIR}/Assets/Samples/Extract (Verbose).bat"
"${CMAKE_SOURCE_DIR}/Assets/Samples/Extract.bat"
)
set(STAGEDEC_SHARED "${CMAKE_SOURCE_DIR}/Shared/AudioManager/AudioManager.cpp" "${CMAKE_SOURCE_DIR}/Shared/AudioManager/AudioManager.h")

source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR}  FILES ${STAGEDEC_INCLUDE})
source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR}  FILES ${STAGEDEC_SRC})
source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR}  FILES ${STAGEDEC_RES})
source_group(TREE ${CMAKE_SOURCE_DIR}          FILES ${STAGEDEC_SHARED})
source_group(TREE ${CMAKE_SOURCE_DIR}          FILES ${SAMPLES_FILES})


find_package(CLI11 CONFIG REQUIRED)
find_package(ZLIB REQUIRED)

add_executable(StageDec
${STAGEDEC_INCLUDE} 
${STAGEDEC_SRC} 
${STAGEDEC_RES}
${STAGEDEC_SHARED}
${SAMPLES_FILES}
)

if(MSVC)
target_precompile_headers(StageDec PRIVATE "include/pch.h")
endif()

target_include_directories(StageDec PRIVATE
${CMAKE_CURRENT_SOURCE_DIR}/include
"${CMAKE_SOURCE_DIR}/Shared/AudioManager"
)

target_link_libraries(StageDec PRIVATE CLI11::CLI11)
target_link_libraries(StageDec PRIVATE ZLIB::ZLIB)

install(TARGETS StageDec DESTINATION "Tool/StageDec")
install(FILES ${STAGEDEC_RES} ${SAMPLES_FILES} DESTINATION "Tool/StageDec")
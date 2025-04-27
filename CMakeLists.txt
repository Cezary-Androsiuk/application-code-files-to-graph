cmake_minimum_required(VERSION 3.16)

project(ApplicationCodeFilesToGraph LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include_directories(${CMAKE_SOURCE_DIR}/source)

add_executable(
    ${PROJECT_NAME}

    startup.json

    source/main.cpp

    source/nlohmann_json/json.hpp

    # Log library
    source/Log/Log.h source/Log/Log.cpp

    source/ReadStartupJson.h source/ReadStartupJson.cpp
    source/File.h source/File.cpp
    source/Program.h source/Program.cpp
)

# file(GLOB RESOURCE_FILES_LIST "${CMAKE_SOURCE_DIR}/resources/*")
# add_custom_target(resources_copy ALL
#     COMMAND ${CMAKE_COMMAND} -E copy_directory
#         ${CMAKE_SOURCE_DIR}/resources
#         ${CMAKE_BINARY_DIR}/resources
#     DEPENDS ${RESOURCE_FILES_LIST}
#     COMMENT "Copying resources to build directory"
# )
# add_dependencies(${PROJECT_NAME} resources_copy)

set(SOURCE_JSON "${CMAKE_SOURCE_DIR}/startup.json")
set(TARGET_JSON "${CMAKE_BINARY_DIR}/startup.json")
add_custom_target(copy_startup_json ALL
    COMMAND ${CMAKE_COMMAND} -E copy
        ${SOURCE_JSON}
        ${TARGET_JSON}
    DEPENDS ${SOURCE_JSON}
    COMMENT "Copying startup.json to build directory"
)
add_dependencies(${PROJECT_NAME} copy_startup_json)

include(GNUInstallDirs)
install(TARGETS ${PROJECT_NAME}
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)

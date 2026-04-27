cmake_minimum_required(VERSION 3.12)
project(ReactorSimulatorGame)

# vcpkg integration: Try to find the toolchain file automatically if not provided
if(NOT CMAKE_TOOLCHAIN_FILE)
    if(DEFINED ENV{VCPKG_ROOT})
        set(CMAKE_TOOLCHAIN_FILE "$ENV{VCPKG_ROOT}/scripts/buildsystems/vcpkg.cmake" CACHE STRING "")
    elseif(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/vcpkg/scripts/buildsystems/vcpkg.cmake")
        # Support for vcpkg as a git submodule in the project root
        set(CMAKE_TOOLCHAIN_FILE "${CMAKE_CURRENT_SOURCE_DIR}/vcpkg/scripts/buildsystems/vcpkg.cmake" CACHE STRING "")
    endif()
endif()

set(CMAKE_CXX_STANDARD 23)

# Find raylib (vcpkg toolchain is set by CMakePresets.json)
find_package(raylib CONFIG REQUIRED)

# Reusable function for raylib projects
function(add_raylib_executable TARGET_NAME)
    cmake_parse_arguments(ARG "" "RESOURCES" "SOURCES" ${ARGN})

    # Create executable
    add_executable(${TARGET_NAME} ${ARG_SOURCES})

    # Include project root
    target_include_directories(${TARGET_NAME} PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})

    # Link raylib
    target_link_libraries(${TARGET_NAME} PRIVATE raylib)

    # Windows-specific libraries
    if (WIN32)
        target_link_libraries(${TARGET_NAME} PRIVATE winmm)
    endif()

    # Handle resources if specified
    if (ARG_RESOURCES)
        set(RELEASE_RESOURCE_DIR "${CMAKE_CURRENT_BINARY_DIR}/bin/${TARGET_NAME}_${PROJECT_VERSION}/Resources/")
        set(DEBUG_RESOURCE_DIR "${CMAKE_CURRENT_BINARY_DIR}/Resources/")

        target_compile_definitions(
                ${TARGET_NAME}
                PUBLIC
                RESOURCES_PATH=$<IF:$<CONFIG:Release>,"./Resources/","${DEBUG_RESOURCE_DIR}">
        )

        add_custom_command(
                TARGET ${TARGET_NAME}
                POST_BUILD
                COMMAND ${CMAKE_COMMAND} -E copy_directory
                "${CMAKE_CURRENT_SOURCE_DIR}/${ARG_RESOURCES}"
                $<IF:$<CONFIG:Release>,"${RELEASE_RESOURCE_DIR}","${DEBUG_RESOURCE_DIR}">
        )
    endif()
endfunction()

# Collect all source files. CONFIGURE_DEPENDS makes the build system re-glob
# on every build invocation, so newly added .cpp / .h files under Classes/
# are picked up without a manual `Reload CMake Project` in CLion.
file(GLOB_RECURSE CLASS_SOURCES CONFIGURE_DEPENDS "Classes/*.cpp")
file(GLOB_RECURSE CLASS_HEADERS CONFIGURE_DEPENDS "Classes/*.h")

# Add the main executable
add_raylib_executable(ReactorSim
        SOURCES
        main.cpp
        ${CLASS_SOURCES}
        ${CLASS_HEADERS}
        RESOURCES
        Resources
)

# Add the project root to the include directories for cleaner include paths.
target_include_directories(ReactorSim PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
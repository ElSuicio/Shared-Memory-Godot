cmake_minimum_required(VERSION 3.20)

set(LIBRARY_NAME "shared_memory")
set(GODOT_PROJECT_DIR "test-shared-memory")

project(
    SharedMemory-Godot
    VERSION 1.0
    LANGUAGES CXX
    HOMEPAGE_URL https://github.com/ElSuicio/Shared-Memory-Godot
)

add_subdirectory("third_party/godot-cpp" SYSTEM)

list(APPEND CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/third_party/godot-cpp/cmake")
include(GodotCPPModule)

get_target_property(GODOTCPP_SUFFIX godot::cpp GODOTCPP_SUFFIX)
get_target_property(GODOTCPP_PLATFORM godot::cpp GODOTCPP_PLATFORM)

file(GLOB_RECURSE COMMON_HEADERS CONFIGURE_DEPENDS "${CMAKE_CURRENT_SOURCE_DIR}/src/*.h")
file(GLOB_RECURSE COMMON_SOURCES CONFIGURE_DEPENDS "${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp")
list(FILTER COMMON_SOURCES EXCLUDE REGEX "_windows\\.cpp$")

file(GLOB_RECURSE WINDOWS_SOURCES CONFIGURE_DEPENDS "${CMAKE_CURRENT_SOURCE_DIR}/src/*_windows.cpp")

add_library(${LIBRARY_NAME} SHARED)

target_sources(${LIBRARY_NAME}
    PRIVATE
    ${COMMON_HEADERS}
    ${COMMON_SOURCES}
    $<$<PLATFORM_ID:Windows>:${WINDOWS_SOURCES}>
)

target_link_libraries(${LIBRARY_NAME}
    PRIVATE
        godot-cpp
)

if(MSVC)
    target_compile_options(${LIBRARY_NAME} PRIVATE /utf-8)
endif()

file(GLOB_RECURSE DOC_XML CONFIGURE_DEPENDS "${PROJECT_SOURCE_DIR}/doc_classes/*.xml")

if(DOC_XML AND GODOTCPP_TARGET MATCHES "editor|template_debug")
    target_doc_sources(${LIBRARY_NAME} ${DOC_XML})
endif()

target_compile_features(${LIBRARY_NAME}
    PRIVATE
        cxx_std_17
)

set_target_properties(${LIBRARY_NAME}
    PROPERTIES
        RUNTIME_OUTPUT_DIRECTORY
            "$<1:${PROJECT_SOURCE_DIR}/bin/${GODOTCPP_PLATFORM}>"
        ARCHIVE_OUTPUT_DIRECTORY
            "$<1:${PROJECT_SOURCE_DIR}/bin/${GODOTCPP_PLATFORM}>"
        LIBRARY_OUTPUT_DIRECTORY
            "$<1:${PROJECT_SOURCE_DIR}/bin/${GODOTCPP_PLATFORM}>"
        PREFIX
            ""
        OUTPUT_NAME
            "${LIBRARY_NAME}${GODOTCPP_SUFFIX}"
)

set(GODOT_PROJECT_BINARY_DIR "${PROJECT_SOURCE_DIR}/${GODOT_PROJECT_DIR}/bin/${GODOTCPP_PLATFORM}")

add_custom_command(TARGET ${LIBRARY_NAME} POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E make_directory
        "${GODOT_PROJECT_BINARY_DIR}"
    COMMAND ${CMAKE_COMMAND} -E copy
        "$<TARGET_FILE:${LIBRARY_NAME}>"
        "${GODOT_PROJECT_BINARY_DIR}/$<TARGET_FILE_NAME:${LIBRARY_NAME}>"
    COMMAND ${CMAKE_COMMAND} -E copy
        "$<TARGET_LINKER_FILE:${LIBRARY_NAME}>"
        "${GODOT_PROJECT_BINARY_DIR}/$<TARGET_LINKER_FILE_NAME:${LIBRARY_NAME}>"
)

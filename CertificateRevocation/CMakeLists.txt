# (c) 2022 Copyright, Real-Time Innovations, Inc. (RTI) All rights reserved.
#
# RTI grants Licensee a license to use, modify, compile, and create derivative
# works of the software solely for use with RTI Connext DDS.  Licensee may
# redistribute copies of the software provided that all such copies are
# subject to this license. The software is provided "as is", with no warranty
# of any type, including any warranty for fitness for any purpose. RTI is
# under no obligation to maintain or support the software.  RTI shall not be
# liable for any incidental or consequential damages arising out of the use or
# inability to use the software.

cmake_minimum_required(VERSION 3.7)
project(ShapePublisher C CXX)
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Copy the USER_QOS_PROFILES.xml to build directory
add_custom_target(copy_qos_to_build ALL)

add_custom_command(TARGET copy_qos_to_build PRE_BUILD
    COMMAND
        ${CMAKE_COMMAND} -E copy_if_different
        "${CMAKE_CURRENT_SOURCE_DIR}/config/xml/qos/USER_QOS_PROFILES.xml"
        "${CMAKE_CURRENT_BINARY_DIR}/.."
    COMMENT "Copying USER_QOS_PROFILES.xml"
    DEPENDS
        "${CMAKE_CURRENT_SOURCE_DIR}/config/xml/qos/USER_QOS_PROFILES.xml"
    VERBATIM
)

set(CMAKE_MODULE_PATH
    ${CMAKE_MODULE_PATH}
    ${CMAKE_CURRENT_SOURCE_DIR}/resources/rticonnextdds-cmake-utils/cmake/Modules
)

# Find the ConnextDDS libraries. This will look fo the core and API libraries
# only
find_package(RTIConnextDDS
    "7.0.0"
    REQUIRED
    COMPONENTS
    core
)

# Set paths
set(PATH_CODEGEN ${CMAKE_CURRENT_SOURCE_DIR}/src/generated)
set(PATH_COMMON ${CMAKE_CURRENT_SOURCE_DIR}/src/common)
set(PATH_SHAPEPUBLISHER ${CMAKE_CURRENT_SOURCE_DIR}/src/shapePublisher)
set(PATH_IGNOREPARTICIPANT ${CMAKE_CURRENT_SOURCE_DIR}/src/ignoreParticipant)

# Set files generated from code gen
set(FILES_CODEGEN
    ${PATH_CODEGEN}/Shapes.cxx
    ${PATH_CODEGEN}/ShapesPlugin.cxx
    ${PATH_CODEGEN}/IDS.cxx
    ${PATH_CODEGEN}/IDSPlugin.cxx
)

# Set files from common directory
set(FILES_COMMON ${PATH_COMMON}/ConnextCommunicator.cxx)

# Run Codegen to generate the source code files for the types
include(ConnextDdsCodegen)
connextdds_rtiddsgen_run(
    IDL_FILE ${CMAKE_CURRENT_SOURCE_DIR}/config/idl/Shapes.idl
    OUTPUT_DIRECTORY ${PATH_CODEGEN}
    LANG C++11
)
connextdds_rtiddsgen_run(
    IDL_FILE ${CMAKE_CURRENT_SOURCE_DIR}/config/idl/IDS.idl
    OUTPUT_DIRECTORY ${PATH_CODEGEN}
    LANG C++11
)

add_custom_target(codegen_sources
    DEPENDS ${FILES_CODEGEN}
)

# Create a common library used by all executables
add_library(common
  OBJECT ${FILES_CODEGEN}
  ${FILES_COMMON})

target_link_libraries(common RTIConnextDDS::cpp2_api)

add_dependencies(common codegen_sources)

# Create the target for the ShapePublisher executable
add_executable(ShapePublisher
    ${PATH_SHAPEPUBLISHER}/Main.cxx
    ${PATH_SHAPEPUBLISHER}/ShapePublisher.cxx
    $<TARGET_OBJECTS:common>
)

set_target_properties(ShapePublisher
    PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY_DEBUG ${CMAKE_CURRENT_BINARY_DIR}/..
)

target_include_directories(ShapePublisher
PRIVATE
    "${CMAKE_CURRENT_BINARY_DIR}"
    "${CMAKE_CURRENT_SOURCE_DIR}"
)

target_link_libraries(ShapePublisher RTIConnextDDS::cpp2_api)

# Create the target for the IgnoreParticipant executable
add_executable(IgnoreParticipant
    ${PATH_IGNOREPARTICIPANT}/Main.cxx
    ${PATH_IGNOREPARTICIPANT}/IgnoreParticipant.cxx
    $<TARGET_OBJECTS:common>
)

set_target_properties(IgnoreParticipant
    PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY_DEBUG ${CMAKE_CURRENT_BINARY_DIR}/..
)

target_include_directories(IgnoreParticipant
PRIVATE
    "${CMAKE_CURRENT_BINARY_DIR}"
    "${CMAKE_CURRENT_SOURCE_DIR}"
)

target_link_libraries(IgnoreParticipant RTIConnextDDS::cpp2_api)

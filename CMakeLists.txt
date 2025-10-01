cmake_minimum_required(VERSION 3.15)

# Project information
project(ArchiNote
    VERSION 1.0.0
    DESCRIPTION "Semantic Document Management System"
    LANGUAGES CXX
)

# Set C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Set build type if not specified
if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release)
endif()

# Compiler flags
add_compile_options(
    -Wall
    -Wextra
    -Wpedantic
    $<$<CONFIG:Debug>:-g>
    $<$<CONFIG:Release>:-O3>
)

# Find packages (optional dependencies)
find_package(HDF5 QUIET)

# Include directories
include_directories(${CMAKE_CURRENT_SOURCE_DIR})

# Source files - Main ArchiNote system
set(ARCHINOTE_SOURCES
    ArchiNote/RAIIStylePointer.hpp
    ArchiNote/ArchiNoteAdmin/IArchiNoteManager.hpp
    ArchiNote/ArchiNoteAttribute/ProjcetBriefAttributes.hpp
    ArchiNote/ArchiNoteStorage/IStorageAdapter.hpp
    ArchiNote/ArchiNoteStorage/HDF5StorageAdapter/HDF5StorageAdapter.hpp
    ArchiNote/Enums/ArchiNoteAttributeType.hpp
    ArchiNote/Enums/ArchiNoteAttributeRelationType.hpp
    ArchiNote/Enums/ArchiNoteType.hpp
    ArchiNote/Enums/AttributeContentDataType.hpp
    ArchiNote/Enums/ObjectiveSubType.hpp
    ArchiNote/Enums/RiskSubType.hpp
    ArchiNote/Structs/AttributeMetadata.hpp
    ArchiNote/Structs/ExportEvent.hpp
    ArchiNote/Structs/UpdateEvent.hpp
    ArchiNote/Structs/ValidationEvent.hpp
)

# Supporting types
set(SUPPORTING_SOURCES
    SupportingTypes/EAttributeContentType.hpp
    SupportingTypes/EAttributeType.hpp
    SupportingTypes/EKGNodeType.hpp
    SupportingTypes/EKGPathType.hpp
    SupportingTypes/EKGRelation.hpp
    SupportingTypes/KGBaseNode.cpp
    SupportingTypes/KGBaseNode.hpp
    SupportingTypes/KGEdge.cpp
    SupportingTypes/KGEdge.hpp
    SupportingTypes/KGPath.cpp
    SupportingTypes/KGPath.hpp
    SupportingTypes/KGStructuredAttribute.cpp
    SupportingTypes/KGStructuredAttribute.hpp
    SupportingTypes/NodePipelineManager.cpp
    SupportingTypes/NodePipelineManager.hpp
)

# KGraph sources
set(KGRAPH_SOURCES
    KGraph/KGraph.cpp
    KGraph/KGraph.hpp
)

# Create library targets
add_library(SupportingTypes STATIC ${SUPPORTING_SOURCES})
add_library(KGraph STATIC ${KGRAPH_SOURCES})
add_library(ArchiNote INTERFACE)

# Link dependencies
target_link_libraries(KGraph
    PUBLIC
        SupportingTypes
)

target_link_libraries(ArchiNote
    INTERFACE
        KGraph
        SupportingTypes
)

# Optional HDF5 support
if(HDF5_FOUND)
    target_link_libraries(ArchiNote INTERFACE ${HDF5_LIBRARIES})
    target_include_directories(ArchiNote INTERFACE ${HDF5_INCLUDE_DIRS})
    target_compile_definitions(ArchiNote INTERFACE -DHAS_HDF5)
endif()

# Installation
install(TARGETS SupportingTypes KGraph
    LIBRARY DESTINATION lib
    ARCHIVE DESTINATION lib
)

install(DIRECTORY ArchiNote/ SupportingTypes/ KGraph/
    DESTINATION include/ArchiNote
    FILES_MATCHING PATTERN "*.hpp"
)

# Enable testing
option(BUILD_TESTS "Build tests" OFF)
if(BUILD_TESTS)
    enable_testing()
    add_subdirectory(tests)
endif()

# Doxygen documentation (optional)
option(BUILD_DOCS "Build documentation" OFF)
if(BUILD_DOCS)
    find_package(Doxygen)
    if(DOXYGEN_FOUND)
        configure_file(${CMAKE_CURRENT_SOURCE_DIR}/Doxyfile.in
                      ${CMAKE_CURRENT_BINARY_DIR}/Doxyfile @ONLY)
        add_custom_target(doc
            ${DOXYGEN_EXECUTABLE} ${CMAKE_CURRENT_BINARY_DIR}/Doxyfile
            WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
            COMMENT "Generating API documentation with Doxygen" VERBATIM
        )
    endif(DOXYGEN_FOUND)
endif(BUILD_DOCS)

# Print configuration summary
message(STATUS "ArchiNote Configuration Summary:")
message(STATUS "  Version: ${PROJECT_VERSION}")
message(STATUS "  Build type: ${CMAKE_BUILD_TYPE}")
message(STATUS "  C++ Standard: ${CMAKE_CXX_STANDARD}")
message(STATUS "  HDF5 support: ${HDF5_FOUND}")
message(STATUS "  Build tests: ${BUILD_TESTS}")
message(STATUS "  Build docs: ${BUILD_DOCS}")
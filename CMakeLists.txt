cmake_minimum_required(VERSION 3.10)
project(PhantomFrame VERSION 1.0.0 LANGUAGES CXX)

# Set C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Set build type
if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release)
endif()

# Output directories
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)

# Compiler flags
set(CMAKE_CXX_FLAGS_DEBUG "-g -O0 -Wall -Wextra -DDEBUG")
set(CMAKE_CXX_FLAGS_RELEASE "-O3 -DNDEBUG")
set(CMAKE_CXX_FLAGS_RELWITHDEBINFO "-O2 -g -DNDEBUG")

# Find required packages
find_package(OpenCV REQUIRED)
find_package(PkgConfig REQUIRED)

# Include directories
include_directories(${CMAKE_SOURCE_DIR}/src)
include_directories(${OpenCV_INCLUDE_DIRS})

# Source files
set(SOURCES
    src/encoder/watermark_encoder.cpp
    src/extractor/watermark_extractor.cpp
    src/common/utils.cpp
)

# Header files
set(HEADERS
    src/encoder/watermark_encoder.h
    src/extractor/watermark_extractor.h
    src/common/utils.h
)

# Create library first
add_library(phantomframe_lib STATIC ${SOURCES} ${HEADERS})

# Link libraries to the library
target_link_libraries(phantomframe_lib ${OpenCV_LIBS})

# Set library properties
set_target_properties(phantomframe_lib PROPERTIES
    OUTPUT_NAME "phantomframe"
    VERSION ${PROJECT_VERSION}
)

# Create executable that links against the library
add_executable(phantomframe src/main.cpp)

# Link executable with the library
target_link_libraries(phantomframe phantomframe_lib)

# Set executable properties
set_target_properties(phantomframe PROPERTIES
    OUTPUT_NAME "phantomframe"
    VERSION ${PROJECT_VERSION}
)

# Install rules
install(TARGETS phantomframe
    RUNTIME DESTINATION bin
    LIBRARY DESTINATION lib
    ARCHIVE DESTINATION lib
)

# Install headers
install(FILES ${HEADERS}
    DESTINATION include/phantomframe
)

# Create package configuration
include(CMakePackageConfigHelpers)
write_basic_package_version_file(
    "${CMAKE_CURRENT_BINARY_DIR}/PhantomFrameConfigVersion.cmake"
    VERSION ${PROJECT_VERSION}
    COMPATIBILITY AnyNewerVersion
)

# Testing
enable_testing()
add_subdirectory(tests)

# Documentation
find_package(Doxygen)
if(DOXYGEN_FOUND)
    set(DOXYGEN_IN ${CMAKE_CURRENT_SOURCE_DIR}/docs/Doxyfile.in)
    set(DOXYGEN_OUT ${CMAKE_CURRENT_BINARY_DIR}/Doxyfile)
    
    configure_file(${DOXYGEN_IN} ${DOXYGEN_OUT} @ONLY)
    
    add_custom_target(docs ALL
        COMMAND ${DOXYGEN_EXECUTABLE} ${DOXYGEN_OUT}
        WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
        COMMENT "Generating API documentation with Doxygen"
        VERBATIM
    )
endif()

# Print configuration summary
message(STATUS "PhantomFrame Configuration:")
message(STATUS "  Version: ${PROJECT_VERSION}")
message(STATUS "  Build type: ${CMAKE_BUILD_TYPE}")
message(STATUS "  C++ standard: ${CMAKE_CXX_STANDARD}")
message(STATUS "  OpenCV version: ${OpenCV_VERSION}")
message(STATUS "  Install prefix: ${CMAKE_INSTALL_PREFIX}")

cmake_minimum_required(VERSION 3.16)
project(PhantomFrame VERSION 1.0.0 LANGUAGES CXX)

# Set C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Set build type if not specified
if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release)
endif()

# Output directories
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)

# Compiler flags
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -Wpedantic")
set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -g -O0 -DDEBUG")
set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -O3 -DNDEBUG")

# Find required packages
find_package(OpenCV REQUIRED)
find_package(PkgConfig REQUIRED)

# Find FFmpeg components
pkg_check_modules(FFMPEG REQUIRED IMPORTED_TARGET
    libavcodec
    libavformat
    libavutil
    libswscale
    libswresample
)

# Find CUDA if available
find_package(CUDA QUIET)
if(CUDA_FOUND)
    set(HAVE_CUDA TRUE)
    message(STATUS "CUDA found: ${CUDA_VERSION}")
else()
    set(HAVE_CUDA FALSE)
    message(STATUS "CUDA not found, building without GPU acceleration")
endif()

# Find OpenCL if available
find_package(OpenCL QUIET)
if(OpenCL_FOUND)
    set(HAVE_OPENCL TRUE)
    message(STATUS "OpenCL found")
else()
    set(HAVE_OPENCL FALSE)
    message(STATUS "OpenCL not found, building without OpenCL acceleration")
endif()

# Include directories
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/include)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/src)

# Add subdirectories
add_subdirectory(src)
add_subdirectory(tests)

# Create main executable
add_executable(phantomframe src/main.cpp)
target_link_libraries(phantomframe 
    phantomframe_core
    ${OpenCV_LIBS}
    PkgConfig::FFMPEG
)

if(CUDA_FOUND)
    target_link_libraries(phantomframe ${CUDA_LIBRARIES})
    target_include_directories(phantomframe PRIVATE ${CUDA_INCLUDE_DIRS})
endif()

if(OpenCL_FOUND)
    target_link_libraries(phantomframe ${OpenCL_LIBRARIES})
    target_include_directories(phantomframe PRIVATE ${OpenCL_INCLUDE_DIRS})
endif()

# Install targets
install(TARGETS phantomframe
    RUNTIME DESTINATION bin
    LIBRARY DESTINATION lib
    ARCHIVE DESTINATION lib
)

install(DIRECTORY include/ DESTINATION include/phantomframe)

# Package configuration
include(CMakePackageConfigHelpers)
write_basic_package_version_file(
    "${CMAKE_CURRENT_BINARY_DIR}/PhantomFrameConfigVersion.cmake"
    VERSION ${PROJECT_VERSION}
    COMPATIBILITY AnyNewerVersion
)

configure_file(
    "${CMAKE_CURRENT_SOURCE_DIR}/cmake/PhantomFrameConfig.cmake.in"
    "${CMAKE_CURRENT_BINARY_DIR}/PhantomFrameConfig.cmake"
    @ONLY
)

install(FILES
    "${CMAKE_CURRENT_BINARY_DIR}/PhantomFrameConfig.cmake"
    "${CMAKE_CURRENT_BINARY_DIR}/PhantomFrameConfigVersion.cmake"
    DESTINATION lib/cmake/PhantomFrame
)

# Print configuration summary
message(STATUS "")
message(STATUS "PhantomFrame Configuration Summary")
message(STATUS "==================================")
message(STATUS "Version: ${PROJECT_VERSION}")
message(STATUS "Build Type: ${CMAKE_BUILD_TYPE}")
message(STATUS "C++ Standard: ${CMAKE_CXX_STANDARD}")
message(STATUS "OpenCV: ${OpenCV_VERSION}")
message(STATUS "FFmpeg: Found")
message(STATUS "CUDA: ${HAVE_CUDA}")
message(STATUS "OpenCL: ${HAVE_OPENCL}")
message(STATUS "Install Prefix: ${CMAKE_INSTALL_PREFIX}")
message(STATUS "")

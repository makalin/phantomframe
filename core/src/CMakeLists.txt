# Create the core library
add_library(phantomframe_core STATIC
    common/utils.cpp
    common/logger.cpp
    common/config.cpp
    encoder/watermark_encoder.cpp
    encoder/frame_processor.cpp
    encoder/quality_analyzer.cpp
    extractor/watermark_extractor.cpp
    extractor/frame_analyzer.cpp
    extractor/pattern_detector.cpp
    vlc_integration/vlc_watermark.cpp
    vlc_integration/encoder_patch.cpp
)

# Set target properties
set_target_properties(phantomframe_core PROPERTIES
    VERSION ${PROJECT_VERSION}
    SOVERSION ${PROJECT_VERSION_MAJOR}
    PUBLIC_HEADER ""
)

# Include directories for the core library
target_include_directories(phantomframe_core PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/../include
    ${CMAKE_CURRENT_SOURCE_DIR}
)

# Link libraries for the core library
target_link_libraries(phantomframe_core
    ${OpenCV_LIBS}
    PkgConfig::FFMPEG
)

# Add CUDA support if available
if(CUDA_FOUND)
    target_compile_definitions(phantomframe_core PRIVATE HAVE_CUDA)
    target_link_libraries(phantomframe_core ${CUDA_LIBRARIES})
    target_include_directories(phantomframe_core PRIVATE ${CUDA_INCLUDE_DIRS})
endif()

# Add OpenCL support if available
if(OpenCL_FOUND)
    target_compile_definitions(phantomframe_core PRIVATE HAVE_OPENCL)
    target_link_libraries(phantomframe_core ${OpenCL_LIBRARIES})
    target_include_directories(phantomframe_core PRIVATE ${OpenCL_INCLUDE_DIRS})
endif()

# Compiler-specific flags
if(CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
    target_compile_options(phantomframe_core PRIVATE
        -fPIC
        -fvisibility=hidden
        -ffunction-sections
        -fdata-sections
    )
    target_link_options(phantomframe_core PRIVATE
        -Wl,--gc-sections
    )
endif()

# Windows-specific settings
if(WIN32)
    target_compile_definitions(phantomframe_core PRIVATE
        WIN32_LEAN_AND_MEAN
        NOMINMAX
        _CRT_SECURE_NO_WARNINGS
    )
endif()

# Install the core library
install(TARGETS phantomframe_core
    EXPORT PhantomFrameTargets
    LIBRARY DESTINATION lib
    ARCHIVE DESTINATION lib
    RUNTIME DESTINATION bin
)

# Export targets
install(EXPORT PhantomFrameTargets
    FILE PhantomFrameTargets.cmake
    NAMESPACE PhantomFrame::
    DESTINATION lib/cmake/PhantomFrame
)

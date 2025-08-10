# Enable testing
enable_testing()

# Find Google Test
find_package(GTest REQUIRED)

# Create test executable
add_executable(phantomframe_tests
    main.cpp
    test_helpers.cpp
    utils_test.cpp
    logger_test.cpp
    config_test.cpp
    watermark_encoder_test.cpp
    frame_processor_test.cpp
    quality_analyzer_test.cpp
    watermark_extractor_test.cpp
    frame_analyzer_test.cpp
    pattern_detector_test.cpp
    vlc_watermark_test.cpp
    encoder_patch_test.cpp
)

# Link libraries for tests
target_link_libraries(phantomframe_tests
    phantomframe_core
    GTest::gtest
    GTest::gtest_main
    ${OpenCV_LIBS}
    PkgConfig::FFMPEG
)

# Include directories for tests
target_include_directories(phantomframe_tests PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/../include
    ${CMAKE_CURRENT_SOURCE_DIR}/../src
    ${CMAKE_CURRENT_SOURCE_DIR}
)

# Add CUDA support if available
if(CUDA_FOUND)
    target_link_libraries(phantomframe_tests ${CUDA_LIBRARIES})
    target_include_directories(phantomframe_tests PRIVATE ${CUDA_INCLUDE_DIRS})
endif()

# Add OpenCL support if available
if(OpenCL_FOUND)
    target_link_libraries(phantomframe_tests ${OpenCL_LIBRARIES})
    target_include_directories(phantomframe_tests PRIVATE ${OpenCL_INCLUDE_DIRS})
endif()

# Add tests to CTest
add_test(NAME PhantomFrameTests COMMAND phantomframe_tests)

# Set test properties
set_tests_properties(PhantomFrameTests PROPERTIES
    TIMEOUT 300
    LABELS "unit"
)

# Create a custom target for running tests
add_custom_target(run_tests
    COMMAND ${CMAKE_CTEST_COMMAND} --verbose
    DEPENDS phantomframe_tests
    COMMENT "Running PhantomFrame tests..."
)

# Install test executable (optional)
install(TARGETS phantomframe_tests
    RUNTIME DESTINATION bin
)

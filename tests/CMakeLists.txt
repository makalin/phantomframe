# PhantomFrame Test Suite CMakeLists.txt

# Enable testing
enable_testing()

# Find required packages for testing
find_package(GTest REQUIRED)
find_package(OpenCV REQUIRED)

# Set test source files
set(TEST_SOURCES
    test_watermark_encoder.cpp
    test_watermark_extractor.cpp
    test_utils.cpp
    test_main.cpp
)

# Set test header files
set(TEST_HEADERS
    test_helpers.h
)

# Create test executable
add_executable(phantomframe_tests ${TEST_SOURCES} ${TEST_HEADERS})

# Link test executable with main library and dependencies
target_link_libraries(phantomframe_tests
    phantomframe_lib
    ${GTEST_LIBRARIES}
    ${GTEST_MAIN_LIBRARIES}
    ${OpenCV_LIBS}
    pthread
)

# Include directories for tests
target_include_directories(phantomframe_tests PRIVATE
    ${CMAKE_SOURCE_DIR}/src
    ${GTEST_INCLUDE_DIRS}
    ${OpenCV_INCLUDE_DIRS}
)

# Compiler flags for tests
target_compile_options(phantomframe_tests PRIVATE
    ${CMAKE_CXX_FLAGS}
    -Wall
    -Wextra
    -Werror
    -O0
    -g
)

# Add tests to CTest
add_test(NAME PhantomFrameTests COMMAND phantomframe_tests)

# Set test properties
set_tests_properties(PhantomFrameTests PROPERTIES
    TIMEOUT 300
    ENVIRONMENT "GTEST_COLOR=1"
)

# Create test data directory
add_custom_command(
    TARGET phantomframe_tests POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E make_directory ${CMAKE_BINARY_DIR}/test_data
    COMMENT "Creating test data directory"
)

# Copy test resources if they exist
if(EXISTS ${CMAKE_SOURCE_DIR}/tests/resources)
    add_custom_command(
        TARGET phantomframe_tests POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_directory
        ${CMAKE_SOURCE_DIR}/tests/resources
        ${CMAKE_BINARY_DIR}/test_data/resources
        COMMENT "Copying test resources"
    )
endif()

# Install test executable (optional)
install(TARGETS phantomframe_tests
    RUNTIME DESTINATION bin
    OPTIONAL
)

# Print test configuration
message(STATUS "Test configuration:")
message(STATUS "  Test executable: phantomframe_tests")
message(STATUS "  Test sources: ${TEST_SOURCES}")
message(STATUS "  GTest version: ${GTEST_VERSION}")
message(STATUS "  OpenCV version: ${OpenCV_VERSION}")

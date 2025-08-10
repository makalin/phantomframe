#ifndef PHANTOMFRAME_TEST_HELPERS_H
#define PHANTOMFRAME_TEST_HELPERS_H

#include <gtest/gtest.h>
#include <opencv2/opencv.hpp>
#include <string>
#include <vector>
#include <random>
#include <filesystem>

namespace phantomframe {
namespace test {

/**
 * @brief Test helper utilities for PhantomFrame tests
 */
class TestHelpers {
public:
    /**
     * @brief Create a random test frame with specified dimensions
     * @param width Frame width
     * @param height Frame height
     * @param channels Number of color channels (1 for grayscale, 3 for RGB)
     * @return Random test frame
     */
    static cv::Mat createRandomFrame(int width, int height, int channels = 3);
    
    /**
     * @brief Create a test frame with specific pattern
     * @param width Frame width
     * @param height Frame height
     * @param pattern Pattern type ("checkerboard", "gradient", "solid")
     * @return Test frame with specified pattern
     */
    static cv::Mat createTestFrame(int width, int height, const std::string& pattern = "checkerboard");
    
    /**
     * @brief Generate random test data
     * @param size Size of data to generate
     * @return Vector of random bytes
     */
    static std::vector<uint8_t> generateRandomData(size_t size);
    
    /**
     * @brief Create a temporary test file
     * @param prefix File prefix
     * @param extension File extension
     * @param content File content
     * @return Path to created temporary file
     */
    static std::string createTempTestFile(const std::string& prefix, 
                                        const std::string& extension,
                                        const std::string& content = "test content");
    
    /**
     * @brief Clean up temporary test files
     * @param file_paths Vector of file paths to clean up
     */
    static void cleanupTempFiles(const std::vector<std::string>& file_paths);
    
    /**
     * @brief Generate a random string
     * @param length Length of string to generate
     * @return Random string
     */
    static std::string generateRandomString(size_t length);
    
    /**
     * @brief Check if two frames are approximately equal
     * @param frame1 First frame
     * @param frame2 Second frame
     * @param tolerance Tolerance for comparison
     * @return True if frames are approximately equal
     */
    static bool framesApproximatelyEqual(const cv::Mat& frame1, 
                                       const cv::Mat& frame2, 
                                       double tolerance = 1.0);
    
    /**
     * @brief Create a test video file (simulated)
     * @param output_path Output path for test video
     * @param num_frames Number of frames to generate
     * @param width Frame width
     * @param height Frame height
     * @return True if successful
     */
    static bool createTestVideo(const std::string& output_path, 
                              int num_frames = 10, 
                              int width = 640, 
                              int height = 480);
    
    /**
     * @brief Get test data directory path
     * @return Path to test data directory
     */
    static std::string getTestDataDir();
    
    /**
     * @brief Ensure test data directory exists
     */
    static void ensureTestDataDir();
    
    /**
     * @brief Generate test configuration
     * @return Test configuration string
     */
    static std::string generateTestConfig();
    
    /**
     * @brief Compare two floating point values with tolerance
     * @param a First value
     * @param b Second value
     * @param tolerance Tolerance for comparison
     * @return True if values are approximately equal
     */
    template<typename T>
    static bool approximatelyEqual(T a, T b, T tolerance = static_cast<T>(0.001)) {
        return std::abs(a - b) <= tolerance;
    }
    
    /**
     * @brief Generate random integer in range
     * @param min Minimum value
     * @param max Maximum value
     * @return Random integer
     */
    static int randomInt(int min, int max);
    
    /**
     * @brief Generate random float in range
     * @param min Minimum value
     * @param max Maximum value
     * @return Random float
     */
    static float randomFloat(float min, float max);
    
    /**
     * @brief Generate random double in range
     * @param min Minimum value
     * @param max Maximum value
     * @return Random double
     */
    static double randomDouble(double min, double max);

private:
    static std::mt19937 rng;
    static bool rng_initialized;
    
    static void initializeRNG();
};

} // namespace test
} // namespace phantomframe

// Convenience macros for common test assertions
#define EXPECT_FRAMES_EQUAL(frame1, frame2) \
    EXPECT_TRUE(phantomframe::test::TestHelpers::framesApproximatelyEqual(frame1, frame2))

#define EXPECT_FRAMES_NOT_EQUAL(frame1, frame2) \
    EXPECT_FALSE(phantomframe::test::TestHelpers::framesApproximatelyEqual(frame1, frame2))

#define EXPECT_APPROX_EQUAL(a, b, tolerance) \
    EXPECT_TRUE(phantomframe::test::TestHelpers::approximatelyEqual(a, b, tolerance))

#define EXPECT_APPROX_EQUAL_FLOAT(a, b) \
    EXPECT_APPROX_EQUAL(a, b, 0.001f)

#define EXPECT_APPROX_EQUAL_DOUBLE(a, b) \
    EXPECT_APPROX_EQUAL(a, b, 0.001)

// Test fixture base class with common setup
class PhantomFrameTestBase : public ::testing::Test {
protected:
    void SetUp() override {
        phantomframe::test::TestHelpers::ensureTestDataDir();
    }
    
    void TearDown() override {
        // Clean up any temporary files created during tests
    }
};

#endif // PHANTOMFRAME_TEST_HELPERS_H

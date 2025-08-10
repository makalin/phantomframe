#include "test_helpers.h"
#include <fstream>
#include <sstream>
#include <iomanip>
#include <algorithm>

namespace phantomframe {
namespace test {

// Static member initialization
std::mt19937 TestHelpers::rng;
bool TestHelpers::rng_initialized = false;

void TestHelpers::initializeRNG() {
    if (!rng_initialized) {
        std::random_device rd;
        rng.seed(rd());
        rng_initialized = true;
    }
}

cv::Mat TestHelpers::createRandomFrame(int width, int height, int channels) {
    initializeRNG();
    
    cv::Mat frame(height, width, CV_8UC(channels));
    std::uniform_int_distribution<int> dist(0, 255);
    
    for (int y = 0; y < height; ++y) {
        for (int x = 0; x < width; ++x) {
            if (channels == 1) {
                frame.at<uint8_t>(y, x) = static_cast<uint8_t>(dist(rng));
            } else if (channels == 3) {
                cv::Vec3b& pixel = frame.at<cv::Vec3b>(y, x);
                pixel[0] = static_cast<uint8_t>(dist(rng)); // B
                pixel[1] = static_cast<uint8_t>(dist(rng)); // G
                pixel[2] = static_cast<uint8_t>(dist(rng)); // R
            }
        }
    }
    
    return frame;
}

cv::Mat TestHelpers::createTestFrame(int width, int height, const std::string& pattern) {
    cv::Mat frame(height, width, CV_8UC3);
    
    if (pattern == "checkerboard") {
        int block_size = 32;
        for (int y = 0; y < height; ++y) {
            for (int x = 0; x < width; ++x) {
                int block_x = x / block_size;
                int block_y = y / block_size;
                bool is_white = (block_x + block_y) % 2 == 0;
                
                cv::Vec3b& pixel = frame.at<cv::Vec3b>(y, x);
                uint8_t value = is_white ? 255 : 0;
                pixel[0] = pixel[1] = pixel[2] = value;
            }
        }
    } else if (pattern == "gradient") {
        for (int y = 0; y < height; ++y) {
            for (int x = 0; x < width; ++x) {
                cv::Vec3b& pixel = frame.at<cv::Vec3b>(y, x);
                uint8_t value_x = static_cast<uint8_t>((x * 255) / width);
                uint8_t value_y = static_cast<uint8_t>((y * 255) / height);
                
                pixel[0] = value_x;  // B
                pixel[1] = value_y;  // G
                pixel[2] = (value_x + value_y) / 2;  // R
            }
        }
    } else if (pattern == "solid") {
        frame.setTo(cv::Scalar(128, 128, 128));
    } else {
        // Default to random
        frame = createRandomFrame(width, height, 3);
    }
    
    return frame;
}

std::vector<uint8_t> TestHelpers::generateRandomData(size_t size) {
    initializeRNG();
    
    std::vector<uint8_t> data(size);
    std::uniform_int_distribution<int> dist(0, 255);
    
    for (size_t i = 0; i < size; ++i) {
        data[i] = static_cast<uint8_t>(dist(rng));
    }
    
    return data;
}

std::string TestHelpers::createTempTestFile(const std::string& prefix, 
                                           const std::string& extension,
                                           const std::string& content) {
    std::string temp_dir = std::filesystem::temp_directory_path().string();
    std::string filename = prefix + "_" + std::to_string(std::time(nullptr)) + extension;
    std::string filepath = (std::filesystem::path(temp_dir) / filename).string();
    
    std::ofstream file(filepath);
    if (file.is_open()) {
        file << content;
        file.close();
        return filepath;
    }
    
    return "";
}

void TestHelpers::cleanupTempFiles(const std::vector<std::string>& file_paths) {
    for (const std::string& path : file_paths) {
        if (std::filesystem::exists(path)) {
            std::filesystem::remove(path);
        }
    }
}

std::string TestHelpers::generateRandomString(size_t length) {
    initializeRNG();
    
    static const char charset[] = "0123456789"
                                 "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
                                 "abcdefghijklmnopqrstuvwxyz";
    
    std::string result;
    result.reserve(length);
    
    std::uniform_int_distribution<int> dist(0, sizeof(charset) - 2);
    
    for (size_t i = 0; i < length; ++i) {
        result += charset[dist(rng)];
    }
    
    return result;
}

bool TestHelpers::framesApproximatelyEqual(const cv::Mat& frame1, 
                                         const cv::Mat& frame2, 
                                         double tolerance) {
    if (frame1.size() != frame2.size() || frame1.type() != frame2.type()) {
        return false;
    }
    
    cv::Mat diff;
    cv::absdiff(frame1, frame2, diff);
    
    double max_diff = 0.0;
    cv::minMaxLoc(diff, nullptr, &max_diff);
    
    return max_diff <= tolerance;
}

bool TestHelpers::createTestVideo(const std::string& output_path, 
                                int num_frames, 
                                int width, 
                                int height) {
    // This is a simplified implementation that creates a text file
    // In a real implementation, you would use OpenCV's VideoWriter
    
    std::ofstream file(output_path);
    if (!file.is_open()) {
        return false;
    }
    
    file << "Test video file (simulated)" << std::endl;
    file << "Frames: " << num_frames << std::endl;
    file << "Resolution: " << width << "x" << height << std::endl;
    file << "Format: PhantomFrame test format" << std::endl;
    
    for (int i = 0; i < num_frames; ++i) {
        file << "Frame " << i << ": " << generateRandomString(16) << std::endl;
    }
    
    file.close();
    return true;
}

std::string TestHelpers::getTestDataDir() {
    std::string current_dir = std::filesystem::current_path().string();
    return (std::filesystem::path(current_dir) / "tests" / "test_data").string();
}

void TestHelpers::ensureTestDataDir() {
    std::string test_data_dir = getTestDataDir();
    if (!std::filesystem::exists(test_data_dir)) {
        std::filesystem::create_directories(test_data_dir);
    }
}

std::string TestHelpers::generateTestConfig() {
    std::ostringstream config;
    config << "test_config_" << std::time(nullptr) << ".json";
    return config.str();
}

int TestHelpers::randomInt(int min, int max) {
    initializeRNG();
    std::uniform_int_distribution<int> dist(min, max);
    return dist(rng);
}

float TestHelpers::randomFloat(float min, float max) {
    initializeRNG();
    std::uniform_real_distribution<float> dist(min, max);
    return dist(rng);
}

double TestHelpers::randomDouble(double min, double max) {
    initializeRNG();
    std::uniform_real_distribution<double> dist(min, max);
    return dist(rng);
}

} // namespace test
} // namespace phantomframe

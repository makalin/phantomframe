#include <gtest/gtest.h>
#include "common/utils.h"
#include <fstream>
#include <filesystem>

using namespace phantomframe::utils;

class UtilsTest : public ::testing::Test {
protected:
    void SetUp() override {
        // Create a temporary test file
        test_file_path = "/tmp/phantomframe_test_file.txt";
        std::ofstream test_file(test_file_path);
        test_file << "Test content for PhantomFrame";
        test_file.close();
    }
    
    void TearDown() override {
        // Clean up test file
        if (std::filesystem::exists(test_file_path)) {
            std::filesystem::remove(test_file_path);
        }
    }
    
    std::string test_file_path;
};

TEST_F(UtilsTest, GenerateRandomSeedTest) {
    uint32_t seed1 = generateRandomSeed();
    uint32_t seed2 = generateRandomSeed();
    
    // Seeds should be different (very unlikely to be the same)
    EXPECT_NE(seed1, seed2);
    
    // Seeds should be within reasonable range
    EXPECT_GT(seed1, 0);
    EXPECT_GT(seed2, 0);
}

TEST_F(UtilsTest, StringToHexTest) {
    std::string test_string = "Hello, PhantomFrame!";
    std::string hex_result = stringToHex(test_string);
    
    EXPECT_FALSE(hex_result.empty());
    EXPECT_EQ(hex_result.length(), test_string.length() * 2);
    
    // Verify it's valid hex
    for (char c : hex_result) {
        EXPECT_TRUE((c >= '0' && c <= '9') || (c >= 'a' && c <= 'f'));
    }
}

TEST_F(UtilsTest, HexToStringTest) {
    std::string original = "PhantomFrame";
    std::string hex = stringToHex(original);
    std::string result = hexToString(hex);
    
    EXPECT_EQ(result, original);
}

TEST_F(UtilsTest, HexToStringWithInvalidHex) {
    std::string invalid_hex = "invalid_hex_string";
    std::string result = hexToString(invalid_hex);
    
    // Should handle invalid hex gracefully
    EXPECT_TRUE(result.empty() || result != invalid_hex);
}

TEST_F(UtilsTest, StringHashTest) {
    std::string test_string = "Test string for hashing";
    uint32_t hash1 = stringHash(test_string);
    uint32_t hash2 = stringHash(test_string);
    
    // Same string should produce same hash
    EXPECT_EQ(hash1, hash2);
    
    // Different strings should produce different hashes
    uint32_t hash3 = stringHash("Different string");
    EXPECT_NE(hash1, hash3);
    
    // Hash should be non-zero
    EXPECT_GT(hash1, 0);
}

TEST_F(UtilsTest, XorEncryptTest) {
    std::string original = "Secret message";
    std::string key = "secretkey";
    
    std::string encrypted = xorEncrypt(original, key);
    std::string decrypted = xorEncrypt(encrypted, key);
    
    // XOR encryption should be reversible
    EXPECT_EQ(decrypted, original);
    
    // Encrypted should be different from original
    EXPECT_NE(encrypted, original);
}

TEST_F(UtilsTest, XorEncryptWithEmptyKey) {
    std::string original = "Test message";
    std::string empty_key = "";
    
    std::string result = xorEncrypt(original, empty_key);
    
    // Should handle empty key gracefully
    EXPECT_EQ(result, original);
}

TEST_F(UtilsTest, XorEncryptWithEmptyMessage) {
    std::string empty_message = "";
    std::string key = "testkey";
    
    std::string result = xorEncrypt(empty_message, key);
    
    // Should handle empty message gracefully
    EXPECT_EQ(result, empty_message);
}

TEST_F(UtilsTest, ValidateVideoFileTest) {
    // Test with valid video file extension
    EXPECT_TRUE(validateVideoFile("test.mp4"));
    EXPECT_TRUE(validateVideoFile("test.avi"));
    EXPECT_TRUE(validateVideoFile("test.mkv"));
    EXPECT_TRUE(validateVideoFile("test.mov"));
    EXPECT_TRUE(validateVideoFile("test.webm"));
    
    // Test with invalid extensions
    EXPECT_FALSE(validateVideoFile("test.txt"));
    EXPECT_FALSE(validateVideoFile("test.jpg"));
    EXPECT_FALSE(validateVideoFile("test.pdf"));
    EXPECT_FALSE(validateVideoFile("test"));
    EXPECT_FALSE(validateVideoFile(""));
}

TEST_F(UtilsTest, ValidateVideoFileWithPath) {
    // Test with full paths
    EXPECT_TRUE(validateVideoFile("/path/to/video.mp4"));
    EXPECT_TRUE(validateVideoFile("C:\\Videos\\test.avi"));
    EXPECT_FALSE(validateVideoFile("/path/to/document.pdf"));
}

TEST_F(UtilsTest, GetVideoInfoTest) {
    // Test with existing file
    VideoInfo info = getVideoInfo(test_file_path);
    
    // Should return basic info even for non-video files
    EXPECT_FALSE(info.path.empty());
    EXPECT_GT(info.size_bytes, 0);
    
    // Test with non-existent file
    VideoInfo non_existent = getVideoInfo("/nonexistent/file.mp4");
    EXPECT_EQ(non_existent.size_bytes, 0);
    EXPECT_TRUE(non_existent.path.empty());
}

TEST_F(UtilsTest, CreateTempFileTest) {
    std::string temp_path = createTempFile("phantomframe_test", ".tmp");
    
    EXPECT_FALSE(temp_path.empty());
    EXPECT_TRUE(std::filesystem::exists(temp_path));
    
    // Clean up
    std::filesystem::remove(temp_path);
}

TEST_F(UtilsTest, CreateTempFileWithCustomExtension) {
    std::string temp_path = createTempFile("test", ".mp4");
    
    EXPECT_FALSE(temp_path.empty());
    EXPECT_TRUE(std::filesystem::exists(temp_path));
    EXPECT_TRUE(temp_path.find(".mp4") != std::string::npos);
    
    // Clean up
    std::filesystem::remove(temp_path);
}

TEST_F(UtilsTest, FormatFileSizeTest) {
    // Test various file sizes
    EXPECT_EQ(formatFileSize(0), "0 B");
    EXPECT_EQ(formatFileSize(1024), "1.0 KB");
    EXPECT_EQ(formatFileSize(1024 * 1024), "1.0 MB");
    EXPECT_EQ(formatFileSize(1024 * 1024 * 1024), "1.0 GB");
    
    // Test with bytes less than 1 KB
    EXPECT_EQ(formatFileSize(512), "512 B");
    
    // Test with fractional sizes
    EXPECT_EQ(formatFileSize(1536), "1.5 KB");
    EXPECT_EQ(formatFileSize(1024 * 1024 * 1.5), "1.5 MB");
}

TEST_F(UtilsTest, FormatFileSizeWithLargeValues) {
    // Test with very large values
    uint64_t large_size = 1024ULL * 1024 * 1024 * 1024; // 1 TB
    std::string result = formatFileSize(large_size);
    EXPECT_EQ(result, "1.0 TB");
    
    // Test with multiple terabytes
    uint64_t multiple_tb = 1024ULL * 1024 * 1024 * 1024 * 5; // 5 TB
    result = formatFileSize(multiple_tb);
    EXPECT_EQ(result, "5.0 TB");
}

TEST_F(UtilsTest, GetCurrentTimestampTest) {
    std::string timestamp1 = getCurrentTimestamp();
    std::string timestamp2 = getCurrentTimestamp();
    
    EXPECT_FALSE(timestamp1.empty());
    EXPECT_FALSE(timestamp2.empty());
    
    // Timestamps should be different (unless called very quickly)
    // This test might occasionally fail on very fast systems
    // EXPECT_NE(timestamp1, timestamp2);
    
    // Verify timestamp format (should contain date and time)
    EXPECT_NE(timestamp1.find("-"), std::string::npos);
    EXPECT_NE(timestamp1.find(":"), std::string::npos);
}

TEST_F(UtilsTest, GetElapsedTimeTest) {
    auto start_time = std::chrono::steady_clock::now();
    
    // Simulate some work
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
    
    auto end_time = std::chrono::steady_clock::now();
    double elapsed = getElapsedTime(start_time, end_time);
    
    EXPECT_GT(elapsed, 0.0);
    EXPECT_LT(elapsed, 1.0); // Should be less than 1 second
}

TEST_F(UtilsTest, GetElapsedTimeWithSameTime) {
    auto time = std::chrono::steady_clock::now();
    double elapsed = getElapsedTime(time, time);
    
    EXPECT_EQ(elapsed, 0.0);
}

TEST_F(UtilsTest, StringToHexRoundTrip) {
    std::vector<std::string> test_strings = {
        "",
        "a",
        "Hello",
        "PhantomFrame",
        "Special chars: !@#$%^&*()",
        "Unicode: 🎥📹🎬",
        "Numbers: 1234567890"
    };
    
    for (const std::string& original : test_strings) {
        std::string hex = stringToHex(original);
        std::string result = hexToString(hex);
        EXPECT_EQ(result, original) << "Failed for: " << original;
    }
}

TEST_F(UtilsTest, XorEncryptRoundTrip) {
    std::vector<std::string> test_strings = {
        "",
        "a",
        "Hello World",
        "PhantomFrame is awesome!",
        "Special: !@#$%^&*()_+",
        "Numbers: 0123456789"
    };
    
    std::vector<std::string> test_keys = {
        "",
        "a",
        "secret",
        "PhantomFrameKey",
        "SpecialKey!@#",
        "1234567890"
    };
    
    for (const std::string& original : test_strings) {
        for (const std::string& key : test_keys) {
            std::string encrypted = xorEncrypt(original, key);
            std::string decrypted = xorEncrypt(encrypted, key);
            EXPECT_EQ(decrypted, original) 
                << "Failed for original: '" << original << "', key: '" << key << "'";
        }
    }
}

TEST_F(UtilsTest, HashConsistency) {
    std::string test_string = "PhantomFrame Test String";
    uint32_t hash1 = stringHash(test_string);
    
    // Hash should be consistent across multiple calls
    for (int i = 0; i < 100; ++i) {
        uint32_t hash = stringHash(test_string);
        EXPECT_EQ(hash, hash1);
    }
}

TEST_F(UtilsTest, HashDistribution) {
    std::vector<uint32_t> hashes;
    std::vector<std::string> test_strings = {
        "string1", "string2", "string3", "string4", "string5",
        "different1", "different2", "different3", "different4", "different5"
    };
    
    for (const std::string& str : test_strings) {
        hashes.push_back(stringHash(str));
    }
    
    // Check that we have some different hashes (collisions are possible but unlikely)
    bool has_different_hashes = false;
    for (size_t i = 0; i < hashes.size(); ++i) {
        for (size_t j = i + 1; j < hashes.size(); ++j) {
            if (hashes[i] != hashes[j]) {
                has_different_hashes = true;
                break;
            }
        }
        if (has_different_hashes) break;
    }
    
    // This test might occasionally fail due to hash collisions
    // but it's very unlikely with good hash functions
    EXPECT_TRUE(has_different_hashes);
}

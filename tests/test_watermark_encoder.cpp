#include <gtest/gtest.h>
#include <vector>
#include <cstring>
#include "encoder/watermark_encoder.h"

using namespace phantomframe;

class WatermarkEncoderTest : public ::testing::Test {
protected:
    void SetUp() override {
        // Default configuration for testing
        config.payload[0] = 0x01;
        config.payload[1] = 0x23;
        config.payload[2] = 0x45;
        config.payload[3] = 0x67;
        config.payload[4] = 0x89;
        config.payload[5] = 0xAB;
        config.payload[6] = 0xCD;
        config.payload[7] = 0xEF;
        config.payload[8] = 0xFE;
        config.payload[9] = 0xDC;
        config.payload[10] = 0xBA;
        config.payload[11] = 0x98;
        config.payload[12] = 0x76;
        config.payload[13] = 0x54;
        config.payload[14] = 0x32;
        config.payload[15] = 0x10;
        config.seed = 12345;
        config.block_density = 0.3f;
        config.temporal_period = 30;
        config.adaptive_embedding = false;
        config.quality_threshold = 0.8f;
    }

    WatermarkConfig config;
    static constexpr uint32_t TEST_WIDTH = 64;
    static constexpr uint32_t TEST_HEIGHT = 64;
    static constexpr float TEST_FPS = 30.0f;
};

TEST_F(WatermarkEncoderTest, ConstructorTest) {
    WatermarkEncoder encoder(config);
    EXPECT_TRUE(true); // Constructor should not throw
}

TEST_F(WatermarkEncoderTest, InitializationTest) {
    WatermarkEncoder encoder(config);
    bool result = encoder.initialize(TEST_WIDTH, TEST_HEIGHT, TEST_FPS);
    EXPECT_TRUE(result);
}

TEST_F(WatermarkEncoderTest, InitializationWithZeroDimensions) {
    WatermarkEncoder encoder(config);
    bool result = encoder.initialize(0, 0, TEST_FPS);
    EXPECT_FALSE(result);
}

TEST_F(WatermarkEncoderTest, InitializationWithNegativeFPS) {
    WatermarkEncoder encoder(config);
    bool result = encoder.initialize(TEST_WIDTH, TEST_HEIGHT, -1.0f);
    EXPECT_FALSE(result);
}

TEST_F(WatermarkEncoderTest, ProcessFrameTest) {
    WatermarkEncoder encoder(config);
    ASSERT_TRUE(encoder.initialize(TEST_WIDTH, TEST_HEIGHT, TEST_FPS));

    // Create test frame data
    std::vector<uint8_t> frame_data(TEST_WIDTH * TEST_HEIGHT * 3, 128);
    
    // Process frame
    auto result = encoder.processFrame(frame_data.data(), frame_data.size(), 0);
    
    // Check that result is not empty
    EXPECT_FALSE(result.empty());
    
    // Check that result size matches input size
    EXPECT_EQ(result.size(), frame_data.size());
}

TEST_F(WatermarkEncoderTest, ProcessFrameWithNullData) {
    WatermarkEncoder encoder(config);
    ASSERT_TRUE(encoder.initialize(TEST_WIDTH, TEST_HEIGHT, TEST_FPS));

    // Process frame with null data
    auto result = encoder.processFrame(nullptr, TEST_WIDTH * TEST_HEIGHT * 3, 0);
    
    // Should handle null data gracefully
    EXPECT_TRUE(result.empty());
}

TEST_F(WatermarkEncoderTest, ProcessFrameWithZeroSize) {
    WatermarkEncoder encoder(config);
    ASSERT_TRUE(encoder.initialize(TEST_WIDTH, TEST_HEIGHT, TEST_FPS));

    // Create test frame data
    std::vector<uint8_t> frame_data(TEST_WIDTH * TEST_HEIGHT * 3, 128);
    
    // Process frame with zero size
    auto result = encoder.processFrame(frame_data.data(), 0, 0);
    
    // Should handle zero size gracefully
    EXPECT_TRUE(result.empty());
}

TEST_F(WatermarkEncoderTest, GetBlocksForFrameTest) {
    WatermarkEncoder encoder(config);
    ASSERT_TRUE(encoder.initialize(TEST_WIDTH, TEST_HEIGHT, TEST_FPS));

    // Get blocks for frame 0
    auto blocks = encoder.getBlocksForFrame(0);
    
    // Should return some blocks
    EXPECT_FALSE(blocks.empty());
    
    // Check block properties
    for (const auto& block : blocks) {
        EXPECT_GE(block.x, 0);
        EXPECT_GE(block.y, 0);
        EXPECT_LT(block.x, TEST_WIDTH);
        EXPECT_LT(block.y, TEST_HEIGHT);
        EXPECT_GE(block.qp_delta, -1);
        EXPECT_LE(block.qp_delta, 1);
    }
}

TEST_F(WatermarkEncoderTest, GetBlocksForFrameConsistency) {
    WatermarkEncoder encoder(config);
    ASSERT_TRUE(encoder.initialize(TEST_WIDTH, TEST_HEIGHT, TEST_FPS));

    // Get blocks for same frame multiple times
    auto blocks1 = encoder.getBlocksForFrame(0);
    auto blocks2 = encoder.getBlocksForFrame(0);
    
    // Should return same blocks for same frame
    EXPECT_EQ(blocks1.size(), blocks2.size());
    
    // Check that blocks are identical
    for (size_t i = 0; i < blocks1.size(); ++i) {
        EXPECT_EQ(blocks1[i].x, blocks2[i].x);
        EXPECT_EQ(blocks1[i].y, blocks2[i].y);
        EXPECT_EQ(blocks1[i].qp_delta, blocks2[i].qp_delta);
    }
}

TEST_F(WatermarkEncoderTest, UpdateConfigTest) {
    WatermarkEncoder encoder(config);
    ASSERT_TRUE(encoder.initialize(TEST_WIDTH, TEST_HEIGHT, TEST_FPS));

    // Update configuration
    WatermarkConfig new_config = config;
    new_config.seed = 54321;
    new_config.block_density = 0.5f;
    
    encoder.updateConfig(new_config);
    
    // Get blocks with new configuration
    auto blocks = encoder.getBlocksForFrame(0);
    
    // Should still return blocks
    EXPECT_FALSE(blocks.empty());
}

TEST_F(WatermarkEncoderTest, GetStatsTest) {
    WatermarkEncoder encoder(config);
    ASSERT_TRUE(encoder.initialize(TEST_WIDTH, TEST_HEIGHT, TEST_FPS));

    // Get statistics
    std::string stats = encoder.getStats();
    
    // Should return non-empty string
    EXPECT_FALSE(stats.empty());
    
    // Should contain some expected information
    EXPECT_NE(stats.find("WatermarkEncoder"), std::string::npos);
}

TEST_F(WatermarkEncoderTest, MultipleFrameProcessing) {
    WatermarkEncoder encoder(config);
    ASSERT_TRUE(encoder.initialize(TEST_WIDTH, TEST_HEIGHT, TEST_FPS));

    // Create test frame data
    std::vector<uint8_t> frame_data(TEST_WIDTH * TEST_HEIGHT * 3, 128);
    
    // Process multiple frames
    for (uint32_t i = 0; i < 10; ++i) {
        auto result = encoder.processFrame(frame_data.data(), frame_data.size(), i);
        EXPECT_FALSE(result.empty());
        EXPECT_EQ(result.size(), frame_data.size());
    }
}

TEST_F(WatermarkEncoderTest, BlockDensityEffect) {
    // Test with different block densities
    std::vector<float> densities = {0.1f, 0.3f, 0.5f, 0.7f, 0.9f};
    
    for (float density : densities) {
        WatermarkConfig test_config = config;
        test_config.block_density = density;
        
        WatermarkEncoder encoder(test_config);
        ASSERT_TRUE(encoder.initialize(TEST_WIDTH, TEST_HEIGHT, TEST_FPS));
        
        auto blocks = encoder.getBlocksForFrame(0);
        
        // Higher density should generally result in more blocks
        // (though this depends on the specific implementation)
        EXPECT_FALSE(blocks.empty());
    }
}

TEST_F(WatermarkEncoderTest, SeedEffect) {
    // Test with different seeds
    std::vector<uint32_t> seeds = {0, 12345, 54321, 99999, 0xFFFFFFFF};
    
    for (uint32_t seed : seeds) {
        WatermarkConfig test_config = config;
        test_config.seed = seed;
        
        WatermarkEncoder encoder(test_config);
        ASSERT_TRUE(encoder.initialize(TEST_WIDTH, TEST_HEIGHT, TEST_FPS));
        
        auto blocks = encoder.getBlocksForFrame(0);
        
        // Should always return some blocks
        EXPECT_FALSE(blocks.empty());
    }
}

TEST_F(WatermarkEncoderTest, AdaptiveEmbeddingTest) {
    // Test adaptive embedding
    WatermarkConfig test_config = config;
    test_config.adaptive_embedding = true;
    test_config.quality_threshold = 0.7f;
    
    WatermarkEncoder encoder(test_config);
    ASSERT_TRUE(encoder.initialize(TEST_WIDTH, TEST_HEIGHT, TEST_FPS));
    
    // Create test frame with varying quality
    std::vector<uint8_t> frame_data(TEST_WIDTH * TEST_HEIGHT * 3);
    for (size_t i = 0; i < frame_data.size(); ++i) {
        frame_data[i] = static_cast<uint8_t>(i % 256);
    }
    
    auto result = encoder.processFrame(frame_data.data(), frame_data.size(), 0);
    EXPECT_FALSE(result.empty());
}

TEST_F(WatermarkEncoderTest, TemporalPeriodTest) {
    // Test different temporal periods
    std::vector<uint32_t> periods = {1, 15, 30, 60, 120};
    
    for (uint32_t period : periods) {
        WatermarkConfig test_config = config;
        test_config.temporal_period = period;
        
        WatermarkEncoder encoder(test_config);
        ASSERT_TRUE(encoder.initialize(TEST_WIDTH, TEST_HEIGHT, TEST_FPS));
        
        // Process frames and check temporal behavior
        for (uint32_t i = 0; i < period * 2; ++i) {
            std::vector<uint8_t> frame_data(TEST_WIDTH * TEST_HEIGHT * 3, 128);
            auto result = encoder.processFrame(frame_data.data(), frame_data.size(), i);
            EXPECT_FALSE(result.empty());
        }
    }
}

TEST_F(WatermarkEncoderTest, MemoryLeakTest) {
    // Test for memory leaks by creating and destroying many encoders
    for (int i = 0; i < 100; ++i) {
        WatermarkEncoder encoder(config);
        ASSERT_TRUE(encoder.initialize(TEST_WIDTH, TEST_HEIGHT, TEST_FPS));
        
        std::vector<uint8_t> frame_data(TEST_WIDTH * TEST_HEIGHT * 3, 128);
        auto result = encoder.processFrame(frame_data.data(), frame_data.size(), 0);
        EXPECT_FALSE(result.empty());
    }
    
    // If we get here without crashes, memory management is working
    EXPECT_TRUE(true);
}

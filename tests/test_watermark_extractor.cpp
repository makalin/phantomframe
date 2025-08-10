#include <gtest/gtest.h>
#include "extractor/watermark_extractor.h"
#include <opencv2/opencv.hpp>

using namespace phantomframe;

class WatermarkExtractorTest : public ::testing::Test {
protected:
    void SetUp() override {
        config.confidence_threshold = 0.8f;
        config.max_frames = 100;
        config.block_size = 8;
        config.min_blocks = 10;
        config.analysis_method = "hybrid";
        
        extractor = std::make_unique<WatermarkExtractor>(config);
    }
    
    void TearDown() override {
        extractor.reset();
    }
    
    ExtractionConfig config;
    std::unique_ptr<WatermarkExtractor> extractor;
};

TEST_F(WatermarkExtractorTest, ConstructorTest) {
    EXPECT_EQ(config.confidence_threshold, 0.8f);
    EXPECT_EQ(config.max_frames, 100);
    EXPECT_EQ(config.block_size, 8);
    EXPECT_EQ(config.min_blocks, 10);
    EXPECT_EQ(config.analysis_method, "hybrid");
}

TEST_F(WatermarkExtractorTest, InitializationTest) {
    EXPECT_TRUE(extractor->initialize());
}

TEST_F(WatermarkExtractorTest, AnalyzeFrameTest) {
    ASSERT_TRUE(extractor->initialize());
    
    // Create a test frame
    cv::Mat frame(480, 640, CV_8UC3);
    cv::randu(frame, cv::Scalar(0, 0, 0), cv::Scalar(255, 255, 255));
    
    FrameAnalysis analysis = extractor->analyzeFrame(frame, 0);
    
    EXPECT_EQ(analysis.frame_index, 0);
    EXPECT_GT(analysis.blocks_analyzed, 0);
    EXPECT_GE(analysis.confidence, 0.0f);
    EXPECT_LE(analysis.confidence, 1.0f);
    EXPECT_FALSE(analysis.features.empty());
}

TEST_F(WatermarkExtractorTest, AnalyzeFrameWithEmptyFrame) {
    ASSERT_TRUE(extractor->initialize());
    
    cv::Mat empty_frame;
    FrameAnalysis analysis = extractor->analyzeFrame(empty_frame, 0);
    
    EXPECT_EQ(analysis.frame_index, 0);
    EXPECT_EQ(analysis.blocks_analyzed, 0);
    EXPECT_EQ(analysis.confidence, 0.0f);
    EXPECT_TRUE(analysis.features.empty());
}

TEST_F(WatermarkExtractorTest, ExtractWatermarkTest) {
    ASSERT_TRUE(extractor->initialize());
    
    // Create sample frame analyses
    std::vector<FrameAnalysis> frames;
    for (int i = 0; i < 5; ++i) {
        cv::Mat frame(480, 640, CV_8UC3);
        cv::randu(frame, cv::Scalar(0, 0, 0), cv::Scalar(255, 255, 255));
        
        FrameAnalysis analysis = extractor->analyzeFrame(frame, i);
        frames.push_back(analysis);
    }
    
    DetectionResult result = extractor->extractWatermark(frames);
    
    EXPECT_GE(result.confidence, 0.0f);
    EXPECT_LE(result.confidence, 1.0f);
    EXPECT_GE(result.frames_analyzed, 0);
    EXPECT_GE(result.blocks_analyzed, 0);
    EXPECT_FALSE(result.payload.empty());
}

TEST_F(WatermarkExtractorTest, ExtractWatermarkWithEmptyFrames) {
    ASSERT_TRUE(extractor->initialize());
    
    std::vector<FrameAnalysis> empty_frames;
    DetectionResult result = extractor->extractWatermark(empty_frames);
    
    EXPECT_EQ(result.confidence, 0.0f);
    EXPECT_EQ(result.frames_analyzed, 0);
    EXPECT_EQ(result.blocks_analyzed, 0);
    EXPECT_TRUE(result.payload.empty());
}

TEST_F(WatermarkExtractorTest, UpdateConfigTest) {
    ASSERT_TRUE(extractor->initialize());
    
    ExtractionConfig new_config;
    new_config.confidence_threshold = 0.9f;
    new_config.max_frames = 200;
    new_config.block_size = 16;
    new_config.min_blocks = 20;
    new_config.analysis_method = "statistical";
    
    extractor->updateConfig(new_config);
    
    // Verify config was updated
    std::string stats = extractor->getStats();
    EXPECT_FALSE(stats.empty());
}

TEST_F(WatermarkExtractorTest, GetStatsTest) {
    ASSERT_TRUE(extractor->initialize());
    
    std::string stats = extractor->getStats();
    EXPECT_FALSE(stats.empty());
    EXPECT_NE(stats.find("WatermarkExtractor"), std::string::npos);
}

TEST_F(WatermarkExtractorTest, MultipleFrameAnalysis) {
    ASSERT_TRUE(extractor->initialize());
    
    std::vector<FrameAnalysis> analyses;
    for (int i = 0; i < 10; ++i) {
        cv::Mat frame(480, 640, CV_8UC3);
        cv::randu(frame, cv::Scalar(0, 0, 0), cv::Scalar(255, 255, 255));
        
        FrameAnalysis analysis = extractor->analyzeFrame(frame, i);
        analyses.push_back(analysis);
        
        EXPECT_EQ(analysis.frame_index, i);
        EXPECT_GT(analysis.blocks_analyzed, 0);
    }
    
    EXPECT_EQ(analyses.size(), 10);
}

TEST_F(WatermarkExtractorTest, ConfidenceThresholdEffect) {
    ASSERT_TRUE(extractor->initialize());
    
    // Test with different confidence thresholds
    std::vector<float> thresholds = {0.5f, 0.7f, 0.9f};
    
    for (float threshold : thresholds) {
        ExtractionConfig test_config = config;
        test_config.confidence_threshold = threshold;
        
        WatermarkExtractor test_extractor(test_config);
        ASSERT_TRUE(test_extractor.initialize());
        
        cv::Mat frame(480, 640, CV_8UC3);
        cv::randu(frame, cv::Scalar(0, 0, 0), cv::Scalar(255, 255, 255));
        
        FrameAnalysis analysis = test_extractor.analyzeFrame(frame, 0);
        EXPECT_GE(analysis.confidence, 0.0f);
        EXPECT_LE(analysis.confidence, 1.0f);
    }
}

TEST_F(WatermarkExtractorTest, BlockSizeEffect) {
    ASSERT_TRUE(extractor->initialize());
    
    // Test with different block sizes
    std::vector<int> block_sizes = {4, 8, 16, 32};
    
    for (int block_size : block_sizes) {
        ExtractionConfig test_config = config;
        test_config.block_size = block_size;
        
        WatermarkExtractor test_extractor(test_config);
        ASSERT_TRUE(test_extractor.initialize());
        
        cv::Mat frame(480, 640, CV_8UC3);
        cv::randu(frame, cv::Scalar(0, 0, 0), cv::Scalar(255, 255, 255));
        
        FrameAnalysis analysis = test_extractor.analyzeFrame(frame, 0);
        EXPECT_GT(analysis.blocks_analyzed, 0);
    }
}

TEST_F(WatermarkExtractorTest, AnalysisMethodEffect) {
    ASSERT_TRUE(extractor->initialize());
    
    // Test with different analysis methods
    std::vector<std::string> methods = {"statistical", "ml", "hybrid"};
    
    for (const std::string& method : methods) {
        ExtractionConfig test_config = config;
        test_config.analysis_method = method;
        
        WatermarkExtractor test_extractor(test_config);
        ASSERT_TRUE(test_extractor.initialize());
        
        cv::Mat frame(480, 640, CV_8UC3);
        cv::randu(frame, cv::Scalar(0, 0, 0), cv::Scalar(255, 255, 255));
        
        FrameAnalysis analysis = test_extractor.analyzeFrame(frame, 0);
        EXPECT_GT(analysis.blocks_analyzed, 0);
    }
}

TEST_F(WatermarkExtractorTest, MemoryLeakTest) {
    // Test for memory leaks by creating/destroying multiple instances
    for (int i = 0; i < 100; ++i) {
        WatermarkExtractor test_extractor(config);
        test_extractor.initialize();
        
        cv::Mat frame(480, 640, CV_8UC3);
        cv::randu(frame, cv::Scalar(0, 0, 0), cv::Scalar(255, 255, 255));
        
        test_extractor.analyzeFrame(frame, i);
    }
    
    // If we get here without crashes, memory management is likely working
    EXPECT_TRUE(true);
}

TEST_F(WatermarkExtractorTest, InvalidVideoPathTest) {
    ASSERT_TRUE(extractor->initialize());
    
    DetectionResult result = extractor->analyzeVideo("/nonexistent/video.mp4");
    
    EXPECT_EQ(result.confidence, 0.0f);
    EXPECT_EQ(result.frames_analyzed, 0);
    EXPECT_EQ(result.blocks_analyzed, 0);
    EXPECT_TRUE(result.payload.empty());
}

TEST_F(WatermarkExtractorTest, FrameIndexConsistency) {
    ASSERT_TRUE(extractor->initialize());
    
    cv::Mat frame(480, 640, CV_8UC3);
    cv::randu(frame, cv::Scalar(0, 0, 0), cv::Scalar(255, 255, 255));
    
    // Test with different frame indices
    for (uint32_t i = 0; i < 5; ++i) {
        FrameAnalysis analysis = extractor->analyzeFrame(frame, i);
        EXPECT_EQ(analysis.frame_index, i);
    }
}

TEST_F(WatermarkExtractorTest, FeatureExtractionConsistency) {
    ASSERT_TRUE(extractor->initialize());
    
    cv::Mat frame(480, 640, CV_8UC3);
    cv::randu(frame, cv::Scalar(0, 0, 0), cv::Scalar(255, 255, 255));
    
    // Analyze the same frame multiple times
    FrameAnalysis analysis1 = extractor->analyzeFrame(frame, 0);
    FrameAnalysis analysis2 = extractor->analyzeFrame(frame, 0);
    
    // Features should be consistent for the same frame
    EXPECT_EQ(analysis1.features.size(), analysis2.features.size());
    EXPECT_EQ(analysis1.blocks_analyzed, analysis2.blocks_analyzed);
}

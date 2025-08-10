#include <iostream>
#include <string>
#include <memory>
#include <chrono>
#include "encoder/watermark_encoder.h"
#include "extractor/watermark_extractor.h"
#include "common/utils.h"

using namespace phantomframe;

void printUsage() {
    std::cout << "PhantomFrame - Imperceptible Video Watermarking System\n"
              << "Usage:\n"
              << "  phantomframe encode <input_video> <output_video> <payload>\n"
              << "  phantomframe detect <input_video>\n"
              << "  phantomframe demo\n"
              << "\n"
              << "Commands:\n"
              << "  encode  - Embed watermark in video\n"
              << "  detect  - Detect watermark in video\n"
              << "  demo    - Run demonstration\n"
              << "\n"
              << "Examples:\n"
              << "  phantomframe encode input.mp4 output.mp4 \"Creator123\"\n"
              << "  phantomframe detect video.mp4\n"
              << "  phantomframe demo\n";
}

void runDemo() {
    std::cout << "\n=== PhantomFrame Demo ===\n\n";
    
    // Generate demo payload
    std::string demo_creator = "DemoCreator2024";
    uint64_t payload = utils::generatePayloadFromString(demo_creator);
    uint32_t seed = utils::generateRandomSeed();
    
    std::cout << "Demo Creator: " << demo_creator << "\n";
    std::cout << "Generated Payload: " << utils::payloadToHex(payload) << "\n";
    std::cout << "Generated Seed: " << seed << "\n\n";
    
    // Initialize watermark encoder
    WatermarkConfig encoder_config;
    encoder_config.payload = payload;
    encoder_config.seed = seed;
    encoder_config.block_density = 0.008f;  // 0.8% of blocks
    encoder_config.temporal_period = 30;    // Every 30 frames
    encoder_config.enable_encryption = false;
    
    auto encoder = std::make_unique<WatermarkEncoder>(encoder_config);
    
    if (!encoder->initialize(1920, 1080, 30.0f)) {
        std::cerr << "Failed to initialize encoder\n";
        return;
    }
    
    std::cout << "Encoder initialized successfully\n";
    std::cout << encoder->getStats() << "\n\n";
    
    // Simulate processing some frames
    std::cout << "Simulating frame processing...\n";
    for (int i = 0; i < 5; ++i) {
        // Create dummy frame data
        std::vector<uint8_t> dummy_frame(1920 * 1080 * 3, 128); // Gray frame
        
        auto processed_frame = encoder->processFrame(dummy_frame.data(), dummy_frame.size(), i);
        std::cout << "Processed frame " << i << " (" << processed_frame.size() << " bytes)\n";
    }
    
    std::cout << "\nFinal encoder stats:\n";
    std::cout << encoder->getStats() << "\n\n";
    
    // Initialize watermark extractor
    ExtractionConfig extractor_config;
    extractor_config.min_frames = 10;
    extractor_config.max_frames = 1000;
    extractor_config.confidence_threshold = 0.7;
    extractor_config.enable_debug = true;
    
    auto extractor = std::make_unique<WatermarkExtractor>(extractor_config);
    
    if (!extractor->initialize()) {
        std::cerr << "Failed to initialize extractor\n";
        return;
    }
    
    std::cout << "Extractor initialized successfully\n";
    std::cout << extractor->getStats() << "\n\n";
    
    std::cout << "Demo completed successfully!\n";
    std::cout << "Note: This is a demonstration with simulated data.\n";
    std::cout << "Real watermarking requires actual video files and VLC integration.\n";
}

void encodeVideo(const std::string& input_path, const std::string& output_path, const std::string& payload_str) {
    std::cout << "Encoding video with watermark...\n";
    
    // Validate input file
    if (!utils::isValidVideoFile(input_path)) {
        std::cerr << "Error: Invalid video file: " << input_path << "\n";
        return;
    }
    
    // Generate payload and seed
    uint64_t payload = utils::generatePayloadFromString(payload_str);
    uint32_t seed = utils::generateRandomSeed();
    
    std::cout << "Input: " << input_path << "\n";
    std::cout << "Output: " << output_path << "\n";
    std::cout << "Payload: " << utils::payloadToHex(payload) << "\n";
    std::cout << "Seed: " << seed << "\n\n";
    
    // Initialize encoder
    WatermarkConfig config;
    config.payload = payload;
    config.seed = seed;
    config.block_density = 0.008f;
    config.temporal_period = 30;
    config.enable_encryption = false;
    
    auto encoder = std::make_unique<WatermarkEncoder>(config);
    
    // Get video info (simplified)
    std::cout << "Video info:\n";
    std::cout << utils::getVideoInfo(input_path) << "\n\n";
    
    // In a real implementation, this would:
    // 1. Open the video file
    // 2. Process each frame through the encoder
    // 3. Write the watermarked video
    // 4. Integrate with VLC's x264 encoder
    
    std::cout << "Note: Full video encoding requires VLC integration.\n";
    std::cout << "This demo shows the watermarking algorithm setup.\n\n";
    
    std::cout << "Encoder configuration:\n";
    std::cout << encoder->getStats() << "\n";
}

void detectWatermark(const std::string& input_path) {
    std::cout << "Detecting watermark in video...\n";
    
    // Validate input file
    if (!utils::isValidVideoFile(input_path)) {
        std::cerr << "Error: Invalid video file: " << input_path << "\n";
        return;
    }
    
    std::cout << "Input: " << input_path << "\n\n";
    
    // Initialize extractor
    ExtractionConfig config;
    config.min_frames = 10;
    config.max_frames = 1000;
    config.confidence_threshold = 0.7;
    config.enable_debug = true;
    
    auto extractor = std::make_unique<WatermarkExtractor>(config);
    
    if (!extractor->initialize()) {
        std::cerr << "Failed to initialize extractor\n";
        return;
    }
    
    // In a real implementation, this would analyze the actual video
    // For now, we'll show the extractor setup
    
    std::cout << "Extractor initialized successfully\n";
    std::cout << extractor->getStats() << "\n\n";
    
    std::cout << "Note: Full watermark detection requires actual video analysis.\n";
    std::cout << "This demo shows the detection algorithm setup.\n";
}

int main(int argc, char* argv[]) {
    std::cout << "PhantomFrame v1.0.0\n";
    std::cout << "Imperceptible Video Watermarking System\n\n";
    
    if (argc < 2) {
        printUsage();
        return 1;
    }
    
    std::string command = argv[1];
    
    try {
        if (command == "encode") {
            if (argc != 5) {
                std::cerr << "Error: encode command requires 3 arguments\n";
                printUsage();
                return 1;
            }
            encodeVideo(argv[2], argv[3], argv[4]);
        }
        else if (command == "detect") {
            if (argc != 3) {
                std::cerr << "Error: detect command requires 1 argument\n";
                printUsage();
                return 1;
            }
            detectWatermark(argv[2]);
        }
        else if (command == "demo") {
            runDemo();
        }
        else {
            std::cerr << "Error: Unknown command: " << command << "\n";
            printUsage();
            return 1;
        }
    }
    catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << "\n";
        return 1;
    }
    
    return 0;
}

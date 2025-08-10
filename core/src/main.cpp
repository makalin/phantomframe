#include <iostream>
#include <string>
#include <memory>
#include <opencv2/opencv.hpp>
#include "common/config.h"
#include "common/logger.h"
#include "encoder/watermark_encoder.h"
#include "extractor/watermark_extractor.h"
#include "common/utils.h"

using namespace phantomframe;

void printUsage(const char* programName) {
    std::cout << "PhantomFrame Video Watermarking System v1.0.0" << std::endl;
    std::cout << "=============================================" << std::endl;
    std::cout << "Usage: " << programName << " <command> [options]" << std::endl;
    std::cout << std::endl;
    std::cout << "Commands:" << std::endl;
    std::cout << "  encode    Embed watermark in video file" << std::endl;
    std::cout << "  extract   Extract watermark from video file" << std::endl;
    std::cout << "  analyze   Analyze video quality and properties" << std::endl;
    std::cout << "  help      Show this help message" << std::endl;
    std::cout << std::endl;
    std::cout << "Examples:" << std::endl;
    std::cout << "  " << programName << " encode input.mp4 output.mp4 --payload \"Hello World\" --seed 12345" << std::endl;
    std::cout << "  " << programName << " extract input.mp4 --confidence 0.8" << std::endl;
    std::cout << "  " << programName << " analyze input.mp4" << std::endl;
    std::cout << std::endl;
    std::cout << "For detailed help on a command, use: " << programName << " <command> --help" << std::endl;
}

void printEncodeHelp() {
    std::cout << "Encode Command Options:" << std::endl;
    std::cout << "  --payload <text>     Watermark payload text (default: random)" << std::endl;
    std::cout << "  --seed <number>      Random seed for watermark generation (default: auto)" << std::endl;
    std::cout << "  --density <0.1-1.0>  Block density for watermark embedding (default: 0.3)" << std::endl;
    std::cout << "  --adaptive           Enable adaptive embedding based on video content" << std::endl;
    std::cout << "  --temporal <period>  Temporal period for watermark (default: 30)" << std::endl;
    std::cout << "  --quality <0-100>    Quality preservation level (default: 95)" << std::endl;
    std::cout << "  --verbose            Enable verbose output" << std::endl;
}

void printExtractHelp() {
    std::cout << "Extract Command Options:" << std::endl;
    std::cout << "  --confidence <0.0-1.0>  Minimum confidence threshold (default: 0.8)" << std::endl;
    std::cout << "  --method <method>        Analysis method: basic, advanced, hybrid (default: hybrid)" << std::endl;
    std::cout << "  --block-size <size>      Block size for analysis (default: 8)" << std::endl;
    std::cout << "  --temporal              Enable temporal analysis across frames" << std::endl;
    std::cout << "  --batch                 Enable batch processing for multiple files" << std::endl;
    std::cout << "  --verbose               Enable verbose output" << std::endl;
}

void printAnalyzeHelp() {
    std::cout << "Analyze Command Options:" << std::endl;
    std::cout << "  --format              Show detailed format information" << std::endl;
    std::cout << "  --quality             Analyze video quality metrics" << std::endl;
    std::cout << "  --watermark           Check for existing watermarks" << std::endl;
    std::endl;
}

bool parseArgs(int argc, char* argv[], std::string& command, std::map<std::string, std::string>& options) {
    if (argc < 2) {
        return false;
    }
    
    command = argv[1];
    
    for (int i = 2; i < argc; i++) {
        std::string arg = argv[i];
        if (arg.substr(0, 2) == "--") {
            if (i + 1 < argc && argv[i + 1][0] != '-') {
                options[arg.substr(2)] = argv[i + 1];
                i++; // Skip the value
            } else {
                options[arg.substr(2)] = "true";
            }
        }
    }
    
    return true;
}

int encodeCommand(const std::map<std::string, std::string>& options) {
    std::string inputFile = options.count("input") ? options.at("input") : "";
    std::string outputFile = options.count("output") ? options.at("output") : "";
    
    if (inputFile.empty() || outputFile.empty()) {
        std::cerr << "Error: Input and output files must be specified" << std::endl;
        return 1;
    }
    
    // Parse options
    std::string payload = options.count("payload") ? options.at("payload") : utils::generateRandomString(16);
    uint32_t seed = options.count("seed") ? std::stoul(options.at("seed")) : utils::generateRandomSeed();
    float density = options.count("density") ? std::stof(options.at("density")) : 0.3f;
    bool adaptive = options.count("adaptive");
    int temporal = options.count("temporal") ? std::stoi(options.at("temporal")) : 30;
    int quality = options.count("quality") ? std::stoi(options.at("quality")) : 95;
    bool verbose = options.count("verbose");
    
    try {
        // Initialize encoder
        EncodingConfig config;
        config.payload = payload;
        config.random_seed = seed;
        config.block_density = density;
        config.adaptive_embedding = adaptive;
        config.temporal_period = temporal;
        config.quality_preservation = quality;
        
        auto encoder = std::make_unique<WatermarkEncoder>(config);
        
        if (verbose) {
            std::cout << "Encoding configuration:" << std::endl;
            std::cout << "  Input: " << inputFile << std::endl;
            std::cout << "  Output: " << outputFile << std::endl;
            std::cout << "  Payload: " << payload << std::endl;
            std::cout << "  Seed: " << seed << std::endl;
            std::cout << "  Density: " << density << std::endl;
            std::cout << "  Adaptive: " << (adaptive ? "Yes" : "No") << std::endl;
            std::cout << "  Temporal: " << temporal << std::endl;
            std::cout << "  Quality: " << quality << std::endl;
            std::cout << std::endl;
        }
        
        // Process video
        std::cout << "Processing video..." << std::endl;
        auto result = encoder->processVideo(inputFile, outputFile);
        
        if (result.success) {
            std::cout << "Successfully encoded watermark!" << std::endl;
            std::cout << "  Processing time: " << result.processing_time << "ms" << std::endl;
            std::cout << "  Frames processed: " << result.frames_processed << std::endl;
            std::cout << "  Quality metrics: PSNR=" << result.quality_metrics.psnr 
                      << ", SSIM=" << result.quality_metrics.ssim << std::endl;
        } else {
            std::cerr << "Error encoding watermark: " << result.error_message << std::endl;
            return 1;
        }
        
    } catch (const std::exception& e) {
        std::cerr << "Exception during encoding: " << e.what() << std::endl;
        return 1;
    }
    
    return 0;
}

int extractCommand(const std::map<std::string, std::string>& options) {
    std::string inputFile = options.count("input") ? options.at("input") : "";
    
    if (inputFile.empty()) {
        std::cerr << "Error: Input file must be specified" << std::endl;
        return 1;
    }
    
    // Parse options
    float confidence = options.count("confidence") ? std::stof(options.at("confidence")) : 0.8f;
    std::string method = options.count("method") ? options.at("method") : "hybrid";
    int blockSize = options.count("block-size") ? std::stoi(options.at("block-size")) : 8;
    bool temporal = options.count("temporal");
    bool batch = options.count("batch");
    bool verbose = options.count("verbose");
    
    try {
        // Initialize extractor
        ExtractionConfig config;
        config.confidence_threshold = confidence;
        config.analysis_method = method;
        config.block_size = blockSize;
        config.temporal_analysis = temporal;
        config.batch_processing = batch;
        
        auto extractor = std::make_unique<WatermarkExtractor>(config);
        
        if (verbose) {
            std::cout << "Extraction configuration:" << std::endl;
            std::cout << "  Input: " << inputFile << std::endl;
            std::cout << "  Confidence threshold: " << confidence << std::endl;
            std::cout << "  Method: " << method << std::endl;
            std::cout << "  Block size: " << blockSize << std::endl;
            std::cout << "  Temporal analysis: " << (temporal ? "Yes" : "No") << std::endl;
            std::cout << "  Batch processing: " << (batch ? "Yes" : "No") << std::endl;
            std::cout << std::endl;
        }
        
        // Extract watermark
        std::cout << "Analyzing video for watermarks..." << std::endl;
        auto result = extractor->extractWatermark(inputFile);
        
        if (result.watermark_detected) {
            std::cout << "Watermark detected!" << std::endl;
            std::cout << "  Payload: " << result.payload << std::endl;
            std::cout << "  Confidence: " << result.confidence << std::endl;
            std::cout << "  Seed: " << result.seed << std::endl;
            std::cout << "  Block density: " << result.block_density << std::endl;
            std::cout << "  Processing time: " << result.processing_time << "ms" << std::endl;
        } else {
            std::cout << "No watermark detected." << std::endl;
            std::cout << "  Processing time: " << result.processing_time << "ms" << std::endl;
        }
        
    } catch (const std::exception& e) {
        std::cerr << "Exception during extraction: " << e.what() << std::endl;
        return 1;
    }
    
    return 0;
}

int analyzeCommand(const std::map<std::string, std::string>& options) {
    std::string inputFile = options.count("input") ? options.at("input") : "";
    
    if (inputFile.empty()) {
        std::cerr << "Error: Input file must be specified" << std::endl;
        return 1;
    }
    
    bool showFormat = options.count("format");
    bool showQuality = options.count("quality");
    bool showWatermark = options.count("watermark");
    
    try {
        // Get video information
        auto videoInfo = utils::getVideoInfo(inputFile);
        
        std::cout << "Video Analysis Results:" << std::endl;
        std::cout << "=======================" << std::endl;
        std::cout << "File: " << inputFile << std::endl;
        std::cout << "Size: " << utils::formatFileSize(videoInfo.file_size) << std::endl;
        std::cout << "Duration: " << videoInfo.duration << " seconds" << std::endl;
        std::cout << "Resolution: " << videoInfo.width << "x" << videoInfo.height << std::endl;
        std::cout << "Frame rate: " << videoInfo.frame_rate << " fps" << std::endl;
        std::cout << "Codec: " << videoInfo.codec << std::endl;
        std::cout << "Bitrate: " << videoInfo.bitrate << " kbps" << std::endl;
        
        if (showFormat) {
            std::cout << std::endl << "Format Details:" << std::endl;
            std::cout << "  Container: " << videoInfo.container_format << std::endl;
            std::cout << "  Pixel format: " << videoInfo.pixel_format << std::endl;
            std::cout << "  Color space: " << videoInfo.color_space << std::endl;
        }
        
        if (showQuality) {
            std::cout << std::endl << "Quality Analysis:" << std::endl;
            std::cout << "  Quality assessment would be performed here..." << std::endl;
        }
        
        if (showWatermark) {
            std::cout << std::endl << "Watermark Check:" << std::endl;
            std::cout << "  Checking for existing watermarks..." << std::endl;
            
            ExtractionConfig config;
            config.confidence_threshold = 0.5f;
            auto extractor = std::make_unique<WatermarkExtractor>(config);
            auto result = extractor->extractWatermark(inputFile);
            
            if (result.watermark_detected) {
                std::cout << "  Watermark found: " << result.payload << " (confidence: " << result.confidence << ")" << std::endl;
            } else {
                std::cout << "  No watermark detected" << std::endl;
            }
        }
        
    } catch (const std::exception& e) {
        std::cerr << "Exception during analysis: " << e.what() << std::endl;
        return 1;
    }
    
    return 0;
}

int main(int argc, char* argv[]) {
    try {
        // Initialize logger
        Logger::initialize(LogLevel::INFO);
        
        std::string command;
        std::map<std::string, std::string> options;
        
        if (!parseArgs(argc, argv, command, options)) {
            printUsage(argv[0]);
            return 1;
        }
        
        if (command == "help" || command == "--help" || command == "-h") {
            printUsage(argv[0]);
            return 0;
        }
        
        if (command == "encode") {
            if (options.count("help") || options.count("--help")) {
                printEncodeHelp();
                return 0;
            }
            return encodeCommand(options);
        }
        
        if (command == "extract") {
            if (options.count("help") || options.count("--help")) {
                printExtractHelp();
                return 0;
            }
            return extractCommand(options);
        }
        
        if (command == "analyze") {
            if (options.count("help") || options.count("--help")) {
                printAnalyzeHelp();
                return 0;
            }
            return analyzeCommand(options);
        }
        
        std::cerr << "Unknown command: " << command << std::endl;
        printUsage(argv[0]);
        return 1;
        
    } catch (const std::exception& e) {
        std::cerr << "Fatal error: " << e.what() << std::endl;
        return 1;
    }
    
    return 0;
}

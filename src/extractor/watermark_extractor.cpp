#include "watermark_extractor.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include <algorithm>
#include <numeric>
#include <cmath>

namespace phantomframe {

WatermarkExtractor::WatermarkExtractor(const ExtractionConfig& config)
    : config_(config), initialized_(false), frames_analyzed_(0), 
      videos_processed_(0), watermarks_detected_(0) {
}

WatermarkExtractor::~WatermarkExtractor() = default;

bool WatermarkExtractor::initialize() {
    if (initialized_) {
        return true;
    }
    
    // Load the extraction model
    if (!loadModel()) {
        std::cerr << "Failed to load extraction model" << std::endl;
        return false;
    }
    
    initialized_ = true;
    std::cout << "WatermarkExtractor initialized successfully" << std::endl;
    
    return true;
}

DetectionResult WatermarkExtractor::analyzeVideo(const std::string& video_path) {
    if (!initialized_) {
        return {false, 0.0, 0, 0, "Extractor not initialized"};
    }
    
    // Open video file
    cv::VideoCapture cap(video_path);
    if (!cap.isOpened()) {
        return {false, 0.0, 0, 0, "Failed to open video file: " + video_path};
    }
    
    std::vector<FrameAnalysis> frame_analyses;
    uint32_t frame_count = 0;
    
    // Analyze frames
    while (cap.isOpened() && frame_count < config_.max_frames) {
        cv::Mat frame;
        if (!cap.read(frame)) {
            break;
        }
        
        auto analysis = analyzeFrame(frame, frame_count);
        frame_analyses.push_back(analysis);
        frame_count++;
        
        if (frame_count % 100 == 0 && config_.enable_debug) {
            std::cout << "Analyzed " << frame_count << " frames..." << std::endl;
        }
    }
    
    cap.release();
    
    if (frame_analyses.size() < config_.min_frames) {
        return {false, 0.0, 0, 0, 
                "Insufficient frames: " + std::to_string(frame_analyses.size()) + 
                " < " + std::to_string(config_.min_frames)};
    }
    
    videos_processed_++;
    frames_analyzed_ += frame_analyses.size();
    
    // Extract watermark from analyzed frames
    return extractWatermark(frame_analyses);
}

FrameAnalysis WatermarkExtractor::analyzeFrame(const cv::Mat& frame, uint32_t frame_index) {
    FrameAnalysis analysis;
    analysis.frame_index = frame_index;
    
    // Preprocess frame
    cv::Mat processed = preprocessFrame(frame);
    
    // Extract features
    analysis.qp_values = extractQPValues(processed);
    analysis.dct_coefficients = extractDCTCoefficients(processed);
    analysis.entropy = calculateEntropy(processed);
    analysis.variance = calculateVariance(processed);
    
    return analysis;
}

DetectionResult WatermarkExtractor::extractWatermark(const std::vector<FrameAnalysis>& frames) {
    // Try statistical analysis first
    auto stat_result = statisticalAnalysis(frames);
    if (stat_result.detected && stat_result.confidence >= config_.confidence_threshold) {
        watermarks_detected_++;
        return stat_result;
    }
    
    // Fall back to machine learning analysis
    auto ml_result = mlAnalysis(frames);
    if (ml_result.detected && ml_result.confidence >= config_.confidence_threshold) {
        watermarks_detected_++;
        return ml_result;
    }
    
    return {false, 0.0, 0, 0, "No watermark detected with sufficient confidence"};
}

void WatermarkExtractor::updateConfig(const ExtractionConfig& config) {
    config_ = config;
}

std::string WatermarkExtractor::getStats() const {
    std::ostringstream oss;
    oss << "WatermarkExtractor Stats:\n"
        << "  Videos processed: " << videos_processed_ << "\n"
        << "  Frames analyzed: " << frames_analyzed_ << "\n"
        << "  Watermarks detected: " << watermarks_detected_ << "\n"
        << "  Detection rate: " 
        << (videos_processed_ > 0 ? (double)watermarks_detected_ / videos_processed_ * 100 : 0)
        << "%\n"
        << "  Confidence threshold: " << config_.confidence_threshold << "\n"
        << "  Min frames: " << config_.min_frames << "\n"
        << "  Max frames: " << config_.max_frames;
    
    return oss.str();
}

bool WatermarkExtractor::loadModel() {
    // In a real implementation, this would load a TensorFlow.js model
    // For now, we'll create some dummy weights
    
    model_weights_.resize(1024, 0.0);
    
    // Initialize with some random-like weights
    for (size_t i = 0; i < model_weights_.size(); ++i) {
        model_weights_[i] = std::sin(i * 0.1) * 0.5 + 0.5;
    }
    
    std::cout << "Loaded model with " << model_weights_.size() << " weights" << std::endl;
    return true;
}

cv::Mat WatermarkExtractor::preprocessFrame(const cv::Mat& frame) {
    cv::Mat processed;
    
    // Convert to grayscale if needed
    if (frame.channels() == 3) {
        cv::cvtColor(frame, processed, cv::COLOR_BGR2GRAY);
    } else {
        processed = frame.clone();
    }
    
    // Resize to standard size for analysis
    cv::resize(processed, processed, cv::Size(720, 720));
    
    // Normalize to 0-1 range
    processed.convertTo(processed, CV_64F, 1.0/255.0);
    
    return processed;
}

std::vector<double> WatermarkExtractor::extractQPValues(const cv::Mat& frame) {
    // In a real implementation, this would extract actual QP values
    // from the H.264 stream. For now, we'll simulate this by analyzing
    // the frame's statistical properties
    
    std::vector<double> qp_values;
    qp_values.reserve(64); // 8x8 blocks
    
    // Divide frame into 8x8 blocks and calculate "QP-like" values
    for (int y = 0; y < frame.rows; y += 8) {
        for (int x = 0; x < frame.cols; x += 8) {
            cv::Rect block_rect(x, y, std::min(8, frame.cols - x), std::min(8, frame.rows - y));
            cv::Mat block = frame(block_rect);
            
            // Calculate block variance as a proxy for QP
            cv::Scalar mean, stddev;
            cv::meanStdDev(block, mean, stddev);
            double qp_value = stddev[0] * 100; // Scale to reasonable range
            
            qp_values.push_back(qp_value);
        }
    }
    
    return qp_values;
}

std::vector<double> WatermarkExtractor::extractDCTCoefficients(const cv::Mat& frame) {
    // In a real implementation, this would extract actual DCT coefficients
    // For now, we'll simulate this by applying DCT to the frame
    
    cv::Mat dct_frame;
    cv::dct(frame, dct_frame);
    
    std::vector<double> coefficients;
    coefficients.reserve(dct_frame.rows * dct_frame.cols);
    
    // Flatten DCT coefficients
    for (int i = 0; i < dct_frame.rows; ++i) {
        for (int j = 0; j < dct_frame.cols; ++j) {
            coefficients.push_back(dct_frame.at<double>(i, j));
        }
    }
    
    return coefficients;
}

double WatermarkExtractor::calculateEntropy(const cv::Mat& frame) {
    // Calculate image entropy as a measure of information content
    std::vector<int> histogram(256, 0);
    
    for (int i = 0; i < frame.rows; ++i) {
        for (int j = 0; j < frame.cols; ++j) {
            int pixel_value = static_cast<int>(frame.at<double>(i, j) * 255);
            histogram[pixel_value]++;
        }
    }
    
    double entropy = 0.0;
    int total_pixels = frame.rows * frame.cols;
    
    for (int i = 0; i < 256; ++i) {
        if (histogram[i] > 0) {
            double probability = static_cast<double>(histogram[i]) / total_pixels;
            entropy -= probability * std::log2(probability);
        }
    }
    
    return entropy;
}

double WatermarkExtractor::calculateVariance(const cv::Mat& frame) {
    cv::Scalar mean, stddev;
    cv::meanStdDev(frame, mean, stddev);
    return stddev[0] * stddev[0]; // Return variance (stddev^2)
}

DetectionResult WatermarkExtractor::statisticalAnalysis(const std::vector<FrameAnalysis>& frames) {
    // Simple statistical analysis to detect watermark patterns
    // This is a simplified version - real implementation would be more sophisticated
    
    if (frames.size() < 10) {
        return {false, 0.0, 0, 0, "Insufficient frames for statistical analysis"};
    }
    
    // Analyze QP value patterns across frames
    std::vector<double> qp_patterns;
    
    for (const auto& frame : frames) {
        if (!frame.qp_values.empty()) {
            // Calculate average QP for this frame
            double avg_qp = std::accumulate(frame.qp_values.begin(), frame.qp_values.end(), 0.0) 
                           / frame.qp_values.size();
            qp_patterns.push_back(avg_qp);
        }
    }
    
    // Look for periodic patterns that might indicate watermark
    double confidence = 0.0;
    uint64_t payload = 0;
    uint32_t seed = 0;
    
    // Simple pattern detection (in reality, this would be much more sophisticated)
    if (qp_patterns.size() >= 20) {
        // Calculate autocorrelation to find periodic patterns
        std::vector<double> autocorr;
        for (size_t lag = 1; lag < qp_patterns.size() / 2; ++lag) {
            double corr = 0.0;
            for (size_t i = 0; i < qp_patterns.size() - lag; ++i) {
                corr += qp_patterns[i] * qp_patterns[i + lag];
            }
            autocorr.push_back(corr);
        }
        
        // Find peaks in autocorrelation
        double max_corr = *std::max_element(autocorr.begin(), autocorr.end());
        if (max_corr > 0.1) { // Threshold for pattern detection
            confidence = std::min(0.8, max_corr);
            payload = static_cast<uint64_t>(max_corr * 1000000);
            seed = static_cast<uint32_t>(autocorr.size());
        }
    }
    
    return {confidence > 0.5, confidence, payload, seed, ""};
}

DetectionResult WatermarkExtractor::mlAnalysis(const std::vector<FrameAnalysis>& frames) {
    // Machine learning-based analysis using the loaded model
    // This is a simplified version - real implementation would use TensorFlow.js
    
    if (frames.empty() || model_weights_.empty()) {
        return {false, 0.0, 0, 0, "No frames or model available for ML analysis"};
    }
    
    // Extract features from frames
    std::vector<double> features;
    for (const auto& frame : frames) {
        // Combine various frame features
        features.insert(features.end(), frame.qp_values.begin(), frame.qp_values.end());
        features.insert(features.end(), frame.dct_coefficients.begin(), frame.dct_coefficients.end());
        features.push_back(frame.entropy);
        features.push_back(frame.variance);
    }
    
    // Simple neural network forward pass (simplified)
    double confidence = 0.0;
    for (size_t i = 0; i < std::min(features.size(), model_weights_.size()); ++i) {
        confidence += features[i] * model_weights_[i];
    }
    
    // Normalize confidence
    confidence = std::tanh(confidence) * 0.5 + 0.5;
    confidence = std::max(0.0, std::min(1.0, confidence));
    
    // Generate payload and seed from features
    uint64_t payload = 0;
    uint32_t seed = 0;
    
    for (size_t i = 0; i < std::min(features.size(), static_cast<size_t>(8)); ++i) {
        payload |= static_cast<uint64_t>(features[i] * 255) << (i * 8);
    }
    
    for (size_t i = 0; i < std::min(features.size(), static_cast<size_t>(4)); ++i) {
        seed |= static_cast<uint32_t>(features[i] * 255) << (i * 8);
    }
    
    return {confidence > 0.6, confidence, payload, seed, ""};
}

uint64_t WatermarkExtractor::decodePayload(const std::vector<double>& pattern) {
    // Decode 128-bit payload from detected watermark pattern
    // This is a simplified implementation
    
    uint64_t payload = 0;
    
    for (size_t i = 0; i < std::min(pattern.size(), static_cast<size_t>(16)); ++i) {
        uint8_t byte = static_cast<uint8_t>(pattern[i] * 255);
        payload |= static_cast<uint64_t>(byte) << (i * 8);
    }
    
    return payload;
}

} // namespace phantomframe

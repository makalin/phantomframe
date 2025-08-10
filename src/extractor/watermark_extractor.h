#ifndef PHANTOMFRAME_WATERMARK_EXTRACTOR_H
#define PHANTOMFRAME_WATERMARK_EXTRACTOR_H

#include <cstdint>
#include <vector>
#include <memory>
#include <string>
#include <opencv2/opencv.hpp>

namespace phantomframe {

/**
 * @brief Watermark detection result
 */
struct DetectionResult {
    bool detected;              // Whether watermark was detected
    double confidence;          // Detection confidence (0.0 - 1.0)
    uint64_t payload;          // Extracted payload
    uint32_t seed;             // Detected seed
    std::string error_message; // Error message if detection failed
};

/**
 * @brief Configuration for watermark extraction
 */
struct ExtractionConfig {
    uint32_t min_frames;        // Minimum frames to analyze
    uint32_t max_frames;        // Maximum frames to analyze
    double confidence_threshold; // Minimum confidence for detection
    bool enable_debug;          // Enable debug output
    std::string model_path;     // Path to TensorFlow.js model
};

/**
 * @brief Frame analysis data
 */
struct FrameAnalysis {
    uint32_t frame_index;
    std::vector<double> qp_values;
    std::vector<double> dct_coefficients;
    double entropy;
    double variance;
};

/**
 * @brief Main watermark extractor class
 */
class WatermarkExtractor {
public:
    explicit WatermarkExtractor(const ExtractionConfig& config);
    ~WatermarkExtractor();

    /**
     * @brief Initialize the extractor
     * @return true if successful
     */
    bool initialize();

    /**
     * @brief Analyze a video file for watermarks
     * @param video_path Path to video file
     * @return Detection result
     */
    DetectionResult analyzeVideo(const std::string& video_path);

    /**
     * @brief Analyze a single frame
     * @param frame Frame data
     * @param frame_index Frame index
     * @return Frame analysis data
     */
    FrameAnalysis analyzeFrame(const cv::Mat& frame, uint32_t frame_index);

    /**
     * @brief Extract watermark from analyzed frames
     * @param frames Vector of frame analysis data
     * @return Detection result
     */
    DetectionResult extractWatermark(const std::vector<FrameAnalysis>& frames);

    /**
     * @brief Update extraction configuration
     * @param config New configuration
     */
    void updateConfig(const ExtractionConfig& config);

    /**
     * @brief Get extraction statistics
     * @return Statistics string
     */
    std::string getStats() const;

private:
    ExtractionConfig config_;
    bool initialized_;
    
    // Statistics
    uint32_t frames_analyzed_;
    uint32_t videos_processed_;
    uint32_t watermarks_detected_;
    
    // Model data (would be loaded from TensorFlow.js in practice)
    std::vector<double> model_weights_;
    
    /**
     * @brief Load the extraction model
     * @return true if successful
     */
    bool loadModel();
    
    /**
     * @brief Preprocess frame for analysis
     * @param frame Input frame
     * @return Preprocessed frame
     */
    cv::Mat preprocessFrame(const cv::Mat& frame);
    
    /**
     * @brief Extract QP values from frame
     * @param frame Input frame
     * @return Vector of QP values
     */
    std::vector<double> extractQPValues(const cv::Mat& frame);
    
    /**
     * @brief Extract DCT coefficients from frame
     * @param frame Input frame
     * @return Vector of DCT coefficients
     */
    std::vector<double> extractDCTCoefficients(const cv::Mat& frame);
    
    /**
     * @brief Calculate frame entropy
     * @param frame Input frame
     * @return Entropy value
     */
    double calculateEntropy(const cv::Mat& frame);
    
    /**
     * @brief Calculate frame variance
     * @param frame Input frame
     * @return Variance value
     */
    double calculateVariance(const cv::Mat& frame);
    
    /**
     * @brief Apply statistical analysis for watermark detection
     * @param frames Frame analysis data
     * @return Detection result
     */
    DetectionResult statisticalAnalysis(const std::vector<FrameAnalysis>& frames);
    
    /**
     * @brief Apply machine learning model for watermark detection
     * @param frames Frame analysis data
     * @return Detection result
     */
    DetectionResult mlAnalysis(const std::vector<FrameAnalysis>& frames);
    
    /**
     * @brief Decode payload from detected pattern
     * @param pattern Detected watermark pattern
     * @return Decoded payload
     */
    uint64_t decodePayload(const std::vector<double>& pattern);
};

} // namespace phantomframe

#endif // PHANTOMFRAME_WATERMARK_EXTRACTOR_H

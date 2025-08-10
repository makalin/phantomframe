#ifndef PHANTOMFRAME_WATERMARK_ENCODER_H
#define PHANTOMFRAME_WATERMARK_ENCODER_H

#include <cstdint>
#include <vector>
#include <memory>
#include <string>

namespace phantomframe {

/**
 * @brief Configuration for watermark embedding
 */
struct WatermarkConfig {
    uint64_t payload;           // 128-bit payload to embed
    uint32_t seed;              // Pseudo-random seed for block selection
    float block_density;         // Percentage of blocks to modify (0.005-0.01)
    uint32_t temporal_period;   // Frames between pattern repetition
    bool enable_encryption;     // Whether to encrypt the payload
    std::string encryption_key; // Encryption key if enabled
};

/**
 * @brief Block information for watermark embedding
 */
struct BlockInfo {
    uint32_t x, y;              // Block coordinates
    int8_t qp_delta;            // QP modification (-1, 0, +1)
    uint32_t frame_index;       // Frame where this block is modified
};

/**
 * @brief Main watermark encoder class
 */
class WatermarkEncoder {
public:
    explicit WatermarkEncoder(const WatermarkConfig& config);
    ~WatermarkEncoder();

    /**
     * @brief Initialize the encoder with video parameters
     * @param width Video width
     * @param height Video height
     * @param fps Frames per second
     * @return true if successful
     */
    bool initialize(uint32_t width, uint32_t height, float fps);

    /**
     * @brief Process a frame and apply watermark
     * @param frame_data Raw frame data
     * @param frame_size Size of frame data
     * @param frame_index Current frame index
     * @return Modified frame data
     */
    std::vector<uint8_t> processFrame(const uint8_t* frame_data, 
                                     size_t frame_size, 
                                     uint32_t frame_index);

    /**
     * @brief Get blocks to modify for current frame
     * @param frame_index Current frame index
     * @return Vector of blocks to modify
     */
    std::vector<BlockInfo> getBlocksForFrame(uint32_t frame_index);

    /**
     * @brief Update watermark configuration
     * @param config New configuration
     */
    void updateConfig(const WatermarkConfig& config);

    /**
     * @brief Get current watermark statistics
     * @return Statistics string
     */
    std::string getStats() const;

private:
    WatermarkConfig config_;
    uint32_t width_, height_;
    float fps_;
    uint32_t total_blocks_;
    
    // Block selection state
    std::vector<uint32_t> block_indices_;
    uint32_t current_block_index_;
    
    // Statistics
    uint32_t frames_processed_;
    uint32_t blocks_modified_;
    
    /**
     * @brief Generate pseudo-random block selection
     */
    void generateBlockSelection();
    
    /**
     * @brief Calculate QP delta for a block
     * @param block_index Block index
     * @param frame_index Frame index
     * @return QP delta value
     */
    int8_t calculateQPDelta(uint32_t block_index, uint32_t frame_index);
    
    /**
     * @brief Apply QP modification to frame data
     * @param frame_data Frame data to modify
     * @param block_info Block information
     */
    void applyQPModification(uint8_t* frame_data, const BlockInfo& block_info);
    
    /**
     * @brief Encrypt payload if enabled
     * @param payload Payload to encrypt
     * @return Encrypted payload
     */
    uint64_t encryptPayload(uint64_t payload);
};

} // namespace phantomframe

#endif // PHANTOMFRAME_WATERMARK_ENCODER_H

#include "watermark_encoder.h"
#include <random>
#include <algorithm>
#include <cstring>
#include <iostream>
#include <iomanip>
#include <sstream>

namespace phantomframe {

WatermarkEncoder::WatermarkEncoder(const WatermarkConfig& config)
    : config_(config), width_(0), height_(0), fps_(0.0f), 
      total_blocks_(0), current_block_index_(0), 
      frames_processed_(0), blocks_modified_(0) {
}

WatermarkEncoder::~WatermarkEncoder() = default;

bool WatermarkEncoder::initialize(uint32_t width, uint32_t height, float fps) {
    width_ = width;
    height_ = height;
    fps_ = fps;
    
    // Calculate total number of 8x8 blocks
    uint32_t blocks_x = (width + 7) / 8;
    uint32_t blocks_y = (height + 7) / 8;
    total_blocks_ = blocks_x * blocks_y;
    
    // Generate block selection pattern
    generateBlockSelection();
    
    std::cout << "WatermarkEncoder initialized: " << width << "x" << height 
              << " @ " << fps << "fps, " << total_blocks_ << " blocks" << std::endl;
    
    return true;
}

std::vector<uint8_t> WatermarkEncoder::processFrame(const uint8_t* frame_data, 
                                                   size_t frame_size, 
                                                   uint32_t frame_index) {
    // Create a copy of the frame data
    std::vector<uint8_t> modified_frame(frame_data, frame_data + frame_size);
    
    // Get blocks to modify for this frame
    auto blocks = getBlocksForFrame(frame_index);
    
    // Apply watermark modifications
    for (const auto& block : blocks) {
        applyQPModification(modified_frame.data(), block);
        blocks_modified_++;
    }
    
    frames_processed_++;
    
    return modified_frame;
}

std::vector<BlockInfo> WatermarkEncoder::getBlocksForFrame(uint32_t frame_index) {
    std::vector<BlockInfo> blocks;
    
    // Calculate how many blocks to modify this frame
    uint32_t blocks_per_frame = static_cast<uint32_t>(
        total_blocks_ * config_.block_density / config_.temporal_period
    );
    
    // Ensure we don't exceed total blocks
    blocks_per_frame = std::min(blocks_per_frame, total_blocks_);
    
    // Select blocks for this frame
    for (uint32_t i = 0; i < blocks_per_frame; ++i) {
        uint32_t block_idx = (frame_index + i * config_.temporal_period) % total_blocks_;
        
        // Calculate block coordinates
        uint32_t blocks_x = (width_ + 7) / 8;
        uint32_t x = (block_idx % blocks_x) * 8;
        uint32_t y = (block_idx / blocks_x) * 8;
        
        // Calculate QP delta
        int8_t qp_delta = calculateQPDelta(block_idx, frame_index);
        
        blocks.push_back({x, y, qp_delta, frame_index});
    }
    
    return blocks;
}

void WatermarkEncoder::updateConfig(const WatermarkConfig& config) {
    config_ = config;
    generateBlockSelection();
}

std::string WatermarkEncoder::getStats() const {
    std::ostringstream oss;
    oss << "WatermarkEncoder Stats:\n"
        << "  Frames processed: " << frames_processed_ << "\n"
        << "  Blocks modified: " << blocks_modified_ << "\n"
        << "  Total blocks: " << total_blocks_ << "\n"
        << "  Block density: " << (config_.block_density * 100) << "%\n"
        << "  Temporal period: " << config_.temporal_period << " frames\n"
        << "  Payload: 0x" << std::hex << std::setw(16) << std::setfill('0') 
        << config_.payload << std::dec;
    
    return oss.str();
}

void WatermarkEncoder::generateBlockSelection() {
    block_indices_.clear();
    block_indices_.reserve(total_blocks_);
    
    // Generate sequence of all block indices
    for (uint32_t i = 0; i < total_blocks_; ++i) {
        block_indices_.push_back(i);
    }
    
    // Shuffle using the seed
    std::mt19937 rng(config_.seed);
    std::shuffle(block_indices_.begin(), block_indices_.end(), rng);
    
    current_block_index_ = 0;
}

int8_t WatermarkEncoder::calculateQPDelta(uint32_t block_index, uint32_t frame_index) {
    // Use block index and frame index to determine QP delta
    // This creates a pseudo-random but deterministic pattern
    
    // Simple hash function for demonstration
    uint32_t hash = block_index * 31 + frame_index * 17 + config_.seed;
    hash = ((hash << 13) ^ hash) >> 19;
    
    // Map to QP delta: -1, 0, or +1
    uint32_t mod = hash % 3;
    if (mod == 0) return -1;
    if (mod == 1) return 0;
    return 1;
}

void WatermarkEncoder::applyQPModification(uint8_t* frame_data, const BlockInfo& block_info) {
    // This is a simplified implementation
    // In practice, this would modify the DCT coefficients or QP values
    // in the H.264 encoding process
    
    // For now, we'll just mark the block (in a real implementation,
    // this would modify the actual encoding parameters)
    
    // Calculate block offset in frame data
    uint32_t block_offset = block_info.y * width_ + block_info.x;
    
    // In a real implementation, this would modify QP values
    // frame_data[block_offset] += block_info.qp_delta;
    
    // For demonstration, we'll just ensure the block is accessible
    if (block_offset < width_ * height_) {
        // Block is within frame bounds
        // Actual QP modification would happen here
    }
}

uint64_t WatermarkEncoder::encryptPayload(uint64_t payload) {
    if (!config_.enable_encryption) {
        return payload;
    }
    
    // Simple XOR encryption for demonstration
    // In production, use proper cryptographic functions
    uint64_t key = std::hash<std::string>{}(config_.encryption_key);
    return payload ^ key;
}

} // namespace phantomframe

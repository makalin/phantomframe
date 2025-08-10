#include "utils.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include <iomanip>
#include <chrono>
#include <filesystem>
#include <algorithm>
#include <cstring>

namespace phantomframe {

namespace utils {

uint32_t generateRandomSeed() {
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<uint32_t> dis;
    return dis(gen);
}

uint64_t generatePayloadFromString(const std::string& input) {
    uint64_t hash = 0;
    for (char c : input) {
        hash = ((hash << 5) + hash) + c; // hash * 33 + c
    }
    return hash;
}

std::string payloadToHex(uint64_t payload) {
    std::ostringstream oss;
    oss << "0x" << std::hex << std::setw(16) << std::setfill('0') << payload;
    return oss.str();
}

uint64_t hexToPayload(const std::string& hex_string) {
    std::string hex = hex_string;
    if (hex.substr(0, 2) == "0x") {
        hex = hex.substr(2);
    }
    
    std::istringstream iss(hex);
    uint64_t payload;
    iss >> std::hex >> payload;
    return payload;
}

uint32_t calculateHash(const uint8_t* data, size_t length) {
    uint32_t hash = 0x811c9dc5; // FNV-1a hash
    for (size_t i = 0; i < length; ++i) {
        hash ^= data[i];
        hash *= 0x01000193;
    }
    return hash;
}

uint64_t xorEncrypt(uint64_t data, uint64_t key) {
    return data ^ key;
}

bool isValidVideoFile(const std::string& file_path) {
    std::ifstream file(file_path, std::ios::binary);
    if (!file.is_open()) {
        return false;
    }
    
    // Read file header to check video format
    uint8_t header[16];
    file.read(reinterpret_cast<char*>(header), 16);
    file.close();
    
    // Check for common video file signatures
    // MP4 signature
    if (header[4] == 'f' && header[5] == 't' && header[6] == 'y' && header[7] == 'p') {
        return true;
    }
    
    // AVI signature
    if (header[0] == 'R' && header[1] == 'I' && header[2] == 'F' && header[3] == 'F') {
        if (header[8] == 'A' && header[9] == 'V' && header[10] == 'I') {
            return true;
        }
    }
    
    // MKV signature
    if (header[0] == 0x1A && header[1] == 0x45 && header[2] == 0xDF && header[3] == 0xA3) {
        return true;
    }
    
    // MOV signature
    if (header[4] == 'f' && header[5] == 'r' && header[6] == 'e' && header[7] == 'e') {
        return true;
    }
    
    return false;
}

std::string getVideoInfo(const std::string& file_path) {
    std::ostringstream oss;
    
    if (!std::filesystem::exists(file_path)) {
        return "File does not exist";
    }
    
    std::filesystem::path path(file_path);
    oss << "File: " << path.filename().string() << "\n";
    oss << "Size: " << formatFileSize(std::filesystem::file_size(path)) << "\n";
    oss << "Format: " << path.extension().string() << "\n";
    
    // In a real implementation, you would use ffprobe or similar to get detailed video info
    oss << "Note: Detailed video info requires ffprobe integration";
    
    return oss.str();
}

std::string createTempFilePath(const std::string& prefix, const std::string& extension) {
    auto now = std::chrono::system_clock::now();
    auto timestamp = std::chrono::duration_cast<std::chrono::milliseconds>(
        now.time_since_epoch()
    ).count();
    
    std::ostringstream oss;
    oss << "/tmp/" << prefix << "_" << timestamp << extension;
    return oss.str();
}

void cleanupTempFiles(const std::vector<std::string>& file_paths) {
    for (const auto& file_path : file_paths) {
        try {
            if (std::filesystem::exists(file_path)) {
                std::filesystem::remove(file_path);
            }
        } catch (const std::exception& e) {
            std::cerr << "Failed to remove temp file " << file_path << ": " << e.what() << std::endl;
        }
    }
}

std::string formatFileSize(uint64_t bytes) {
    const char* units[] = {"B", "KB", "MB", "GB", "TB"};
    int unit_index = 0;
    double size = static_cast<double>(bytes);
    
    while (size >= 1024.0 && unit_index < 4) {
        size /= 1024.0;
        unit_index++;
    }
    
    std::ostringstream oss;
    oss << std::fixed << std::setprecision(2) << size << " " << units[unit_index];
    return oss.str();
}

std::string getCurrentTimestamp() {
    auto now = std::chrono::system_clock::now();
    auto time_t = std::chrono::system_clock::to_time_t(now);
    auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(
        now.time_since_epoch()
    ) % 1000;
    
    std::ostringstream oss;
    oss << std::put_time(std::localtime(&time_t), "%Y-%m-%d %H:%M:%S");
    oss << "." << std::setfill('0') << std::setw(3) << ms.count();
    
    return oss.str();
}

uint64_t calculateElapsedTime(const std::string& start, const std::string& end) {
    // Parse timestamps and calculate difference
    // This is a simplified implementation
    // In practice, you'd want more robust timestamp parsing
    
    try {
        std::tm start_tm = {}, end_tm = {};
        std::istringstream start_ss(start);
        std::istringstream end_ss(end);
        
        start_ss >> std::get_time(&start_tm, "%Y-%m-%d %H:%M:%S");
        end_ss >> std::get_time(&end_tm, "%Y-%m-%d %H:%M:%S");
        
        auto start_time = std::chrono::system_clock::from_time_t(std::mktime(&start_tm));
        auto end_time = std::chrono::system_clock::from_time_t(std::mktime(&end_tm));
        
        auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end_time - start_time);
        return duration.count();
    } catch (const std::exception& e) {
        std::cerr << "Error parsing timestamps: " << e.what() << std::endl;
        return 0;
    }
}

} // namespace utils

} // namespace phantomframe

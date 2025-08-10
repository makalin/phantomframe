#ifndef PHANTOMFRAME_UTILS_H
#define PHANTOMFRAME_UTILS_H

#include <cstdint>
#include <string>
#include <vector>
#include <random>

namespace phantomframe {

/**
 * @brief Utility functions for PhantomFrame
 */
namespace utils {

/**
 * @brief Generate a cryptographically secure random seed
 * @return Random 32-bit seed
 */
uint32_t generateRandomSeed();

/**
 * @brief Generate a 128-bit payload from a string
 * @param input Input string
 * @return 128-bit payload
 */
uint64_t generatePayloadFromString(const std::string& input);

/**
 * @brief Convert payload to hexadecimal string
 * @param payload Payload to convert
 * @return Hexadecimal string representation
 */
std::string payloadToHex(uint64_t payload);

/**
 * @brief Convert hexadecimal string to payload
 * @param hex_string Hexadecimal string
 * @return Payload value
 */
uint64_t hexToPayload(const std::string& hex_string);

/**
 * @brief Calculate hash of input data
 * @param data Input data
 * @param length Length of data
 * @return 32-bit hash value
 */
uint32_t calculateHash(const uint8_t* data, size_t length);

/**
 * @brief Simple encryption/decryption using XOR
 * @param data Data to encrypt/decrypt
 * @param key Encryption key
 * @return Encrypted/decrypted data
 */
uint64_t xorEncrypt(uint64_t data, uint64_t key);

/**
 * @brief Validate video file format
 * @param file_path Path to video file
 * @return true if valid video format
 */
bool isValidVideoFile(const std::string& file_path);

/**
 * @brief Get video file information
 * @param file_path Path to video file
 * @return Video info string
 */
std::string getVideoInfo(const std::string& file_path);

/**
 * @brief Create a temporary file path
 * @param prefix File prefix
 * @param extension File extension
 * @return Temporary file path
 */
std::string createTempFilePath(const std::string& prefix, const std::string& extension);

/**
 * @brief Clean up temporary files
 * @param file_paths Vector of file paths to clean up
 */
void cleanupTempFiles(const std::vector<std::string>& file_paths);

/**
 * @brief Format file size in human readable format
 * @param bytes Size in bytes
 * @return Formatted size string
 */
std::string formatFileSize(uint64_t bytes);

/**
 * @brief Get current timestamp as string
 * @return Timestamp string
 */
std::string getCurrentTimestamp();

/**
 * @brief Calculate elapsed time between two timestamps
 * @param start Start timestamp
 * @param end End timestamp
 * @return Elapsed time in milliseconds
 */
uint64_t calculateElapsedTime(const std::string& start, const std::string& end);

} // namespace utils

} // namespace phantomframe

#endif // PHANTOMFRAME_UTILS_H

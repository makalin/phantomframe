# PhantomFrame API Documentation

## Overview

PhantomFrame provides a comprehensive API for imperceptible video watermarking, including both C++ and Node.js interfaces.

## Table of Contents

1. [C++ API](#c-api)
2. [Node.js Backend API](#nodejs-backend-api)
3. [Model Training API](#model-training-api)
4. [Error Handling](#error-handling)
5. [Examples](#examples)

## C++ API

### Core Classes

#### WatermarkEncoder

The main class for embedding watermarks into video frames.

```cpp
class WatermarkEncoder {
public:
    explicit WatermarkEncoder(const WatermarkConfig& config);
    bool initialize(uint32_t width, uint32_t height, float fps);
    std::vector<uint8_t> processFrame(const uint8_t* frame_data, size_t frame_size, uint32_t frame_index);
    std::vector<BlockInfo> getBlocksForFrame(uint32_t frame_index);
    void updateConfig(const WatermarkConfig& config);
    std::string getStats() const;
};
```

**Constructor Parameters:**
- `config`: Watermark configuration structure

**Methods:**
- `initialize()`: Initialize encoder with video dimensions and frame rate
- `processFrame()`: Process a single frame and return watermarked data
- `getBlocksForFrame()`: Get information about blocks selected for watermarking
- `updateConfig()`: Update watermark configuration at runtime
- `getStats()`: Get encoder statistics and performance metrics

#### WatermarkExtractor

The main class for detecting and extracting watermarks from video frames.

```cpp
class WatermarkExtractor {
public:
    explicit WatermarkExtractor(const ExtractionConfig& config);
    bool initialize();
    DetectionResult analyzeVideo(const std::string& video_path);
    FrameAnalysis analyzeFrame(const cv::Mat& frame, uint32_t frame_index);
    DetectionResult extractWatermark(const std::vector<FrameAnalysis>& frames);
    void updateConfig(const ExtractionConfig& config);
    std::string getStats() const;
};
```

**Methods:**
- `initialize()`: Initialize the extractor
- `analyzeVideo()`: Analyze an entire video file for watermarks
- `analyzeFrame()`: Analyze a single frame for watermark features
- `extractWatermark()`: Extract watermark payload from analyzed frames
- `updateConfig()`: Update extraction configuration
- `getStats()`: Get extractor statistics

### Data Structures

#### WatermarkConfig

```cpp
struct WatermarkConfig {
    uint8_t payload[16];           // 128-bit watermark payload
    uint32_t seed;                 // Random seed for block selection
    float block_density;           // Fraction of blocks to watermark (0.0-1.0)
    uint32_t temporal_period;      // Temporal watermarking period
    bool adaptive_embedding;       // Enable adaptive embedding
    float quality_threshold;       // Quality threshold for embedding
};
```

#### ExtractionConfig

```cpp
struct ExtractionConfig {
    float confidence_threshold;     // Detection confidence threshold
    uint32_t max_frames;           // Maximum frames to analyze
    bool use_ml_model;             // Enable machine learning model
    std::string model_path;        // Path to trained model
    float temporal_weight;         // Weight for temporal analysis
};
```

#### DetectionResult

```cpp
struct DetectionResult {
    bool watermark_detected;       // Whether watermark was detected
    float confidence;              // Detection confidence (0.0-1.0)
    uint8_t payload[16];          // Extracted payload
    uint32_t frame_count;         // Number of frames analyzed
    float processing_time;         // Processing time in seconds
    std::string error_message;    // Error message if detection failed
};
```

### Utility Functions

```cpp
namespace phantomframe::utils {
    // Random number generation
    uint32_t generateRandomSeed();
    
    // Payload conversion
    std::string payloadToHex(const uint8_t* payload, size_t length);
    std::vector<uint8_t> hexToPayload(const std::string& hex_string);
    
    // Hashing and encryption
    uint32_t hashString(const std::string& input);
    std::vector<uint8_t> xorEncrypt(const std::vector<uint8_t>& data, const std::vector<uint8_t>& key);
    
    // Video utilities
    bool isValidVideoFile(const std::string& filepath);
    std::string getVideoInfo(const std::string& filepath);
    std::string createTempFile(const std::string& extension);
}
```

## Node.js Backend API

### Server Endpoints

#### Health Check

```
GET /api/health
```

Returns server health status.

**Response:**
```json
{
  "status": "healthy",
  "timestamp": "2024-01-01T00:00:00.000Z",
  "version": "1.0.0"
}
```

#### Watermark Embedding

```
POST /api/watermark/embed
```

Embed a watermark into a video file.

**Request Body:**
```json
{
  "videoPath": "/path/to/video.mp4",
  "payload": "0123456789abcdef",
  "seed": 12345,
  "blockDensity": 0.3,
  "temporalPeriod": 30
}
```

**Response:**
```json
{
  "message": "Watermark embedded successfully",
  "outputPath": "/path/to/watermarked_video.mp4",
  "processingTime": 2.5,
  "watermarkInfo": {
    "payload": "0123456789abcdef",
    "seed": 12345,
    "blocksModified": 1500
  }
}
```

#### Watermark Detection

```
POST /api/detection/analyze
```

Analyze a video file for watermarks.

**Request Body:**
```json
{
  "videoPath": "/path/to/video.mp4",
  "confidenceThreshold": 0.8,
  "maxFrames": 1000
}
```

**Response:**
```json
{
  "message": "Video analysis completed",
  "result": {
    "watermarkDetected": true,
    "confidence": 0.95,
    "payload": "0123456789abcdef",
    "frameCount": 1000,
    "processingTime": 3.2
  }
}
```

#### Batch Processing

```
POST /api/watermark/batch
POST /api/detection/batch
```

Process multiple videos in batch.

**Request Body:**
```json
{
  "videos": [
    {
      "path": "/path/to/video1.mp4",
      "payload": "0123456789abcdef"
    },
    {
      "path": "/path/to/video2.mp4",
      "payload": "fedcba9876543210"
    }
  ],
  "config": {
    "blockDensity": 0.3,
    "seed": 12345
  }
}
```

#### File Upload

```
POST /api/upload
```

Upload a video file for processing.

**Request:**
- Content-Type: `multipart/form-data`
- Field: `video` (file)

**Response:**
```json
{
  "message": "File uploaded successfully",
  "filename": "uploaded_video_12345.mp4",
  "size": 1048576,
  "path": "/uploads/uploaded_video_12345.mp4"
}
```

### Configuration Endpoints

```
GET /api/watermark/config
GET /api/detection/config
```

Get default configuration for watermarking and detection.

**Response:**
```json
{
  "defaultConfig": {
    "blockDensity": 0.3,
    "temporalPeriod": 30,
    "confidenceThreshold": 0.8,
    "maxFrames": 1000
  }
}
```

## Model Training API

### Python Classes

#### WatermarkDetectionModel

```python
class WatermarkDetectionModel:
    def __init__(self, config: Dict[str, Any]):
        """Initialize the model with configuration."""
    
    def build_model(self) -> keras.Model:
        """Build the neural network architecture."""
    
    def train(self, train_data, val_data, epochs=100, batch_size=32) -> History:
        """Train the model on provided data."""
    
    def predict(self, X: np.ndarray) -> np.ndarray:
        """Make predictions on new data."""
    
    def evaluate(self, X: np.ndarray, y: np.ndarray) -> Dict[str, float]:
        """Evaluate model performance."""
    
    def save_model(self, filepath: str) -> None:
        """Save the trained model."""
    
    def load_model(self, filepath: str) -> None:
        """Load a trained model."""
```

#### WatermarkDataGenerator

```python
class WatermarkDataGenerator:
    def __init__(self, config: Dict[str, Any]):
        """Initialize the data generator."""
    
    def generate_synthetic_data(self, num_samples: int) -> Tuple[np.ndarray, np.ndarray]:
        """Generate synthetic training data."""
    
    def load_video_data(self, video_paths: List[str], labels: List[int]) -> Tuple[np.ndarray, np.ndarray]:
        """Load training data from video files."""
    
    def create_data_generator(self, X, y, batch_size=32, shuffle=True) -> Generator:
        """Create a data generator for training."""
    
    def split_data(self, X, y, train_ratio=0.7, val_ratio=0.2, test_ratio=0.1):
        """Split data into train/validation/test sets."""
```

### Training Script

```bash
# Basic training
python model/scripts/train_model.py

# Custom configuration
python model/scripts/train_model.py --config custom_config.json

# Override parameters
python model/scripts/train_model.py --samples 20000 --epochs 150 --batch-size 64

# Custom output directory
python model/scripts/train_model.py --output-dir /path/to/output
```

## Error Handling

### C++ Error Codes

```cpp
enum class WatermarkError {
    SUCCESS = 0,
    INVALID_CONFIG = 1,
    INITIALIZATION_FAILED = 2,
    FRAME_PROCESSING_ERROR = 3,
    MODEL_LOAD_ERROR = 4,
    INSUFFICIENT_DATA = 5,
    UNKNOWN_ERROR = 99
};
```

### Node.js Error Responses

```json
{
  "error": "Processing failed",
  "code": "WATERMARK_ERROR",
  "details": "Invalid payload format",
  "timestamp": "2024-01-01T00:00:00.000Z"
}
```

### Python Exceptions

```python
class WatermarkError(Exception):
    """Base exception for watermark operations."""
    pass

class ConfigError(WatermarkError):
    """Configuration error."""
    pass

class ModelError(WatermarkError):
    """Model-related error."""
    pass
```

## Examples

### C++ Watermark Embedding

```cpp
#include "watermark_encoder.h"

int main() {
    // Configure watermark
    phantomframe::WatermarkConfig config;
    config.seed = 12345;
    config.block_density = 0.3;
    config.temporal_period = 30;
    
    // Initialize encoder
    phantomframe::WatermarkEncoder encoder(config);
    if (!encoder.initialize(1920, 1080, 30.0)) {
        return -1;
    }
    
    // Process frame
    uint8_t frame_data[1920 * 1080 * 3];
    // ... fill frame data ...
    
    auto watermarked_frame = encoder.processFrame(frame_data, sizeof(frame_data), 0);
    
    return 0;
}
```

### Node.js Watermark Detection

```javascript
const axios = require('axios');

async function detectWatermark(videoPath) {
    try {
        const response = await axios.post('http://localhost:3000/api/detection/analyze', {
            videoPath: videoPath,
            confidenceThreshold: 0.8,
            maxFrames: 1000
        });
        
        const result = response.data.result;
        if (result.watermarkDetected) {
            console.log(`Watermark detected with confidence: ${result.confidence}`);
            console.log(`Payload: ${result.payload}`);
        } else {
            console.log('No watermark detected');
        }
    } catch (error) {
        console.error('Detection failed:', error.response.data);
    }
}
```

### Python Model Training

```python
from model.src.watermark_model import WatermarkDetectionModel
from model.src.data_generator import WatermarkDataGenerator

# Initialize components
config = {
    'input_shape': (64, 64, 3),
    'num_classes': 2,
    'learning_rate': 0.001
}

model = WatermarkDetectionModel(config)
data_gen = WatermarkDataGenerator(config)

# Generate training data
X, y = data_gen.generate_synthetic_data(10000)
train_data, val_data, test_data = data_gen.split_data(X, y)

# Train model
history = model.train(train_data, val_data, epochs=100, batch_size=32)

# Save model
model.save_model('models/watermark_model.h5')
```

## Performance Considerations

- **C++ API**: Optimized for real-time processing, minimal latency
- **Node.js API**: Suitable for batch processing and web services
- **Python API**: Best for model training and research
- **Memory Usage**: Frame processing uses minimal additional memory
- **CPU Usage**: Watermarking adds <5% CPU overhead
- **Storage**: No additional storage required beyond output files

## Security Notes

- Payloads are encrypted using XOR with configurable keys
- Random seeds ensure unique watermark patterns
- No sensitive data is logged or stored
- API endpoints support authentication (implementation dependent)
- File uploads are validated and sanitized

import React, { useState, useCallback } from 'react';
import { useDropzone } from 'react-dropzone';
import { Upload, Search, Download, Settings, Eye, Info, AlertCircle, CheckCircle, XCircle } from 'lucide-react';
import toast from 'react-hot-toast';

const Detection = () => {
  const [uploadedFile, setUploadedFile] = useState(null);
  const [detectionConfig, setDetectionConfig] = useState({
    confidenceThreshold: 0.8,
    analysisMethod: 'ai_enhanced',
    blockSize: 16,
    temporalAnalysis: true,
    batchProcessing: false
  });
  const [isAnalyzing, setIsAnalyzing] = useState(false);
  const [detectionResults, setDetectionResults] = useState(null);
  const [showAdvanced, setShowAdvanced] = useState(false);

  const onDrop = useCallback((acceptedFiles) => {
    if (acceptedFiles.length > 0) {
      const file = acceptedFiles[0];
      if (file.type.startsWith('video/')) {
        setUploadedFile(file);
        setDetectionResults(null);
        toast.success(`Video uploaded: ${file.name}`);
      } else {
        toast.error('Please upload a valid video file');
      }
    }
  }, []);

  const { getRootProps, getInputProps, isDragActive } = useDropzone({
    onDrop,
    accept: {
      'video/*': ['.mp4', '.avi', '.mov', '.mkv', '.webm']
    },
    multiple: false
  });

  const handleConfigChange = (field, value) => {
    setDetectionConfig(prev => ({
      ...prev,
      [field]: value
    }));
  };

  const analyzeVideo = async () => {
    if (!uploadedFile) {
      toast.error('Please upload a video file first');
      return;
    }

    setIsAnalyzing(true);
    
    // Simulate AI analysis
    setTimeout(() => {
      const mockResults = {
        watermarkDetected: Math.random() > 0.3, // 70% chance of detection
        confidence: Math.random() * 0.4 + 0.6, // 60-100% confidence
        payload: Math.random() > 0.5 ? 'a1b2c3d4e5f67890' : null,
        seed: Math.random() > 0.5 ? Math.floor(Math.random() * 1000000) : null,
        blockDensity: Math.random() * 0.3 + 0.1, // 10-40%
        frameAnalysis: {
          totalFrames: Math.floor(Math.random() * 1000) + 500,
          analyzedFrames: Math.floor(Math.random() * 800) + 400,
          watermarkedFrames: Math.floor(Math.random() * 600) + 200
        },
        qualityMetrics: {
          psnr: Math.random() * 10 + 30, // 30-40 dB
          ssim: Math.random() * 0.2 + 0.8, // 0.8-1.0
          mse: Math.random() * 50 + 10 // 10-60
        },
        processingTime: Math.random() * 5 + 2, // 2-7 seconds
        analysisMethod: detectionConfig.analysisMethod
      };
      
      setDetectionResults(mockResults);
      setIsAnalyzing(false);
      
      if (mockResults.watermarkDetected) {
        toast.success('Watermark detected successfully!');
      } else {
        toast.info('No watermark detected in this video');
      }
    }, 4000);
  };

  const downloadReport = () => {
    if (!detectionResults) return;
    
    const report = {
      filename: uploadedFile.name,
      timestamp: new Date().toISOString(),
      results: detectionResults
    };
    
    const blob = new Blob([JSON.stringify(report, null, 2)], { type: 'application/json' });
    const url = URL.createObjectURL(blob);
    const a = document.createElement('a');
    a.href = url;
    a.download = `watermark_detection_report_${Date.now()}.json`;
    document.body.appendChild(a);
    a.click();
    document.body.removeChild(a);
    URL.revokeObjectURL(url);
  };

  const getConfidenceColor = (confidence) => {
    if (confidence >= 0.9) return 'text-green-600';
    if (confidence >= 0.7) return 'text-yellow-600';
    return 'text-red-600';
  };

  const getConfidenceLabel = (confidence) => {
    if (confidence >= 0.9) return 'Very High';
    if (confidence >= 0.7) return 'High';
    if (confidence >= 0.5) return 'Medium';
    return 'Low';
  };

  return (
    <div className="max-w-6xl mx-auto space-y-8">
      {/* Header */}
      <div className="text-center">
        <h1 className="text-4xl font-bold text-gray-900 mb-4">
          Detect Watermarks
        </h1>
        <p className="text-xl text-gray-600 max-w-3xl mx-auto">
          Use our advanced AI-powered detection system to identify and extract invisible watermarks 
          from any video source, even after compression or modification.
        </p>
      </div>

      <div className="grid lg:grid-cols-2 gap-8">
        {/* Left Column - File Upload and Configuration */}
        <div className="space-y-6">
          {/* File Upload */}
          <div className="card">
            <h2 className="text-2xl font-semibold text-gray-900 mb-4 flex items-center">
              <Upload className="w-6 h-6 mr-2 text-blue-600" />
              Upload Video
            </h2>
            
            <div
              {...getRootProps()}
              className={`dropzone ${isDragActive ? 'dropzone-active' : ''} ${
                uploadedFile ? 'dropzone-success' : ''
              }`}
            >
              <input {...getInputProps()} />
              <Upload className="w-12 h-12 text-gray-400 mb-4" />
              {uploadedFile ? (
                <div className="text-center">
                  <p className="text-lg font-medium text-gray-900 mb-2">
                    {uploadedFile.name}
                  </p>
                  <p className="text-sm text-gray-500">
                    {(uploadedFile.size / (1024 * 1024)).toFixed(2)} MB
                  </p>
                </div>
              ) : (
                <div className="text-center">
                  <p className="text-lg font-medium text-gray-900 mb-2">
                    {isDragActive ? 'Drop video here' : 'Drag & drop video here'}
                  </p>
                  <p className="text-sm text-gray-500">
                    or click to select a video file
                  </p>
                </div>
              )}
            </div>
          </div>

          {/* Detection Configuration */}
          <div className="card">
            <h2 className="text-2xl font-semibold text-gray-900 mb-4 flex items-center">
              <Settings className="w-6 h-6 mr-2 text-blue-600" />
              Detection Configuration
            </h2>
            
            <div className="space-y-4">
              {/* Confidence Threshold */}
              <div className="form-group">
                <label className="form-label">Confidence Threshold: {detectionConfig.confidenceThreshold}</label>
                <input
                  type="range"
                  min="0.5"
                  max="0.99"
                  step="0.01"
                  value={detectionConfig.confidenceThreshold}
                  onChange={(e) => handleConfigChange('confidenceThreshold', parseFloat(e.target.value))}
                  className="w-full"
                />
                <p className="text-sm text-gray-500 mt-1">
                  Minimum confidence level for watermark detection (0.5 = 50%, 0.99 = 99%)
                </p>
              </div>

              {/* Analysis Method */}
              <div className="form-group">
                <label className="form-label">Analysis Method</label>
                <select
                  value={detectionConfig.analysisMethod}
                  onChange={(e) => handleConfigChange('analysisMethod', e.target.value)}
                  className="form-select"
                >
                  <option value="ai_enhanced">AI Enhanced (Recommended)</option>
                  <option value="traditional">Traditional Algorithm</option>
                  <option value="hybrid">Hybrid Approach</option>
                  <option value="deep_learning">Deep Learning</option>
                </select>
                <p className="text-sm text-gray-500 mt-1">
                  Choose the detection algorithm based on your needs
                </p>
              </div>

              {/* Block Size */}
              <div className="form-group">
                <label className="form-label">Block Size</label>
                <select
                  value={detectionConfig.blockSize}
                  onChange={(e) => handleConfigChange('blockSize', parseInt(e.target.value))}
                  className="form-select"
                >
                  <option value={8}>8x8 pixels</option>
                  <option value={16}>16x16 pixels</option>
                  <option value={32}>32x32 pixels</option>
                  <option value={64}>64x64 pixels</option>
                </select>
                <p className="text-sm text-gray-500 mt-1">
                  Size of blocks to analyze for watermark patterns
                </p>
              </div>

              {/* Advanced Options */}
              <button
                onClick={() => setShowAdvanced(!showAdvanced)}
                className="text-blue-600 hover:text-blue-700 text-sm font-medium flex items-center"
              >
                {showAdvanced ? 'Hide' : 'Show'} Advanced Options
                <Settings className={`w-4 h-4 ml-1 transition-transform ${showAdvanced ? 'rotate-90' : ''}`} />
              </button>

              {showAdvanced && (
                <div className="space-y-4 pl-4 border-l-2 border-gray-200">
                  <div className="form-group">
                    <label className="form-label flex items-center">
                      <input
                        type="checkbox"
                        checked={detectionConfig.temporalAnalysis}
                        onChange={(e) => handleConfigChange('temporalAnalysis', e.target.checked)}
                        className="mr-2"
                      />
                      Temporal Analysis
                    </label>
                    <p className="text-sm text-gray-500 mt-1">
                      Analyze watermark patterns across multiple frames
                    </p>
                  </div>

                  <div className="form-group">
                    <label className="form-label flex items-center">
                      <input
                        type="checkbox"
                        checked={detectionConfig.batchProcessing}
                        onChange={(e) => handleConfigChange('batchProcessing', e.target.checked)}
                        className="mr-2"
                      />
                      Batch Processing
                    </label>
                    <p className="text-sm text-gray-500 mt-1">
                      Process multiple video segments simultaneously
                    </p>
                  </div>
                </div>
              )}
            </div>
          </div>

          {/* Analyze Button */}
          <button
            onClick={analyzeVideo}
            disabled={!uploadedFile || isAnalyzing}
            className="btn-primary w-full py-4 text-lg disabled:opacity-50 disabled:cursor-not-allowed"
          >
            {isAnalyzing ? (
              <>
                <div className="spinner spinner-sm mr-2"></div>
                Analyzing Video...
              </>
            ) : (
              <>
                <Search className="w-5 h-5 mr-2" />
                Detect Watermark
              </>
            )}
          </button>
        </div>

        {/* Right Column - Preview and Results */}
        <div className="space-y-6">
          {/* Video Preview */}
          <div className="card">
            <h2 className="text-2xl font-semibold text-gray-900 mb-4 flex items-center">
              <Eye className="w-6 h-6 mr-2 text-blue-600" />
              Video Preview
            </h2>
            
            {uploadedFile ? (
              <div className="aspect-video bg-gray-100 rounded-lg flex items-center justify-center">
                <video
                  src={URL.createObjectURL(uploadedFile)}
                  controls
                  className="w-full h-full rounded-lg"
                />
              </div>
            ) : (
              <div className="aspect-video bg-gray-100 rounded-lg flex items-center justify-center">
                <div className="text-center text-gray-500">
                  <Eye className="w-16 h-16 mx-auto mb-2" />
                  <p>Upload a video to see preview</p>
                </div>
              </div>
            )}
          </div>

          {/* Analysis Status */}
          {isAnalyzing && (
            <div className="card">
              <h3 className="text-lg font-semibold text-gray-900 mb-4">Analysis Status</h3>
              <div className="space-y-3">
                <div className="flex items-center justify-between">
                  <span className="text-sm text-gray-600">Loading AI model...</span>
                  <div className="spinner spinner-sm"></div>
                </div>
                <div className="flex items-center justify-between">
                  <span className="text-sm text-gray-600">Extracting frames...</span>
                  <div className="spinner spinner-sm"></div>
                </div>
                <div className="flex items-center justify-between">
                  <span className="text-sm text-gray-600">Analyzing patterns...</span>
                  <div className="spinner spinner-sm"></div>
                </div>
                <div className="flex items-center justify-between">
                  <span className="text-sm text-gray-600">Processing results...</span>
                  <div className="spinner spinner-sm"></div>
                </div>
              </div>
            </div>
          )}

          {/* Detection Results */}
          {detectionResults && (
            <div className="card">
              <h3 className="text-lg font-semibold text-gray-900 mb-4 flex items-center">
                {detectionResults.watermarkDetected ? (
                  <CheckCircle className="w-5 h-5 mr-2 text-green-600" />
                ) : (
                  <XCircle className="w-5 h-5 mr-2 text-red-600" />
                )}
                {detectionResults.watermarkDetected ? 'Watermark Detected' : 'No Watermark Found'}
              </h3>
              
              <div className="space-y-4">
                {/* Main Result */}
                <div className={`p-4 rounded-lg border ${
                  detectionResults.watermarkDetected 
                    ? 'bg-green-50 border-green-200' 
                    : 'bg-gray-50 border-gray-200'
                }`}>
                  <div className="flex items-center justify-between">
                    <span className="font-medium text-gray-900">
                      Detection Result
                    </span>
                    <span className={`font-semibold ${
                      detectionResults.watermarkDetected ? 'text-green-600' : 'text-gray-600'
                    }`}>
                      {detectionResults.watermarkDetected ? 'DETECTED' : 'NOT DETECTED'}
                    </span>
                  </div>
                </div>

                {/* Confidence Score */}
                <div className="bg-blue-50 border border-blue-200 rounded-lg p-4">
                  <div className="flex items-center justify-between mb-2">
                    <span className="font-medium text-blue-900">Confidence Score</span>
                    <span className={`font-bold text-lg ${getConfidenceColor(detectionResults.confidence)}`}>
                      {(detectionResults.confidence * 100).toFixed(1)}%
                    </span>
                  </div>
                  <div className="w-full bg-blue-200 rounded-full h-2">
                    <div 
                      className="bg-blue-600 h-2 rounded-full transition-all duration-300"
                      style={{ width: `${detectionResults.confidence * 100}%` }}
                    ></div>
                  </div>
                  <p className="text-sm text-blue-700 mt-1">
                    {getConfidenceLabel(detectionResults.confidence)} confidence level
                  </p>
                </div>

                {/* Watermark Details */}
                {detectionResults.watermarkDetected && (
                  <div className="space-y-3">
                    <h4 className="font-medium text-gray-900">Watermark Details</h4>
                    
                    {detectionResults.payload && (
                      <div className="flex justify-between items-center">
                        <span className="text-sm text-gray-600">Payload:</span>
                        <span className="font-mono text-sm bg-gray-100 px-2 py-1 rounded">
                          {detectionResults.payload}
                        </span>
                      </div>
                    )}
                    
                    {detectionResults.seed && (
                      <div className="flex justify-between items-center">
                        <span className="text-sm text-gray-600">Seed:</span>
                        <span className="font-mono text-sm bg-gray-100 px-2 py-1 rounded">
                          {detectionResults.seed}
                        </span>
                      </div>
                    )}
                    
                    <div className="flex justify-between items-center">
                      <span className="text-sm text-gray-600">Block Density:</span>
                      <span className="text-sm font-medium">
                        {(detectionResults.blockDensity * 100).toFixed(1)}%
                      </span>
                    </div>
                  </div>
                )}

                {/* Frame Analysis */}
                <div className="border-t pt-4">
                  <h4 className="font-medium text-gray-900 mb-3">Frame Analysis</h4>
                  <div className="grid grid-cols-3 gap-4 text-center">
                    <div>
                      <div className="text-2xl font-bold text-blue-600">
                        {detectionResults.frameAnalysis.totalFrames}
                      </div>
                      <div className="text-xs text-gray-500">Total Frames</div>
                    </div>
                    <div>
                      <div className="text-2xl font-bold text-green-600">
                        {detectionResults.frameAnalysis.analyzedFrames}
                      </div>
                      <div className="text-xs text-gray-500">Analyzed</div>
                    </div>
                    <div>
                      <div className="text-2xl font-bold text-purple-600">
                        {detectionResults.frameAnalysis.watermarkedFrames}
                      </div>
                      <div className="text-xs text-gray-500">Watermarked</div>
                    </div>
                  </div>
                </div>

                {/* Quality Metrics */}
                <div className="border-t pt-4">
                  <h4 className="font-medium text-gray-900 mb-3">Quality Metrics</h4>
                  <div className="space-y-2">
                    <div className="flex justify-between items-center">
                      <span className="text-sm text-gray-600">PSNR:</span>
                      <span className="text-sm font-medium">
                        {detectionResults.qualityMetrics.psnr.toFixed(2)} dB
                      </span>
                    </div>
                    <div className="flex justify-between items-center">
                      <span className="text-sm text-gray-600">SSIM:</span>
                      <span className="text-sm font-medium">
                        {detectionResults.qualityMetrics.ssim.toFixed(3)}
                      </span>
                    </div>
                    <div className="flex justify-between items-center">
                      <span className="text-sm text-gray-600">MSE:</span>
                      <span className="text-sm font-medium">
                        {detectionResults.qualityMetrics.mse.toFixed(2)}
                      </span>
                    </div>
                  </div>
                </div>

                {/* Processing Info */}
                <div className="border-t pt-4">
                  <div className="flex justify-between items-center text-sm text-gray-600">
                    <span>Processing Time:</span>
                    <span>{detectionResults.processingTime.toFixed(2)} seconds</span>
                  </div>
                  <div className="flex justify-between items-center text-sm text-gray-600">
                    <span>Method:</span>
                    <span className="capitalize">{detectionResults.analysisMethod.replace('_', ' ')}</span>
                  </div>
                </div>

                {/* Download Report */}
                <button
                  onClick={downloadReport}
                  className="btn-secondary w-full flex items-center justify-center"
                >
                  <Download className="w-5 h-5 mr-2" />
                  Download Analysis Report
                </button>
              </div>
            </div>
          )}

          {/* Info Panel */}
          <div className="card bg-blue-50 border-blue-200">
            <div className="flex items-start">
              <Info className="w-5 h-5 text-blue-600 mr-2 mt-0.5 flex-shrink-0" />
              <div>
                <h4 className="font-medium text-blue-900 mb-2">Detection Tips</h4>
                <ul className="text-sm text-blue-800 space-y-1">
                  <li>• Higher confidence thresholds reduce false positives</li>
                  <li>• AI Enhanced method provides best accuracy</li>
                  <li>• Temporal analysis improves detection reliability</li>
                  <li>• Results include quality metrics and frame analysis</li>
                </ul>
              </div>
            </div>
          </div>
        </div>
      </div>
    </div>
  );
};

export default Detection;

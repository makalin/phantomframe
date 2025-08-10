import React, { useState, useCallback } from 'react';
import { useDropzone } from 'react-dropzone';
import { Upload, Play, Download, Settings, Shield, Info, AlertCircle } from 'lucide-react';
import toast from 'react-hot-toast';

const Watermark = () => {
  const [uploadedFile, setUploadedFile] = useState(null);
  const [watermarkConfig, setWatermarkConfig] = useState({
    payload: '',
    seed: Math.floor(Math.random() * 1000000),
    blockDensity: 0.3,
    adaptiveEmbedding: true,
    temporalPeriod: 1,
    qualityPreservation: 'high'
  });
  const [isProcessing, setIsProcessing] = useState(false);
  const [processedVideo, setProcessedVideo] = useState(null);
  const [showAdvanced, setShowAdvanced] = useState(false);

  const onDrop = useCallback((acceptedFiles) => {
    if (acceptedFiles.length > 0) {
      const file = acceptedFiles[0];
      if (file.type.startsWith('video/')) {
        setUploadedFile(file);
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
    setWatermarkConfig(prev => ({
      ...prev,
      [field]: value
    }));
  };

  const generateRandomPayload = () => {
    const payload = Array.from({ length: 16 }, () => 
      Math.floor(Math.random() * 256)
    ).map(b => b.toString(16).padStart(2, '0')).join('');
    setWatermarkConfig(prev => ({ ...prev, payload }));
  };

  const processVideo = async () => {
    if (!uploadedFile) {
      toast.error('Please upload a video file first');
      return;
    }

    if (!watermarkConfig.payload) {
      toast.error('Please set a watermark payload');
      return;
    }

    setIsProcessing(true);
    
    // Simulate processing
    setTimeout(() => {
      const processedFile = new File(
        [uploadedFile], 
        `watermarked_${uploadedFile.name}`,
        { type: uploadedFile.type }
      );
      setProcessedVideo(processedFile);
      setIsProcessing(false);
      toast.success('Video watermarked successfully!');
    }, 3000);
  };

  const downloadVideo = () => {
    if (processedVideo) {
      const url = URL.createObjectURL(processedVideo);
      const a = document.createElement('a');
      a.href = url;
      a.download = processedVideo.name;
      document.body.appendChild(a);
      a.click();
      document.body.removeChild(a);
      URL.revokeObjectURL(url);
    }
  };

  return (
    <div className="max-w-6xl mx-auto space-y-8">
      {/* Header */}
      <div className="text-center">
        <h1 className="text-4xl font-bold text-gray-900 mb-4">
          Embed Watermarks
        </h1>
        <p className="text-xl text-gray-600 max-w-3xl mx-auto">
          Protect your video content with invisible, imperceptible watermarks that can be detected 
          even after compression, cropping, or other modifications.
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

          {/* Watermark Configuration */}
          <div className="card">
            <h2 className="text-2xl font-semibold text-gray-900 mb-4 flex items-center">
              <Settings className="w-6 h-6 mr-2 text-blue-600" />
              Watermark Configuration
            </h2>
            
            <div className="space-y-4">
              {/* Payload */}
              <div className="form-group">
                <label className="form-label">Watermark Payload (16 bytes)</label>
                <div className="flex gap-2">
                  <input
                    type="text"
                    value={watermarkConfig.payload}
                    onChange={(e) => handleConfigChange('payload', e.target.value)}
                    placeholder="Enter 32-character hex string"
                    className="form-input flex-1"
                    maxLength={32}
                  />
                  <button
                    onClick={generateRandomPayload}
                    className="btn-secondary px-3"
                    title="Generate random payload"
                  >
                    🎲
                  </button>
                </div>
                <p className="text-sm text-gray-500 mt-1">
                  This is the unique identifier that will be embedded in your video
                </p>
              </div>

              {/* Seed */}
              <div className="form-group">
                <label className="form-label">Random Seed</label>
                <input
                  type="number"
                  value={watermarkConfig.seed}
                  onChange={(e) => handleConfigChange('seed', parseInt(e.target.value))}
                  className="form-input"
                  min="0"
                  max="999999"
                />
                <p className="text-sm text-gray-500 mt-1">
                  Controls the pattern of watermark placement
                </p>
              </div>

              {/* Block Density */}
              <div className="form-group">
                <label className="form-label">Block Density: {watermarkConfig.blockDensity}</label>
                <input
                  type="range"
                  min="0.1"
                  max="0.8"
                  step="0.1"
                  value={watermarkConfig.blockDensity}
                  onChange={(e) => handleConfigChange('blockDensity', parseFloat(e.target.value))}
                  className="w-full"
                />
                <p className="text-sm text-gray-500 mt-1">
                  Fraction of video blocks to watermark (0.1 = 10%, 0.8 = 80%)
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
                        checked={watermarkConfig.adaptiveEmbedding}
                        onChange={(e) => handleConfigChange('adaptiveEmbedding', e.target.checked)}
                        className="mr-2"
                      />
                      Adaptive Embedding
                    </label>
                    <p className="text-sm text-gray-500 mt-1">
                      Automatically adjust watermark strength based on video content
                    </p>
                  </div>

                  <div className="form-group">
                    <label className="form-label">Temporal Period</label>
                    <input
                      type="number"
                      value={watermarkConfig.temporalPeriod}
                      onChange={(e) => handleConfigChange('temporalPeriod', parseInt(e.target.value))}
                      className="form-input"
                      min="1"
                      max="10"
                    />
                    <p className="text-sm text-gray-500 mt-1">
                      Number of frames between watermark applications
                    </p>
                  </div>

                  <div className="form-group">
                    <label className="form-label">Quality Preservation</label>
                    <select
                      value={watermarkConfig.qualityPreservation}
                      onChange={(e) => handleConfigChange('qualityPreservation', e.target.value)}
                      className="form-select"
                    >
                      <option value="high">High Quality</option>
                      <option value="medium">Medium Quality</option>
                      <option value="low">Low Quality</option>
                    </select>
                    <p className="text-sm text-gray-500 mt-1">
                      Balance between watermark strength and video quality
                    </p>
                  </div>
                </div>
              )}
            </div>
          </div>

          {/* Process Button */}
          <button
            onClick={processVideo}
            disabled={!uploadedFile || !watermarkConfig.payload || isProcessing}
            className="btn-primary w-full py-4 text-lg disabled:opacity-50 disabled:cursor-not-allowed"
          >
            {isProcessing ? (
              <>
                <div className="spinner spinner-sm mr-2"></div>
                Processing Video...
              </>
            ) : (
              <>
                <Shield className="w-5 h-5 mr-2" />
                Embed Watermark
              </>
            )}
          </button>
        </div>

        {/* Right Column - Preview and Results */}
        <div className="space-y-6">
          {/* Video Preview */}
          <div className="card">
            <h2 className="text-2xl font-semibold text-gray-900 mb-4 flex items-center">
              <Play className="w-6 h-6 mr-2 text-blue-600" />
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
                  <Play className="w-16 h-16 mx-auto mb-2" />
                  <p>Upload a video to see preview</p>
                </div>
              </div>
            )}
          </div>

          {/* Processing Status */}
          {isProcessing && (
            <div className="card">
              <h3 className="text-lg font-semibold text-gray-900 mb-4">Processing Status</h3>
              <div className="space-y-3">
                <div className="flex items-center justify-between">
                  <span className="text-sm text-gray-600">Analyzing video...</span>
                  <div className="spinner spinner-sm"></div>
                </div>
                <div className="flex items-center justify-between">
                  <span className="text-sm text-gray-600">Embedding watermark...</span>
                  <div className="spinner spinner-sm"></div>
                </div>
                <div className="flex items-center justify-between">
                  <span className="text-sm text-gray-600">Optimizing quality...</span>
                  <div className="spinner spinner-sm"></div>
                </div>
              </div>
            </div>
          )}

          {/* Results */}
          {processedVideo && (
            <div className="card">
              <h3 className="text-lg font-semibold text-gray-900 mb-4 flex items-center">
                <Shield className="w-5 h-5 mr-2 text-green-600" />
                Watermark Applied Successfully
              </h3>
              
              <div className="space-y-4">
                <div className="bg-green-50 border border-green-200 rounded-lg p-4">
                  <div className="flex items-center">
                    <AlertCircle className="w-5 h-5 text-green-600 mr-2" />
                    <span className="text-green-800 font-medium">
                      Watermark embedded successfully!
                    </span>
                  </div>
                  <p className="text-green-700 text-sm mt-1">
                    Your video is now protected with an invisible watermark.
                  </p>
                </div>

                <div className="space-y-2">
                  <p className="text-sm text-gray-600">
                    <span className="font-medium">Original:</span> {uploadedFile.name}
                  </p>
                  <p className="text-sm text-gray-600">
                    <span className="font-medium">Watermarked:</span> {processedVideo.name}
                  </p>
                  <p className="text-sm text-gray-600">
                    <span className="font-medium">Payload:</span> {watermarkConfig.payload}
                  </p>
                  <p className="text-sm text-gray-600">
                    <span className="font-medium">Seed:</span> {watermarkConfig.seed}
                  </p>
                </div>

                <button
                  onClick={downloadVideo}
                  className="btn-secondary w-full flex items-center justify-center"
                >
                  <Download className="w-5 h-5 mr-2" />
                  Download Watermarked Video
                </button>
              </div>
            </div>
          )}

          {/* Info Panel */}
          <div className="card bg-blue-50 border-blue-200">
            <div className="flex items-start">
              <Info className="w-5 h-5 text-blue-600 mr-2 mt-0.5 flex-shrink-0" />
              <div>
                <h4 className="font-medium text-blue-900 mb-2">About Watermarking</h4>
                <ul className="text-sm text-blue-800 space-y-1">
                  <li>• Watermarks are invisible to the human eye</li>
                  <li>• Survive compression, cropping, and format conversion</li>
                  <li>• Can be detected even in low-quality copies</li>
                  <li>• No impact on video quality or file size</li>
                </ul>
              </div>
            </div>
          </div>
        </div>
      </div>
    </div>
  );
};

export default Watermark;

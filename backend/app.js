#!/usr/bin/env node
/**
 * PhantomFrame Backend Service
 * Express.js server for video watermarking API
 */

const express = require('express');
const cors = require('cors');
const helmet = require('helmet');
const rateLimit = require('express-rate-limit');
const compression = require('compression');
const morgan = require('morgan');
const path = require('path');
const fs = require('fs');
const multer = require('multer');
const { v4: uuidv4 } = require('uuid');
const bcrypt = require('bcryptjs');
const jwt = require('jsonwebtoken');
const { Pool } = require('pg');
const Redis = require('ioredis');
const prometheus = require('prom-client');

// Import routes
const authRoutes = require('./routes/auth');
const watermarkRoutes = require('./routes/watermark');
const detectionRoutes = require('./routes/detection');
const dashboardRoutes = require('./routes/dashboard');
const adminRoutes = require('./routes/admin');

// Import middleware
const { authenticateToken, requireRole } = require('./middleware/auth');
const { validateFileUpload } = require('./middleware/validation');
const { errorHandler, notFoundHandler } = require('./middleware/error');

// Import services
const WatermarkService = require('./services/watermarkService');
const DetectionService = require('./services/detectionService');
const VideoProcessor = require('./services/videoProcessor');
const MLServiceClient = require('./services/mlServiceClient');

// Import utilities
const logger = require('./utils/logger');
const config = require('./config');
const db = require('./utils/database');
const redis = require('./utils/redis');

// Create Express app
const app = express();
const PORT = process.env.PORT || 3001;

// Prometheus metrics
const collectDefaultMetrics = prometheus.collectDefaultMetrics;
collectDefaultMetrics({ timeout: 5000 });

const httpRequestDurationMicroseconds = new prometheus.Histogram({
  name: 'http_request_duration_seconds',
  help: 'Duration of HTTP requests in seconds',
  labelNames: ['method', 'route', 'status_code'],
  buckets: [0.1, 0.5, 1, 2, 5]
});

const httpRequestsTotal = new prometheus.Counter({
  name: 'http_requests_total',
  help: 'Total number of HTTP requests',
  labelNames: ['method', 'route', 'status_code']
});

// Security middleware
app.use(helmet({
  contentSecurityPolicy: {
    directives: {
      defaultSrc: ["'self'"],
      styleSrc: ["'self'", "'unsafe-inline'"],
      scriptSrc: ["'self'"],
      imgSrc: ["'self'", "data:", "https:"],
      connectSrc: ["'self'", "ws:", "wss:"]
    }
  }
}));

// CORS configuration
app.use(cors({
  origin: process.env.CORS_ORIGIN || 'http://localhost:3000',
  credentials: true,
  methods: ['GET', 'POST', 'PUT', 'DELETE', 'OPTIONS'],
  allowedHeaders: ['Content-Type', 'Authorization', 'X-Requested-With']
}));

// Rate limiting
const limiter = rateLimit({
  windowMs: 15 * 60 * 1000, // 15 minutes
  max: 100, // limit each IP to 100 requests per windowMs
  message: 'Too many requests from this IP, please try again later.',
  standardHeaders: true,
  legacyHeaders: false
});

const uploadLimiter = rateLimit({
  windowMs: 15 * 60 * 1000, // 15 minutes
  max: 10, // limit each IP to 10 uploads per windowMs
  message: 'Too many uploads from this IP, please try again later.'
});

app.use('/api/', limiter);
app.use('/api/upload', uploadLimiter);

// Compression middleware
app.use(compression());

// Logging middleware
app.use(morgan('combined', { stream: { write: message => logger.info(message.trim()) } }));

// Body parsing middleware
app.use(express.json({ limit: '10mb' }));
app.use(express.urlencoded({ extended: true, limit: '10mb' }));

// Static file serving
app.use('/uploads', express.static(path.join(__dirname, 'uploads')));
app.use('/processed', express.static(path.join(__dirname, 'processed')));

// Request timing middleware
app.use((req, res, next) => {
  const start = Date.now();
  res.on('finish', () => {
    const duration = Date.now() - start;
    const durationSeconds = duration / 1000;
    
    httpRequestDurationMicroseconds
      .labels(req.method, req.route?.path || req.path, res.statusCode)
      .observe(durationSeconds);
    
    httpRequestsTotal
      .labels(req.method, req.route?.path || req.path, res.statusCode)
      .inc();
  });
  next();
});

// Health check endpoint
app.get('/health', async (req, res) => {
  try {
    // Check database connection
    const dbClient = await db.getClient();
    await dbClient.query('SELECT 1');
    dbClient.release();
    
    // Check Redis connection
    await redis.ping();
    
    res.json({
      status: 'healthy',
      timestamp: new Date().toISOString(),
      service: 'phantomframe-backend',
      version: '1.0.0',
      uptime: process.uptime(),
      memory: process.memoryUsage(),
      environment: process.env.NODE_ENV || 'development'
    });
  } catch (error) {
    logger.error('Health check failed:', error);
    res.status(503).json({
      status: 'unhealthy',
      error: error.message,
      timestamp: new Date().toISOString()
    });
  }
});

// Metrics endpoint for Prometheus
app.get('/metrics', async (req, res) => {
  try {
    res.set('Content-Type', prometheus.register.contentType);
    res.end(await prometheus.register.metrics());
  } catch (error) {
    logger.error('Metrics generation failed:', error);
    res.status(500).end(error);
  }
});

// API routes
app.use('/api/auth', authRoutes);
app.use('/api/watermark', authenticateToken, watermarkRoutes);
app.use('/api/detection', authenticateToken, detectionRoutes);
app.use('/api/dashboard', authenticateToken, dashboardRoutes);
app.use('/api/admin', authenticateToken, requireRole('admin'), adminRoutes);

// File upload configuration
const storage = multer.diskStorage({
  destination: (req, file, cb) => {
    const uploadDir = path.join(__dirname, 'uploads');
    if (!fs.existsSync(uploadDir)) {
      fs.mkdirSync(uploadDir, { recursive: true });
    }
    cb(null, uploadDir);
  },
  filename: (req, file, cb) => {
    const uniqueName = `${uuidv4()}_${file.originalname}`;
    cb(null, uniqueName);
  }
});

const fileFilter = (req, file, cb) => {
  const allowedTypes = ['video/mp4', 'video/avi', 'video/mov', 'video/mkv', 'video/webm'];
  
  if (allowedTypes.includes(file.mimetype)) {
    cb(null, true);
  } else {
    cb(new Error('Invalid file type. Only video files are allowed.'), false);
  }
};

const upload = multer({
  storage: storage,
  fileFilter: fileFilter,
  limits: {
    fileSize: 100 * 1024 * 1024, // 100MB limit
    files: 1
  }
});

// File upload endpoint
app.post('/api/upload', authenticateToken, upload.single('video'), validateFileUpload, async (req, res) => {
  try {
    if (!req.file) {
      return res.status(400).json({ error: 'No video file uploaded' });
    }

    const userId = req.user.id;
    const fileInfo = {
      originalName: req.file.originalname,
      filename: req.file.filename,
      path: req.file.path,
      size: req.file.size,
      mimetype: req.file.mimetype
    };

    // Get video metadata
    const videoProcessor = new VideoProcessor();
    const metadata = await videoProcessor.getVideoMetadata(fileInfo.path);

    // Save to database
    const dbClient = await db.getClient();
    const result = await dbClient.query(
      `INSERT INTO videos (user_id, original_filename, stored_filename, file_path, file_size, duration, width, height, frame_rate, codec, bitrate, status)
       VALUES ($1, $2, $3, $4, $5, $6, $7, $8, $9, $10, $11, $12)
       RETURNING id`,
      [
        userId,
        fileInfo.originalName,
        fileInfo.filename,
        fileInfo.path,
        fileInfo.size,
        metadata.duration,
        metadata.width,
        metadata.height,
        metadata.frameRate,
        metadata.codec,
        metadata.bitrate,
        'uploaded'
      ]
    );

    dbClient.release();

    // Log upload
    logger.info(`Video uploaded by user ${userId}: ${fileInfo.originalName} (${fileInfo.size} bytes)`);

    res.json({
      message: 'Video uploaded successfully',
      videoId: result.rows[0].id,
      filename: fileInfo.filename,
      size: fileInfo.size,
      metadata: metadata
    });

  } catch (error) {
    logger.error('File upload failed:', error);
    
    // Clean up uploaded file on error
    if (req.file && fs.existsSync(req.file.path)) {
      fs.unlinkSync(req.file.path);
    }
    
    res.status(500).json({ error: 'File upload failed', details: error.message });
  }
});

// Error handling middleware
app.use(notFoundHandler);
app.use(errorHandler);

// Graceful shutdown
process.on('SIGTERM', async () => {
  logger.info('SIGTERM received, shutting down gracefully...');
  
  try {
    await db.close();
    await redis.quit();
    process.exit(0);
  } catch (error) {
    logger.error('Error during shutdown:', error);
    process.exit(1);
  }
});

process.on('SIGINT', async () => {
  logger.info('SIGINT received, shutting down gracefully...');
  
  try {
    await db.close();
    await redis.quit();
    process.exit(0);
  } catch (error) {
    logger.error('Error during shutdown:', error);
    process.exit(1);
  }
});

// Start server
const server = app.listen(PORT, () => {
  logger.info(`PhantomFrame Backend Service running on port ${PORT}`);
  logger.info(`Environment: ${process.env.NODE_ENV || 'development'}`);
  logger.info(`Health check: http://localhost:${PORT}/health`);
  logger.info(`API Documentation: http://localhost:${PORT}/api-docs`);
  logger.info(`Metrics: http://localhost:${PORT}/metrics`);
});

// Handle server errors
server.on('error', (error) => {
  if (error.syscall !== 'listen') {
    throw error;
  }

  const bind = typeof PORT === 'string' ? 'Pipe ' + PORT : 'Port ' + PORT;

  switch (error.code) {
    case 'EACCES':
      logger.error(`${bind} requires elevated privileges`);
      process.exit(1);
      break;
    case 'EADDRINUSE':
      logger.error(`${bind} is already in use`);
      process.exit(1);
      break;
    default:
      throw error;
  }
});

module.exports = app;

#!/usr/bin/env python3
"""
PhantomFrame ML Service
FastAPI application for machine learning-based video watermarking and detection
"""

import os
import logging
import asyncio
from typing import Optional, Dict, Any, List
from pathlib import Path
import uuid
import json
from datetime import datetime, timedelta

import uvicorn
from fastapi import FastAPI, HTTPException, UploadFile, File, Form, BackgroundTasks, Depends
from fastapi.middleware.cors import CORSMiddleware
from fastapi.responses import JSONResponse, FileResponse
from fastapi.staticfiles import StaticFiles
import numpy as np
from pydantic import BaseModel, Field
import psycopg2
import redis
from prometheus_client import Counter, Histogram, Gauge, generate_latest, CONTENT_TYPE_LATEST

# Configure logging
logging.basicConfig(
    level=logging.INFO,
    format='%(asctime)s - %(name)s - %(levelname)s - %(message)s',
    handlers=[
        logging.FileHandler('logs/ml_service.log'),
        logging.StreamHandler()
    ]
)
logger = logging.getLogger(__name__)

# Prometheus metrics
REQUEST_COUNT = Counter('ml_service_requests_total', 'Total requests', ['endpoint', 'method'])
REQUEST_DURATION = Histogram('ml_service_request_duration_seconds', 'Request duration')
ACTIVE_JOBS = Gauge('ml_service_active_jobs', 'Number of active processing jobs')
MODEL_LOAD_TIME = Histogram('ml_service_model_load_time_seconds', 'Model loading time')

# Pydantic models
class WatermarkRequest(BaseModel):
    payload: str = Field(..., description="Watermark payload to embed")
    seed: int = Field(..., description="Random seed for watermark generation")
    block_density: float = Field(0.3, ge=0.1, le=0.9, description="Block density for embedding")
    adaptive_embedding: bool = Field(True, description="Use adaptive embedding")
    temporal_period: int = Field(1, ge=1, le=10, description="Temporal embedding period")
    quality_preservation: str = Field("high", regex="^(low|medium|high)$", description="Quality preservation level")

class DetectionRequest(BaseModel):
    confidence_threshold: float = Field(0.8, ge=0.1, le=1.0, description="Detection confidence threshold")
    analysis_method: str = Field("ai_enhanced", regex="^(basic|ai_enhanced|deep_learning)$", description="Analysis method")
    block_size: int = Field(16, ge=8, le=64, description="Block size for analysis")
    temporal_analysis: bool = Field(True, description="Enable temporal analysis")
    batch_processing: bool = Field(False, description="Enable batch processing")

class ProcessingJob(BaseModel):
    id: str
    status: str
    progress: float
    created_at: datetime
    updated_at: datetime
    result: Optional[Dict[str, Any]] = None
    error: Optional[str] = None

# Database connection
def get_db_connection():
    try:
        conn = psycopg2.connect(
            host=os.getenv('POSTGRES_HOST', 'postgres'),
            database=os.getenv('POSTGRES_DB', 'phantomframe'),
            user=os.getenv('POSTGRES_USER', 'phantomframe_user'),
            password=os.getenv('POSTGRES_PASSWORD', 'phantomframe_password'),
            port=os.getenv('POSTGRES_PORT', '5432')
        )
        return conn
    except Exception as e:
        logger.error(f"Database connection failed: {e}")
        raise HTTPException(status_code=500, detail="Database connection failed")

# Redis connection
def get_redis_connection():
    try:
        r = redis.Redis(
            host=os.getenv('REDIS_HOST', 'redis'),
            port=int(os.getenv('REDIS_PORT', '6379')),
            db=0,
            decode_responses=True
        )
        r.ping()
        return r
    except Exception as e:
        logger.error(f"Redis connection failed: {e}")
        raise HTTPException(status_code=500, detail="Redis connection failed")

# Create FastAPI app
app = FastAPI(
    title="PhantomFrame ML Service",
    description="Machine learning service for video watermarking and detection",
    version="1.0.0",
    docs_url="/docs",
    redoc_url="/redoc"
)

# Add CORS middleware
app.add_middleware(
    CORSMiddleware,
    allow_origins=["*"],  # Configure appropriately for production
    allow_credentials=True,
    allow_methods=["*"],
    allow_headers=["*"],
)

# Mount static files
app.mount("/static", StaticFiles(directory="static"), name="static")

# Create necessary directories
os.makedirs("uploads", exist_ok=True)
os.makedirs("processed", exist_ok=True)
os.makedirs("logs", exist_ok=True)
os.makedirs("models", exist_ok=True)
os.makedirs("data", exist_ok=True)

class WatermarkProcessor:
    """Simulated watermark processor for demonstration"""
    
    def __init__(self):
        self.models_loaded = False
        self.load_models()
    
    def load_models(self):
        """Simulate model loading"""
        logger.info("Loading watermarking models...")
        # In real implementation, load TensorFlow/PyTorch models
        self.models_loaded = True
        logger.info("Models loaded successfully")
    
    async def embed_watermark(self, video_path: str, config: WatermarkRequest) -> Dict[str, Any]:
        """Embed watermark in video"""
        if not self.models_loaded:
            raise Exception("Models not loaded")
        
        # Simulate processing time
        await asyncio.sleep(2)
        
        # Generate simulated results
        result = {
            "watermark_id": str(uuid.uuid4()),
            "payload": config.payload,
            "seed": config.seed,
            "block_density": config.block_density,
            "adaptive_embedding": config.adaptive_embedding,
            "temporal_period": config.temporal_period,
            "quality_preservation": config.quality_preservation,
            "processing_time": 2.1,
            "output_path": f"processed/watermarked_{Path(video_path).stem}.mp4",
            "quality_metrics": {
                "psnr": 38.5,
                "ssim": 0.92,
                "bitrate_change": 0.05
            }
        }
        
        return result
    
    async def extract_watermark(self, video_path: str, config: DetectionRequest) -> Dict[str, Any]:
        """Extract watermark from video"""
        if not self.models_loaded:
            raise Exception("Models not loaded")
        
        # Simulate processing time
        await asyncio.sleep(3)
        
        # Simulate detection results
        detected = np.random.random() > 0.3  # 70% detection rate
        
        if detected:
            result = {
                "detected": True,
                "confidence": 0.87,
                "payload": "Sample watermark payload",
                "seed": 12345,
                "block_density": 0.3,
                "processing_time": 3.2,
                "frame_analysis": {
                    "total_frames": 1500,
                    "analyzed_frames": 1500,
                    "detection_frames": 1420
                },
                "quality_metrics": {
                    "psnr": 35.2,
                    "ssim": 0.89
                }
            }
        else:
            result = {
                "detected": False,
                "confidence": 0.12,
                "processing_time": 3.1,
                "frame_analysis": {
                    "total_frames": 1500,
                    "analyzed_frames": 1500,
                    "detection_frames": 0
                }
            }
        
        return result

# Initialize processor
processor = WatermarkProcessor()

@app.on_event("startup")
async def startup_event():
    """Initialize service on startup"""
    logger.info("Starting PhantomFrame ML Service...")
    
    # Test database connection
    try:
        conn = get_db_connection()
        conn.close()
        logger.info("Database connection successful")
    except Exception as e:
        logger.error(f"Database connection failed: {e}")
    
    # Test Redis connection
    try:
        r = get_redis_connection()
        r.ping()
        logger.info("Redis connection successful")
    except Exception as e:
        logger.error(f"Redis connection failed: {e}")
    
    logger.info("ML Service started successfully")

@app.get("/health")
async def health_check():
    """Health check endpoint"""
    try:
        # Check database
        conn = get_db_connection()
        conn.close()
        
        # Check Redis
        r = get_redis_connection()
        r.ping()
        
        return {
            "status": "healthy",
            "timestamp": datetime.utcnow().isoformat(),
            "service": "phantomframe-ml",
            "version": "1.0.0"
        }
    except Exception as e:
        logger.error(f"Health check failed: {e}")
        raise HTTPException(status_code=503, detail="Service unhealthy")

@app.get("/metrics")
async def metrics():
    """Prometheus metrics endpoint"""
    return Response(generate_latest(), media_type=CONTENT_TYPE_LATEST)

@app.post("/watermark/embed")
async def embed_watermark(
    background_tasks: BackgroundTasks,
    file: UploadFile = File(...),
    payload: str = Form(...),
    seed: int = Form(12345),
    block_density: float = Form(0.3),
    adaptive_embedding: bool = Form(True),
    temporal_period: int = Form(1),
    quality_preservation: str = Form("high")
):
    """Embed watermark in uploaded video"""
    REQUEST_COUNT.labels(endpoint="/watermark/embed", method="POST").inc()
    
    # Validate file
    if not file.filename.lower().endswith(('.mp4', '.avi', '.mov', '.mkv', '.webm')):
        raise HTTPException(status_code=400, detail="Invalid video format")
    
    # Create job
    job_id = str(uuid.uuid4())
    job = ProcessingJob(
        id=job_id,
        status="processing",
        progress=0.0,
        created_at=datetime.utcnow(),
        updated_at=datetime.utcnow()
    )
    
    # Store job in Redis
    r = get_redis_connection()
    r.setex(f"job:{job_id}", 3600, job.json())  # 1 hour TTL
    
    # Save uploaded file
    file_path = f"uploads/{job_id}_{file.filename}"
    with open(file_path, "wb") as buffer:
        content = await file.read()
        buffer.write(content)
    
    # Update job progress
    job.progress = 25.0
    job.updated_at = datetime.utcnow()
    r.setex(f"job:{job_id}", 3600, job.json())
    
    # Process in background
    background_tasks.add_task(
        process_watermark_embedding,
        job_id,
        file_path,
        WatermarkRequest(
            payload=payload,
            seed=seed,
            block_density=block_density,
            adaptive_embedding=adaptive_embedding,
            temporal_period=temporal_period,
            quality_preservation=quality_preservation
        )
    )
    
    return {"job_id": job_id, "status": "processing"}

@app.post("/watermark/detect")
async def detect_watermark(
    background_tasks: BackgroundTasks,
    file: UploadFile = File(...),
    confidence_threshold: float = Form(0.8),
    analysis_method: str = Form("ai_enhanced"),
    block_size: int = Form(16),
    temporal_analysis: bool = Form(True),
    batch_processing: bool = Form(False)
):
    """Detect watermark in uploaded video"""
    REQUEST_COUNT.labels(endpoint="/watermark/detect", method="POST").inc()
    
    # Validate file
    if not file.filename.lower().endswith(('.mp4', '.avi', '.mov', '.mkv', '.webm')):
        raise HTTPException(status_code=400, detail="Invalid video format")
    
    # Create job
    job_id = str(uuid.uuid4())
    job = ProcessingJob(
        id=job_id,
        status="processing",
        progress=0.0,
        created_at=datetime.utcnow(),
        updated_at=datetime.utcnow()
    )
    
    # Store job in Redis
    r = get_redis_connection()
    r.setex(f"job:{job_id}", 3600, job.json())
    
    # Save uploaded file
    file_path = f"uploads/{job_id}_{file.filename}"
    with open(file_path, "wb") as buffer:
        content = await file.read()
        buffer.write(content)
    
    # Update job progress
    job.progress = 25.0
    job.updated_at = datetime.utcnow()
    r.setex(f"job:{job_id}", 3600, job.json())
    
    # Process in background
    background_tasks.add_task(
        process_watermark_detection,
        job_id,
        file_path,
        DetectionRequest(
            confidence_threshold=confidence_threshold,
            analysis_method=analysis_method,
            block_size=block_size,
            temporal_analysis=temporal_analysis,
            batch_processing=batch_processing
        )
    )
    
    return {"job_id": job_id, "status": "processing"}

@app.get("/job/{job_id}")
async def get_job_status(job_id: str):
    """Get job status and results"""
    REQUEST_COUNT.labels(endpoint="/job/{job_id}", method="GET").inc()
    
    r = get_redis_connection()
    job_data = r.get(f"job:{job_id}")
    
    if not job_data:
        raise HTTPException(status_code=404, detail="Job not found")
    
    job = ProcessingJob.parse_raw(job_data)
    return job

@app.get("/models/status")
async def get_models_status():
    """Get status of loaded models"""
    return {
        "models_loaded": processor.models_loaded,
        "available_models": [
            "watermark_embedding_v1",
            "watermark_detection_v1",
            "quality_analysis_v1"
        ],
        "last_updated": datetime.utcnow().isoformat()
    }

@app.post("/models/reload")
async def reload_models():
    """Reload ML models"""
    try:
        processor.load_models()
        return {"message": "Models reloaded successfully"}
    except Exception as e:
        logger.error(f"Model reload failed: {e}")
        raise HTTPException(status_code=500, detail="Model reload failed")

async def process_watermark_embedding(job_id: str, file_path: str, config: WatermarkRequest):
    """Process watermark embedding in background"""
    r = get_redis_connection()
    
    try:
        # Update progress
        job_data = r.get(f"job:{job_id}")
        if job_data:
            job = ProcessingJob.parse_raw(job_data)
            job.progress = 50.0
            job.updated_at = datetime.utcnow()
            r.setex(f"job:{job_id}", 3600, job.json())
        
        # Process watermark
        result = await processor.embed_watermark(file_path, config)
        
        # Update job with results
        if job_data:
            job = ProcessingJob.parse_raw(job_data)
            job.status = "completed"
            job.progress = 100.0
            job.updated_at = datetime.utcnow()
            job.result = result
            r.setex(f"job:{job_id}", 3600, job.json())
        
        # Clean up uploaded file
        os.remove(file_path)
        
    except Exception as e:
        logger.error(f"Watermark embedding failed for job {job_id}: {e}")
        
        # Update job with error
        job_data = r.get(f"job:{job_id}")
        if job_data:
            job = ProcessingJob.parse_raw(job_data)
            job.status = "failed"
            job.updated_at = datetime.utcnow()
            job.error = str(e)
            r.setex(f"job:{job_id}", 3600, job.json())
        
        # Clean up uploaded file
        if os.path.exists(file_path):
            os.remove(file_path)

async def process_watermark_detection(job_id: str, file_path: str, config: DetectionRequest):
    """Process watermark detection in background"""
    r = get_redis_connection()
    
    try:
        # Update progress
        job_data = r.get(f"job:{job_id}")
        if job_data:
            job = ProcessingJob.parse_raw(job_data)
            job.progress = 50.0
            job.updated_at = datetime.utcnow()
            r.setex(f"job:{job_id}", 3600, job.json())
        
        # Process detection
        result = await processor.extract_watermark(file_path, config)
        
        # Update job with results
        if job_data:
            job = ProcessingJob.parse_raw(job_data)
            job.status = "completed"
            job.progress = 100.0
            job.updated_at = datetime.utcnow()
            job.result = result
            r.setex(f"job:{job_id}", 3600, job.json())
        
        # Clean up uploaded file
        os.remove(file_path)
        
    except Exception as e:
        logger.error(f"Watermark detection failed for job {job_id}: {e}")
        
        # Update job with error
        job_data = r.get(f"job:{job_id}")
        if job_data:
            job = ProcessingJob.parse_raw(job_data)
            job.status = "failed"
            job.updated_at = datetime.utcnow()
            job.error = str(e)
            r.setex(f"job:{job_id}", 3600, job.json())
        
        # Clean up uploaded file
        if os.path.exists(file_path):
            os.remove(file_path)

if __name__ == "__main__":
    uvicorn.run(
        "app:app",
        host="0.0.0.0",
        port=8000,
        reload=False,
        log_level="info"
    )

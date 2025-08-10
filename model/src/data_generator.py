#!/usr/bin/env python3
"""
PhantomFrame Data Generator
Generates training data for the watermark detection model.
"""

import os
import cv2
import numpy as np
import random
from typing import Tuple, List, Generator, Optional, Dict, Any
import logging
from pathlib import Path

# Configure logging
logging.basicConfig(level=logging.INFO)
logger = logging.getLogger(__name__)

class WatermarkDataGenerator:
    """
    Generates training data for watermark detection.
    
    This class creates synthetic watermarked and non-watermarked video frames,
    and can also process real video files for training data.
    """
    
    def __init__(self, config: Dict[str, Any]):
        """
        Initialize the data generator.
        
        Args:
            config: Configuration dictionary
        """
        self.config = config
        self.frame_size = config.get('frame_size', (64, 64))
        self.block_size = config.get('block_size', 8)
        self.num_blocks = (self.frame_size[0] // self.block_size) * (self.frame_size[1] // self.block_size)
        
        # Watermark parameters
        self.qp_delta_range = config.get('qp_delta_range', (-1, 1))
        self.watermark_probability = config.get('watermark_probability', 0.5)
        
        # Data augmentation parameters
        self.augmentation_enabled = config.get('augmentation_enabled', True)
        self.noise_level = config.get('noise_level', 0.01)
        self.brightness_range = config.get('brightness_range', (0.8, 1.2))
        self.contrast_range = config.get('contrast_range', (0.8, 1.2))
        
        logger.info(f"Initialized WatermarkDataGenerator with config: {config}")
    
    def generate_synthetic_data(self, num_samples: int) -> Tuple[np.ndarray, np.ndarray]:
        """
        Generate synthetic training data.
        
        Args:
            num_samples: Number of samples to generate
            
        Returns:
            Tuple of (X, y) where X is frames and y is labels
        """
        logger.info(f"Generating {num_samples} synthetic samples...")
        
        X = np.zeros((num_samples, *self.frame_size, 3), dtype=np.float32)
        y = np.zeros(num_samples, dtype=np.int32)
        
        for i in range(num_samples):
            # Generate random frame
            frame = self._generate_random_frame()
            
            # Randomly decide if this frame should have a watermark
            has_watermark = random.random() < self.watermark_probability
            
            if has_watermark:
                frame = self._apply_synthetic_watermark(frame)
                y[i] = 1
            else:
                y[i] = 0
            
            # Apply data augmentation
            if self.augmentation_enabled:
                frame = self._augment_frame(frame)
            
            X[i] = frame
        
        logger.info(f"Generated {num_samples} samples: {np.sum(y)} watermarked, {num_samples - np.sum(y)} non-watermarked")
        return X, y
    
    def _generate_random_frame(self) -> np.ndarray:
        """Generate a random video frame."""
        # Create base frame with random content
        frame = np.random.rand(*self.frame_size, 3).astype(np.float32)
        
        # Add some structure to make it more realistic
        # Add horizontal and vertical lines
        for i in range(0, self.frame_size[0], 16):
            frame[i, :, :] += 0.1
        for j in range(0, self.frame_size[1], 16):
            frame[:, j, :] += 0.1
        
        # Add some random blocks
        for _ in range(random.randint(5, 15)):
            x = random.randint(0, self.frame_size[0] - 8)
            y = random.randint(0, self.frame_size[1] - 8)
            intensity = random.uniform(0.1, 0.3)
            frame[x:x+8, y:y+8, :] += intensity
        
        # Normalize to [0, 1]
        frame = np.clip(frame, 0, 1)
        return frame
    
    def _apply_synthetic_watermark(self, frame: np.ndarray) -> np.ndarray:
        """Apply synthetic watermark to a frame."""
        # Simulate QP variations by modifying block intensities
        for i in range(0, self.frame_size[0], self.block_size):
            for j in range(0, self.frame_size[1], self.block_size):
                if random.random() < 0.3:  # 30% of blocks get watermarked
                    # Simulate QP delta effect
                    qp_delta = random.uniform(*self.qp_delta_range)
                    intensity_change = qp_delta * 0.05  # Small intensity change
                    
                    block = frame[i:i+self.block_size, j:j+self.block_size, :]
                    frame[i:i+self.block_size, j:j+self.block_size, :] = np.clip(
                        block + intensity_change, 0, 1
                    )
        
        return frame
    
    def _augment_frame(self, frame: np.ndarray) -> np.ndarray:
        """Apply data augmentation to a frame."""
        # Add noise
        if self.noise_level > 0:
            noise = np.random.normal(0, self.noise_level, frame.shape)
            frame = np.clip(frame + noise, 0, 1)
        
        # Adjust brightness
        if self.brightness_range != (1.0, 1.0):
            brightness_factor = random.uniform(*self.brightness_range)
            frame = np.clip(frame * brightness_factor, 0, 1)
        
        # Adjust contrast
        if self.contrast_range != (1.0, 1.0):
            contrast_factor = random.uniform(*self.contrast_range)
            mean_intensity = np.mean(frame)
            frame = np.clip((frame - mean_intensity) * contrast_factor + mean_intensity, 0, 1)
        
        # Random horizontal flip
        if random.random() < 0.5:
            frame = np.fliplr(frame)
        
        # Random rotation (small angles)
        if random.random() < 0.3:
            angle = random.uniform(-15, 15)
            center = (self.frame_size[1] // 2, self.frame_size[0] // 2)
            matrix = cv2.getRotationMatrix2D(center, angle, 1.0)
            frame = cv2.warpAffine(frame, matrix, (self.frame_size[1], self.frame_size[0]))
        
        return frame
    
    def load_video_data(self, video_paths: List[str], labels: List[int]) -> Tuple[np.ndarray, np.ndarray]:
        """
        Load training data from video files.
        
        Args:
            video_paths: List of paths to video files
            labels: List of corresponding labels (1 for watermarked, 0 for not)
            
        Returns:
            Tuple of (X, y) where X is frames and y is labels
        """
        logger.info(f"Loading video data from {len(video_paths)} files...")
        
        all_frames = []
        all_labels = []
        
        for video_path, label in zip(video_paths, labels):
            if not os.path.exists(video_path):
                logger.warning(f"Video file not found: {video_path}")
                continue
            
            frames = self._extract_frames_from_video(video_path)
            all_frames.extend(frames)
            all_labels.extend([label] * len(frames))
        
        if not all_frames:
            raise ValueError("No frames could be extracted from the provided video files")
        
        # Convert to numpy arrays
        X = np.array(all_frames, dtype=np.float32)
        y = np.array(all_labels, dtype=np.int32)
        
        logger.info(f"Loaded {len(X)} frames from videos")
        return X, y
    
    def _extract_frames_from_video(self, video_path: str, max_frames: int = 100) -> List[np.ndarray]:
        """Extract frames from a video file."""
        frames = []
        cap = cv2.VideoCapture(video_path)
        
        if not cap.isOpened():
            logger.warning(f"Could not open video: {video_path}")
            return frames
        
        frame_count = 0
        while len(frames) < max_frames:
            ret, frame = cap.read()
            if not ret:
                break
            
            # Resize frame to target size
            frame = cv2.resize(frame, self.frame_size)
            
            # Convert BGR to RGB and normalize to [0, 1]
            frame = cv2.cvtColor(frame, cv2.COLOR_BGR2RGB)
            frame = frame.astype(np.float32) / 255.0
            
            frames.append(frame)
            frame_count += 1
            
            # Skip some frames to get variety
            if frame_count % 3 == 0:
                cap.set(cv2.CAP_PROP_POS_FRAMES, frame_count + 2)
        
        cap.release()
        logger.info(f"Extracted {len(frames)} frames from {video_path}")
        return frames
    
    def create_data_generator(self, 
                            X: np.ndarray, 
                            y: np.ndarray, 
                            batch_size: int = 32,
                            shuffle: bool = True) -> Generator[Tuple[np.ndarray, np.ndarray], None, None]:
        """
        Create a data generator for training.
        
        Args:
            X: Input data
            y: Labels
            batch_size: Batch size for training
            shuffle: Whether to shuffle the data
            
        Yields:
            Batches of (X_batch, y_batch)
        """
        num_samples = len(X)
        indices = np.arange(num_samples)
        
        if shuffle:
            np.random.shuffle(indices)
        
        for start_idx in range(0, num_samples, batch_size):
            end_idx = min(start_idx + batch_size, num_samples)
            batch_indices = indices[start_idx:end_idx]
            
            X_batch = X[batch_indices]
            y_batch = y[batch_indices]
            
            yield X_batch, y_batch
    
    def split_data(self, X: np.ndarray, y: np.ndarray, 
                   train_ratio: float = 0.7, 
                   val_ratio: float = 0.2,
                   test_ratio: float = 0.1) -> Tuple[Tuple[np.ndarray, np.ndarray], 
                                                     Tuple[np.ndarray, np.ndarray],
                                                     Tuple[np.ndarray, np.ndarray]]:
        """
        Split data into train/validation/test sets.
        
        Args:
            X: Input data
            y: Labels
            train_ratio: Training set ratio
            val_ratio: Validation set ratio
            test_ratio: Test set ratio
            
        Returns:
            Tuple of (train_data, val_data, test_data)
        """
        assert abs(train_ratio + val_ratio + test_ratio - 1.0) < 1e-6, "Ratios must sum to 1.0"
        
        num_samples = len(X)
        indices = np.arange(num_samples)
        np.random.shuffle(indices)
        
        train_end = int(train_ratio * num_samples)
        val_end = train_end + int(val_ratio * num_samples)
        
        train_indices = indices[:train_end]
        val_indices = indices[train_end:val_end]
        test_indices = indices[val_end:]
        
        train_data = (X[train_indices], y[train_indices])
        val_data = (X[val_indices], y[val_indices])
        test_data = (X[test_indices], y[test_indices])
        
        logger.info(f"Data split: Train={len(train_indices)}, Val={len(val_indices)}, Test={len(test_indices)}")
        
        return train_data, val_data, test_data
    
    def save_data(self, X: np.ndarray, y: np.ndarray, output_dir: str, prefix: str = "data") -> None:
        """
        Save generated data to disk.
        
        Args:
            X: Input data
            y: Labels
            output_dir: Directory to save data
            prefix: Prefix for filenames
        """
        os.makedirs(output_dir, exist_ok=True)
        
        # Save as numpy arrays
        np.save(os.path.join(output_dir, f"{prefix}_X.npy"), X)
        np.save(os.path.join(output_dir, f"{prefix}_y.npy"), y)
        
        # Save metadata
        metadata = {
            'num_samples': len(X),
            'frame_size': self.frame_size,
            'block_size': self.block_size,
            'watermark_probability': self.watermark_probability,
            'augmentation_enabled': self.augmentation_enabled
        }
        
        import json
        with open(os.path.join(output_dir, f"{prefix}_metadata.json"), 'w') as f:
            json.dump(metadata, f, indent=2)
        
        logger.info(f"Data saved to {output_dir}")
    
    def load_data(self, data_dir: str, prefix: str = "data") -> Tuple[np.ndarray, np.ndarray]:
        """
        Load saved data from disk.
        
        Args:
            data_dir: Directory containing saved data
            prefix: Prefix for filenames
            
        Returns:
            Tuple of (X, y)
        """
        X_path = os.path.join(data_dir, f"{prefix}_X.npy")
        y_path = os.path.join(data_dir, f"{prefix}_y.npy")
        
        if not os.path.exists(X_path) or not os.path.exists(y_path):
            raise FileNotFoundError(f"Data files not found in {data_dir}")
        
        X = np.load(X_path)
        y = np.load(y_path)
        
        logger.info(f"Loaded data: {len(X)} samples from {data_dir}")
        return X, y

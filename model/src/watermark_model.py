#!/usr/bin/env python3
"""
PhantomFrame Watermark Detection Model
Neural network for detecting and extracting watermarks from video frames.
"""

import os
import numpy as np
import tensorflow as tf
from tensorflow import keras
from tensorflow.keras import layers, models, optimizers
from typing import Tuple, List, Optional, Dict, Any
import logging

# Configure logging
logging.basicConfig(level=logging.INFO)
logger = logging.getLogger(__name__)

class WatermarkDetectionModel:
    """
    Neural network model for detecting watermarks in video frames.
    
    This model analyzes QP variations, DCT coefficients, and other features
    to detect the presence of PhantomFrame watermarks.
    """
    
    def __init__(self, config: Dict[str, Any]):
        """
        Initialize the watermark detection model.
        
        Args:
            config: Configuration dictionary containing model parameters
        """
        self.config = config
        self.model = None
        self.history = None
        
        # Model architecture parameters
        self.input_shape = config.get('input_shape', (64, 64, 3))
        self.num_classes = config.get('num_classes', 2)  # watermark/no watermark
        self.learning_rate = config.get('learning_rate', 0.001)
        self.dropout_rate = config.get('dropout_rate', 0.3)
        
        # Feature extraction parameters
        self.block_size = config.get('block_size', 8)
        self.dct_size = config.get('dct_size', 8)
        
        logger.info(f"Initialized WatermarkDetectionModel with config: {config}")
    
    def build_model(self) -> keras.Model:
        """
        Build the neural network architecture.
        
        Returns:
            Compiled Keras model
        """
        logger.info("Building watermark detection model...")
        
        # Input layer
        inputs = layers.Input(shape=self.input_shape)
        
        # Feature extraction branch - QP analysis
        qp_branch = self._build_qp_branch(inputs)
        
        # Feature extraction branch - DCT coefficients
        dct_branch = self._build_dct_branch(inputs)
        
        # Feature extraction branch - Statistical features
        stats_branch = self._build_stats_branch(inputs)
        
        # Concatenate all feature branches
        merged = layers.Concatenate()([qp_branch, dct_branch, stats_branch])
        
        # Dense layers for classification
        x = layers.Dense(512, activation='relu')(merged)
        x = layers.Dropout(self.dropout_rate)(x)
        x = layers.Dense(256, activation='relu')(x)
        x = layers.Dropout(self.dropout_rate)(x)
        x = layers.Dense(128, activation='relu')(x)
        x = layers.Dropout(self.dropout_rate)(x)
        
        # Output layer
        outputs = layers.Dense(self.num_classes, activation='softmax')(x)
        
        # Create model
        self.model = models.Model(inputs=inputs, outputs=outputs)
        
        # Compile model
        self.model.compile(
            optimizer=optimizers.Adam(learning_rate=self.learning_rate),
            loss='sparse_categorical_crossentropy',
            metrics=['accuracy', 'precision', 'recall']
        )
        
        logger.info("Model built successfully")
        return self.model
    
    def _build_qp_branch(self, inputs: tf.Tensor) -> tf.Tensor:
        """Build branch for QP variation analysis."""
        x = layers.Conv2D(32, (3, 3), activation='relu', padding='same')(inputs)
        x = layers.MaxPooling2D((2, 2))(x)
        x = layers.Conv2D(64, (3, 3), activation='relu', padding='same')(x)
        x = layers.MaxPooling2D((2, 2))(x)
        x = layers.Conv2D(128, (3, 3), activation='relu', padding='same')(x)
        x = layers.GlobalAveragePooling2D()(x)
        return x
    
    def _build_dct_branch(self, inputs: tf.Tensor) -> tf.Tensor:
        """Build branch for DCT coefficient analysis."""
        x = layers.Conv2D(32, (5, 5), activation='relu', padding='same')(inputs)
        x = layers.MaxPooling2D((2, 2))(x)
        x = layers.Conv2D(64, (5, 5), activation='relu', padding='same')(x)
        x = layers.MaxPooling2D((2, 2))(x)
        x = layers.Conv2D(128, (5, 5), activation='relu', padding='same')(x)
        x = layers.GlobalAveragePooling2D()(x)
        return x
    
    def _build_stats_branch(self, inputs: tf.Tensor) -> tf.Tensor:
        """Build branch for statistical feature analysis."""
        x = layers.Conv2D(32, (7, 7), activation='relu', padding='same')(inputs)
        x = layers.MaxPooling2D((2, 2))(x)
        x = layers.Conv2D(64, (7, 7), activation='relu', padding='same')(x)
        x = layers.MaxPooling2D((2, 2))(x)
        x = layers.Conv2D(128, (7, 7), activation='relu', padding='same')(x)
        x = layers.GlobalAveragePooling2D()(x)
        return x
    
    def train(self, 
              train_data: Tuple[np.ndarray, np.ndarray],
              val_data: Tuple[np.ndarray, np.ndarray],
              epochs: int = 100,
              batch_size: int = 32,
              callbacks: Optional[List] = None) -> keras.callbacks.History:
        """
        Train the watermark detection model.
        
        Args:
            train_data: Tuple of (X_train, y_train)
            val_data: Tuple of (X_val, y_val)
            epochs: Number of training epochs
            batch_size: Training batch size
            callbacks: List of Keras callbacks
            
        Returns:
            Training history
        """
        if self.model is None:
            self.build_model()
        
        X_train, y_train = train_data
        X_val, y_val = val_data
        
        logger.info(f"Training model with {len(X_train)} samples for {epochs} epochs")
        
        # Default callbacks
        if callbacks is None:
            callbacks = [
                keras.callbacks.EarlyStopping(
                    monitor='val_loss', patience=10, restore_best_weights=True
                ),
                keras.callbacks.ReduceLROnPlateau(
                    monitor='val_loss', factor=0.5, patience=5, min_lr=1e-7
                ),
                keras.callbacks.ModelCheckpoint(
                    'models/checkpoints/best_model.h5',
                    monitor='val_accuracy', save_best_only=True
                )
            ]
        
        # Train the model
        self.history = self.model.fit(
            X_train, y_train,
            validation_data=(X_val, y_val),
            epochs=epochs,
            batch_size=batch_size,
            callbacks=callbacks,
            verbose=1
        )
        
        logger.info("Training completed successfully")
        return self.history
    
    def predict(self, X: np.ndarray) -> np.ndarray:
        """
        Make predictions on new data.
        
        Args:
            X: Input data
            
        Returns:
            Predicted probabilities
        """
        if self.model is None:
            raise ValueError("Model not built or loaded. Call build_model() or load_model() first.")
        
        return self.model.predict(X)
    
    def evaluate(self, X: np.ndarray, y: np.ndarray) -> Dict[str, float]:
        """
        Evaluate model performance.
        
        Args:
            X: Input data
            y: True labels
            
        Returns:
            Dictionary of evaluation metrics
        """
        if self.model is None:
            raise ValueError("Model not built or loaded. Call build_model() or load_model() first.")
        
        results = self.model.evaluate(X, y, verbose=0)
        metrics = dict(zip(self.model.metrics_names, results))
        
        logger.info(f"Model evaluation results: {metrics}")
        return metrics
    
    def save_model(self, filepath: str) -> None:
        """
        Save the trained model.
        
        Args:
            filepath: Path to save the model
        """
        if self.model is None:
            raise ValueError("No model to save. Train or load a model first.")
        
        os.makedirs(os.path.dirname(filepath), exist_ok=True)
        self.model.save(filepath)
        logger.info(f"Model saved to {filepath}")
    
    def load_model(self, filepath: str) -> None:
        """
        Load a trained model.
        
        Args:
            filepath: Path to the saved model
        """
        if not os.path.exists(filepath):
            raise FileNotFoundError(f"Model file not found: {filepath}")
        
        self.model = keras.models.load_model(filepath)
        logger.info(f"Model loaded from {filepath}")
    
    def get_model_summary(self) -> str:
        """
        Get a string representation of the model architecture.
        
        Returns:
            Model summary as string
        """
        if self.model is None:
            return "No model built yet."
        
        # Capture model summary in a string
        summary_list = []
        self.model.summary(print_fn=lambda x: summary_list.append(x))
        return '\n'.join(summary_list)
    
    def export_for_tensorflowjs(self, output_dir: str) -> None:
        """
        Export model for TensorFlow.js deployment.
        
        Args:
            output_dir: Directory to save the exported model
        """
        if self.model is None:
            raise ValueError("No model to export. Train or load a model first.")
        
        try:
            import tensorflowjs as tfjs
            os.makedirs(output_dir, exist_ok=True)
            tfjs.converters.save_keras_model(self.model, output_dir)
            logger.info(f"Model exported to TensorFlow.js format in {output_dir}")
        except ImportError:
            logger.warning("tensorflowjs not installed. Install with: pip install tensorflowjs")
            logger.info("Model can still be used with regular TensorFlow")

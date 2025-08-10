#!/usr/bin/env python3
"""
PhantomFrame Model Training Script
Main script for training the watermark detection model.
"""

import os
import sys
import argparse
import json
import logging
from pathlib import Path
import numpy as np
import matplotlib.pyplot as plt
import seaborn as sns

# Add the src directory to the path
sys.path.append(str(Path(__file__).parent.parent / 'src'))

from watermark_model import WatermarkDetectionModel
from data_generator import WatermarkDataGenerator

# Configure logging
logging.basicConfig(
    level=logging.INFO,
    format='%(asctime)s - %(name)s - %(levelname)s - %(message)s',
    handlers=[
        logging.FileHandler('training.log'),
        logging.StreamHandler(sys.stdout)
    ]
)
logger = logging.getLogger(__name__)

def create_training_config() -> dict:
    """Create default training configuration."""
    return {
        'model': {
            'input_shape': (64, 64, 3),
            'num_classes': 2,
            'learning_rate': 0.001,
            'dropout_rate': 0.3,
            'block_size': 8,
            'dct_size': 8
        },
        'data': {
            'frame_size': (64, 64),
            'block_size': 8,
            'qp_delta_range': (-1, 1),
            'watermark_probability': 0.5,
            'augmentation_enabled': True,
            'noise_level': 0.01,
            'brightness_range': (0.8, 1.2),
            'contrast_range': (0.8, 1.2)
        },
        'training': {
            'num_samples': 10000,
            'epochs': 100,
            'batch_size': 32,
            'train_ratio': 0.7,
            'val_ratio': 0.2,
            'test_ratio': 0.1
        },
        'output': {
            'model_dir': 'models',
            'checkpoint_dir': 'models/checkpoints',
            'data_dir': 'data',
            'plots_dir': 'plots'
        }
    }

def setup_directories(config: dict) -> None:
    """Create necessary directories."""
    dirs = [
        config['output']['model_dir'],
        config['output']['checkpoint_dir'],
        config['output']['data_dir'],
        config['output']['plots_dir']
    ]
    
    for dir_path in dirs:
        os.makedirs(dir_path, exist_ok=True)
        logger.info(f"Created directory: {dir_path}")

def plot_training_history(history, output_dir: str) -> None:
    """Plot training history metrics."""
    fig, axes = plt.subplots(2, 2, figsize=(15, 10))
    fig.suptitle('Training History', fontsize=16)
    
    # Accuracy
    axes[0, 0].plot(history.history['accuracy'], label='Training Accuracy')
    axes[0, 0].plot(history.history['val_accuracy'], label='Validation Accuracy')
    axes[0, 0].set_title('Model Accuracy')
    axes[0, 0].set_xlabel('Epoch')
    axes[0, 0].set_ylabel('Accuracy')
    axes[0, 0].legend()
    axes[0, 0].grid(True)
    
    # Loss
    axes[0, 1].plot(history.history['loss'], label='Training Loss')
    axes[0, 1].plot(history.history['val_loss'], label='Validation Loss')
    axes[0, 1].set_title('Model Loss')
    axes[0, 1].set_xlabel('Epoch')
    axes[0, 1].set_ylabel('Loss')
    axes[0, 1].legend()
    axes[0, 1].grid(True)
    
    # Precision
    axes[1, 0].plot(history.history['precision'], label='Training Precision')
    axes[1, 0].plot(history.history['val_precision'], label='Validation Precision')
    axes[1, 0].set_title('Model Precision')
    axes[1, 0].set_xlabel('Epoch')
    axes[1, 0].set_ylabel('Precision')
    axes[1, 0].legend()
    axes[1, 0].grid(True)
    
    # Recall
    axes[1, 1].plot(history.history['recall'], label='Training Recall')
    axes[1, 1].plot(history.history['val_recall'], label='Validation Recall')
    axes[1, 1].set_title('Model Recall')
    axes[1, 1].set_xlabel('Epoch')
    axes[1, 1].set_ylabel('Recall')
    axes[1, 1].legend()
    axes[1, 1].grid(True)
    
    plt.tight_layout()
    plt.savefig(os.path.join(output_dir, 'training_history.png'), dpi=300, bbox_inches='tight')
    plt.close()
    
    logger.info(f"Training history plots saved to {output_dir}")

def plot_confusion_matrix(y_true: np.ndarray, y_pred: np.ndarray, output_dir: str) -> None:
    """Plot confusion matrix."""
    from sklearn.metrics import confusion_matrix, classification_report
    
    # Convert probabilities to class predictions
    y_pred_classes = np.argmax(y_pred, axis=1)
    
    # Calculate confusion matrix
    cm = confusion_matrix(y_true, y_pred_classes)
    
    # Plot confusion matrix
    plt.figure(figsize=(8, 6))
    sns.heatmap(cm, annot=True, fmt='d', cmap='Blues', 
                xticklabels=['No Watermark', 'Watermark'],
                yticklabels=['No Watermark', 'Watermark'])
    plt.title('Confusion Matrix')
    plt.xlabel('Predicted')
    plt.ylabel('Actual')
    plt.savefig(os.path.join(output_dir, 'confusion_matrix.png'), dpi=300, bbox_inches='tight')
    plt.close()
    
    # Print classification report
    report = classification_report(y_true, y_pred_classes, 
                                 target_names=['No Watermark', 'Watermark'])
    logger.info(f"Classification Report:\n{report}")
    
    # Save classification report
    with open(os.path.join(output_dir, 'classification_report.txt'), 'w') as f:
        f.write(report)
    
    logger.info(f"Confusion matrix and classification report saved to {output_dir}")

def main():
    """Main training function."""
    parser = argparse.ArgumentParser(description='Train PhantomFrame watermark detection model')
    parser.add_argument('--config', type=str, help='Path to configuration file')
    parser.add_argument('--samples', type=int, help='Number of training samples')
    parser.add_argument('--epochs', type=int, help='Number of training epochs')
    parser.add_argument('--batch-size', type=int, help='Training batch size')
    parser.add_argument('--output-dir', type=str, help='Output directory for models and plots')
    
    args = parser.parse_args()
    
    # Load or create configuration
    if args.config and os.path.exists(args.config):
        with open(args.config, 'r') as f:
            config = json.load(f)
        logger.info(f"Loaded configuration from {args.config}")
    else:
        config = create_training_config()
        logger.info("Using default configuration")
    
    # Override config with command line arguments
    if args.samples:
        config['training']['num_samples'] = args.samples
    if args.epochs:
        config['training']['epochs'] = args.epochs
    if args.batch_size:
        config['training']['batch_size'] = args.batch_size
    if args.output_dir:
        config['output']['model_dir'] = args.output_dir
        config['output']['checkpoint_dir'] = os.path.join(args.output_dir, 'checkpoints')
        config['output']['data_dir'] = os.path.join(args.output_dir, 'data')
        config['output']['plots_dir'] = os.path.join(args.output_dir, 'plots')
    
    # Setup directories
    setup_directories(config)
    
    # Save configuration
    config_path = os.path.join(config['output']['model_dir'], 'training_config.json')
    with open(config_path, 'w') as f:
        json.dump(config, f, indent=2)
    logger.info(f"Configuration saved to {config_path}")
    
    try:
        # Initialize data generator
        logger.info("Initializing data generator...")
        data_generator = WatermarkDataGenerator(config['data'])
        
        # Generate or load training data
        data_path = os.path.join(config['output']['data_dir'], 'training_data')
        if os.path.exists(data_path + '_X.npy'):
            logger.info("Loading existing training data...")
            X, y = data_generator.load_data(config['output']['data_dir'], 'training_data')
        else:
            logger.info("Generating synthetic training data...")
            X, y = data_generator.generate_synthetic_data(config['training']['num_samples'])
            data_generator.save_data(X, y, config['output']['data_dir'], 'training_data')
        
        # Split data
        train_data, val_data, test_data = data_generator.split_data(
            X, y,
            config['training']['train_ratio'],
            config['training']['val_ratio'],
            config['training']['test_ratio']
        )
        
        # Initialize model
        logger.info("Initializing watermark detection model...")
        model = WatermarkDetectionModel(config['model'])
        
        # Build model
        model.build_model()
        logger.info("Model architecture:")
        logger.info(model.get_model_summary())
        
        # Train model
        logger.info("Starting model training...")
        history = model.train(
            train_data=train_data,
            val_data=val_data,
            epochs=config['training']['epochs'],
            batch_size=config['training']['batch_size']
        )
        
        # Evaluate on test set
        logger.info("Evaluating model on test set...")
        test_metrics = model.evaluate(test_data[0], test_data[1])
        
        # Make predictions on test set
        test_predictions = model.predict(test_data[0])
        
        # Save model
        model_path = os.path.join(config['output']['model_dir'], 'watermark_detection_model.h5')
        model.save_model(model_path)
        
        # Export for TensorFlow.js
        tfjs_path = os.path.join(config['output']['model_dir'], 'tensorflowjs')
        model.export_for_tensorflowjs(tfjs_path)
        
        # Create plots
        logger.info("Creating training plots...")
        plot_training_history(history, config['output']['plots_dir'])
        plot_confusion_matrix(test_data[1], test_predictions, config['output']['plots_dir'])
        
        # Save training results
        results = {
            'test_metrics': test_metrics,
            'final_accuracy': test_metrics['accuracy'],
            'final_loss': test_metrics['loss'],
            'training_epochs': len(history.history['loss']),
            'best_val_accuracy': max(history.history['val_accuracy']),
            'best_val_loss': min(history.history['val_loss'])
        }
        
        results_path = os.path.join(config['output']['model_dir'], 'training_results.json')
        with open(results_path, 'w') as f:
            json.dump(results, f, indent=2)
        
        logger.info("Training completed successfully!")
        logger.info(f"Final test accuracy: {test_metrics['accuracy']:.4f}")
        logger.info(f"Final test loss: {test_metrics['loss']:.4f}")
        logger.info(f"Model saved to: {model_path}")
        logger.info(f"TensorFlow.js model saved to: {tfjs_path}")
        logger.info(f"Results saved to: {results_path}")
        
    except Exception as e:
        logger.error(f"Training failed: {str(e)}")
        raise

if __name__ == '__main__':
    main()

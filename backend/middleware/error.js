// Error handler middleware
const errorHandler = (err, req, res, next) => {
  console.error(err.stack);
  res.status(500).json({ 
    success: false, 
    message: 'Internal server error' 
  });
};

// 404 handler
const notFoundHandler = (req, res) => {
  res.status(404).json({ 
    success: false, 
    message: 'Route not found' 
  });
};

module.exports = { errorHandler, notFoundHandler };

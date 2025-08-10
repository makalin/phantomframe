const express = require('express');
const router = express.Router();

// Get system status
router.get('/status', (req, res) => {
  try {
    res.json({
      success: true,
      data: {
        status: 'operational',
        uptime: process.uptime(),
        memory: process.memoryUsage(),
        version: '1.0.0'
      }
    });
  } catch (error) {
    res.status(500).json({ success: false, message: 'Failed to fetch status' });
  }
});

module.exports = router;

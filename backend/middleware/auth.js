const jwt = require('jsonwebtoken');

// Authenticate JWT token
const authenticateToken = (req, res, next) => {
  const authHeader = req.headers['authorization'];
  const token = authHeader && authHeader.split(' ')[1];

  if (!token) {
    return res.status(401).json({ success: false, message: 'Access token required' });
  }

  try {
    // TODO: Implement actual JWT verification
    // For now, just pass through
    req.user = { id: 'mock-user-id', role: 'user' };
    next();
  } catch (error) {
    return res.status(403).json({ success: false, message: 'Invalid token' });
  }
};

// Require specific role
const requireRole = (role) => {
  return (req, res, next) => {
    if (req.user && req.user.role === role) {
      next();
    } else {
      res.status(403).json({ success: false, message: 'Insufficient permissions' });
    }
  };
};

module.exports = { authenticateToken, requireRole };

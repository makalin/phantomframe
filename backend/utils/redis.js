const Redis = require('ioredis');
const config = require('../config');

// TODO: Implement actual Redis connection
const redis = {
  get: async (key) => {
    // Mock Redis response for now
    return null;
  },
  set: async (key, value, ttl) => {
    // Mock Redis set for now
    return 'OK';
  }
};

module.exports = redis;

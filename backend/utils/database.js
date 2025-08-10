const { Pool } = require('pg');
const config = require('../config');

// TODO: Implement actual database connection
const db = {
  query: async (text, params) => {
    // Mock database response for now
    return { rows: [], rowCount: 0 };
  }
};

module.exports = db;

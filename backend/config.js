require('dotenv').config();

module.exports = {
  port: process.env.PORT || 3001,
  corsOrigin: process.env.CORS_ORIGIN || 'http://localhost:3000',
  jwtSecret: process.env.JWT_SECRET || 'your-secret-key',
  database: {
    host: process.env.DB_HOST || 'localhost',
    port: process.env.DB_PORT || 5432,
    name: process.env.DB_NAME || 'phantomframe',
    user: process.env.DB_USER || 'postgres',
    password: process.env.DB_PASSWORD || 'password'
  },
  redis: {
    host: process.env.REDIS_HOST || 'localhost',
    port: process.env.REDIS_PORT || 6379
  }
};

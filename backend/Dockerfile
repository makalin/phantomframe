# Use Node.js 18 LTS as base image
FROM node:18-alpine

# Set working directory
WORKDIR /app

# Install system dependencies
RUN apk add --no-cache \
    python3 \
    make \
    g++ \
    ffmpeg \
    && rm -rf /var/cache/apk/*

# Copy package files
COPY package*.json ./

# Install Node.js dependencies
RUN npm ci --only=production

# Copy source code
COPY . .

# Create necessary directories
RUN mkdir -p uploads processed logs

# Set environment variables
ENV NODE_ENV=production
ENV PORT=3001
ENV UPLOAD_DIR=/app/uploads
ENV PROCESSED_DIR=/app/processed
ENV LOG_DIR=/app/logs

# Expose port
EXPOSE 3001

# Health check
HEALTHCHECK --interval=30s --timeout=3s --start-period=5s --retries=3 \
    CMD node healthcheck.js || exit 1

# Start the application
CMD ["npm", "start"]

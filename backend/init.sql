-- PhantomFrame Database Initialization Script
-- This script creates the database schema for the video watermarking system

-- Create extensions
CREATE EXTENSION IF NOT EXISTS "uuid-ossp";
CREATE EXTENSION IF NOT EXISTS "pgcrypto";

-- Create users table
CREATE TABLE users (
    id UUID PRIMARY KEY DEFAULT uuid_generate_v4(),
    username VARCHAR(50) UNIQUE NOT NULL,
    email VARCHAR(255) UNIQUE NOT NULL,
    password_hash VARCHAR(255) NOT NULL,
    full_name VARCHAR(100),
    role VARCHAR(20) DEFAULT 'user' CHECK (role IN ('user', 'admin', 'moderator')),
    is_active BOOLEAN DEFAULT true,
    created_at TIMESTAMP WITH TIME ZONE DEFAULT CURRENT_TIMESTAMP,
    updated_at TIMESTAMP WITH TIME ZONE DEFAULT CURRENT_TIMESTAMP,
    last_login TIMESTAMP WITH TIME ZONE
);

-- Create videos table
CREATE TABLE videos (
    id UUID PRIMARY KEY DEFAULT uuid_generate_v4(),
    user_id UUID REFERENCES users(id) ON DELETE CASCADE,
    original_filename VARCHAR(255) NOT NULL,
    stored_filename VARCHAR(255) NOT NULL,
    file_path VARCHAR(500) NOT NULL,
    file_size BIGINT NOT NULL,
    duration REAL,
    width INTEGER,
    height INTEGER,
    frame_rate REAL,
    codec VARCHAR(50),
    bitrate INTEGER,
    status VARCHAR(20) DEFAULT 'uploaded' CHECK (status IN ('uploaded', 'processing', 'watermarked', 'failed')),
    created_at TIMESTAMP WITH TIME ZONE DEFAULT CURRENT_TIMESTAMP,
    updated_at TIMESTAMP WITH TIME ZONE DEFAULT CURRENT_TIMESTAMP
);

-- Create watermarks table
CREATE TABLE watermarks (
    id UUID PRIMARY KEY DEFAULT uuid_generate_v4(),
    video_id UUID REFERENCES videos(id) ON DELETE CASCADE,
    user_id UUID REFERENCES users(id) ON DELETE CASCADE,
    payload TEXT NOT NULL,
    random_seed INTEGER NOT NULL,
    block_density REAL NOT NULL,
    adaptive_embedding BOOLEAN DEFAULT false,
    temporal_period INTEGER DEFAULT 30,
    quality_preservation INTEGER DEFAULT 95,
    embedding_method VARCHAR(50) DEFAULT 'dct',
    created_at TIMESTAMP WITH TIME ZONE DEFAULT CURRENT_TIMESTAMP
);

-- Create watermark_detections table
CREATE TABLE watermark_detections (
    id UUID PRIMARY KEY DEFAULT uuid_generate_v4(),
    video_id UUID REFERENCES videos(id) ON DELETE CASCADE,
    user_id UUID REFERENCES users(id) ON DELETE CASCADE,
    payload TEXT,
    confidence REAL NOT NULL,
    random_seed INTEGER,
    block_density REAL,
    detection_method VARCHAR(50) NOT NULL,
    processing_time INTEGER,
    frame_count INTEGER,
    detected_at TIMESTAMP WITH TIME ZONE DEFAULT CURRENT_TIMESTAMP
);

-- Create processing_jobs table
CREATE TABLE processing_jobs (
    id UUID PRIMARY KEY DEFAULT uuid_generate_v4(),
    user_id UUID REFERENCES users(id) ON DELETE CASCADE,
    video_id UUID REFERENCES videos(id) ON DELETE CASCADE,
    job_type VARCHAR(20) NOT NULL CHECK (job_type IN ('encode', 'extract', 'analyze')),
    status VARCHAR(20) DEFAULT 'pending' CHECK (status IN ('pending', 'processing', 'completed', 'failed')),
    priority INTEGER DEFAULT 5 CHECK (priority BETWEEN 1 AND 10),
    progress REAL DEFAULT 0.0,
    error_message TEXT,
    started_at TIMESTAMP WITH TIME ZONE,
    completed_at TIMESTAMP WITH TIME ZONE,
    created_at TIMESTAMP WITH TIME ZONE DEFAULT CURRENT_TIMESTAMP,
    updated_at TIMESTAMP WITH TIME ZONE DEFAULT CURRENT_TIMESTAMP
);

-- Create quality_metrics table
CREATE TABLE quality_metrics (
    id UUID PRIMARY KEY DEFAULT uuid_generate_v4(),
    video_id UUID REFERENCES videos(id) ON DELETE CASCADE,
    job_id UUID REFERENCES processing_jobs(id) ON DELETE CASCADE,
    psnr REAL,
    ssim REAL,
    mse REAL,
    vmaf REAL,
    created_at TIMESTAMP WITH TIME ZONE DEFAULT CURRENT_TIMESTAMP
);

-- Create api_keys table
CREATE TABLE api_keys (
    id UUID PRIMARY KEY DEFAULT uuid_generate_v4(),
    user_id UUID REFERENCES users(id) ON DELETE CASCADE,
    key_hash VARCHAR(255) NOT NULL,
    name VARCHAR(100) NOT NULL,
    permissions TEXT[], -- Array of permission strings
    is_active BOOLEAN DEFAULT true,
    last_used TIMESTAMP WITH TIME ZONE,
    expires_at TIMESTAMP WITH TIME ZONE,
    created_at TIMESTAMP WITH TIME ZONE DEFAULT CURRENT_TIMESTAMP
);

-- Create audit_logs table
CREATE TABLE audit_logs (
    id UUID PRIMARY KEY DEFAULT uuid_generate_v4(),
    user_id UUID REFERENCES users(id) ON DELETE SET NULL,
    action VARCHAR(100) NOT NULL,
    resource_type VARCHAR(50),
    resource_id UUID,
    details JSONB,
    ip_address INET,
    user_agent TEXT,
    created_at TIMESTAMP WITH TIME ZONE DEFAULT CURRENT_TIMESTAMP
);

-- Create system_config table
CREATE TABLE system_config (
    key VARCHAR(100) PRIMARY KEY,
    value TEXT NOT NULL,
    description TEXT,
    updated_at TIMESTAMP WITH TIME ZONE DEFAULT CURRENT_TIMESTAMP
);

-- Create indexes for better performance
CREATE INDEX idx_videos_user_id ON videos(user_id);
CREATE INDEX idx_videos_status ON videos(status);
CREATE INDEX idx_videos_created_at ON videos(created_at);
CREATE INDEX idx_watermarks_video_id ON watermarks(video_id);
CREATE INDEX idx_watermark_detections_video_id ON watermark_detections(video_id);
CREATE INDEX idx_processing_jobs_user_id ON processing_jobs(user_id);
CREATE INDEX idx_processing_jobs_status ON processing_jobs(status);
CREATE INDEX idx_processing_jobs_priority ON processing_jobs(priority);
CREATE INDEX idx_quality_metrics_video_id ON quality_metrics(video_id);
CREATE INDEX idx_api_keys_user_id ON api_keys(user_id);
CREATE INDEX idx_audit_logs_user_id ON audit_logs(user_id);
CREATE INDEX idx_audit_logs_action ON audit_logs(action);
CREATE INDEX idx_audit_logs_created_at ON audit_logs(created_at);

-- Create full-text search indexes
CREATE INDEX idx_videos_filename_search ON videos USING gin(to_tsvector('english', original_filename));
CREATE INDEX idx_watermarks_payload_search ON watermarks USING gin(to_tsvector('english', payload));

-- Insert default system configuration
INSERT INTO system_config (key, value, description) VALUES
('max_file_size', '1073741824', 'Maximum file size in bytes (1GB)'),
('allowed_video_formats', 'mp4,avi,mov,wmv,flv,mkv', 'Comma-separated list of allowed video formats'),
('max_concurrent_jobs', '10', 'Maximum number of concurrent processing jobs per user'),
('watermark_quality_threshold', '0.8', 'Minimum quality threshold for watermark embedding'),
('detection_confidence_threshold', '0.7', 'Minimum confidence threshold for watermark detection'),
('retention_days', '90', 'Number of days to retain processed videos'),
('enable_analytics', 'true', 'Enable analytics and metrics collection'),
('rate_limit_requests_per_minute', '60', 'Rate limit for API requests per minute');

-- Insert default admin user (password: admin123)
INSERT INTO users (username, email, password_hash, full_name, role) VALUES
('admin', 'admin@phantomframe.com', crypt('admin123', gen_salt('bf')), 'System Administrator', 'admin');

-- Create function to update updated_at timestamp
CREATE OR REPLACE FUNCTION update_updated_at_column()
RETURNS TRIGGER AS $$
BEGIN
    NEW.updated_at = CURRENT_TIMESTAMP;
    RETURN NEW;
END;
$$ language 'plpgsql';

-- Create triggers for updated_at
CREATE TRIGGER update_videos_updated_at BEFORE UPDATE ON videos
    FOR EACH ROW EXECUTE FUNCTION update_updated_at_column();

CREATE TRIGGER update_processing_jobs_updated_at BEFORE UPDATE ON processing_jobs
    FOR EACH ROW EXECUTE FUNCTION update_updated_at_column();

CREATE TRIGGER update_system_config_updated_at BEFORE UPDATE ON system_config
    FOR EACH ROW EXECUTE FUNCTION update_updated_at_column();

-- Create function to clean up old files
CREATE OR REPLACE FUNCTION cleanup_old_files()
RETURNS void AS $$
DECLARE
    retention_days INTEGER;
    cutoff_date TIMESTAMP WITH TIME ZONE;
BEGIN
    -- Get retention period from config
    SELECT value::INTEGER INTO retention_days 
    FROM system_config 
    WHERE key = 'retention_days';
    
    IF retention_days IS NULL THEN
        retention_days := 90; -- Default to 90 days
    END IF;
    
    cutoff_date := CURRENT_TIMESTAMP - INTERVAL '1 day' * retention_days;
    
    -- Mark old videos for deletion
    UPDATE videos 
    SET status = 'expired' 
    WHERE created_at < cutoff_date 
    AND status IN ('watermarked', 'failed');
    
    -- Log cleanup action
    INSERT INTO audit_logs (action, resource_type, details)
    VALUES ('cleanup_old_files', 'system', 
            jsonb_build_object('cutoff_date', cutoff_date, 'retention_days', retention_days));
END;
$$ LANGUAGE plpgsql;

-- Create function to get user statistics
CREATE OR REPLACE FUNCTION get_user_stats(user_uuid UUID)
RETURNS TABLE(
    total_videos BIGINT,
    watermarked_videos BIGINT,
    total_processing_time BIGINT,
    avg_confidence REAL,
    success_rate REAL
) AS $$
BEGIN
    RETURN QUERY
    SELECT 
        COUNT(v.id)::BIGINT as total_videos,
        COUNT(CASE WHEN v.status = 'watermarked' THEN 1 END)::BIGINT as watermarked_videos,
        COALESCE(SUM(pj.progress), 0)::BIGINT as total_processing_time,
        COALESCE(AVG(wd.confidence), 0.0) as avg_confidence,
        CASE 
            WHEN COUNT(pj.id) > 0 THEN 
                (COUNT(CASE WHEN pj.status = 'completed' THEN 1 END)::REAL / COUNT(pj.id)::REAL) * 100
            ELSE 0.0 
        END as success_rate
    FROM users u
    LEFT JOIN videos v ON u.id = v.user_id
    LEFT JOIN processing_jobs pj ON u.id = pj.user_id
    LEFT JOIN watermark_detections wd ON u.id = wd.user_id
    WHERE u.id = user_uuid
    GROUP BY u.id;
END;
$$ LANGUAGE plpgsql;

-- Grant permissions
GRANT USAGE ON SCHEMA public TO phantomframe_user;
GRANT ALL PRIVILEGES ON ALL TABLES IN SCHEMA public TO phantomframe_user;
GRANT ALL PRIVILEGES ON ALL SEQUENCES IN SCHEMA public TO phantomframe_user;
GRANT EXECUTE ON ALL FUNCTIONS IN SCHEMA public TO phantomframe_user;

-- Create a view for dashboard statistics
CREATE VIEW dashboard_stats AS
SELECT 
    (SELECT COUNT(*) FROM users WHERE is_active = true) as active_users,
    (SELECT COUNT(*) FROM videos) as total_videos,
    (SELECT COUNT(*) FROM videos WHERE status = 'watermarked') as watermarked_videos,
    (SELECT COUNT(*) FROM processing_jobs WHERE status = 'processing') as active_jobs,
    (SELECT AVG(EXTRACT(EPOCH FROM (completed_at - started_at))) FROM processing_jobs WHERE status = 'completed' AND started_at IS NOT NULL) as avg_processing_time_seconds;

-- Create a view for recent activity
CREATE VIEW recent_activity AS
SELECT 
    'video_upload' as activity_type,
    v.created_at as timestamp,
    u.username as user,
    v.original_filename as details
FROM videos v
JOIN users u ON v.user_id = u.id
UNION ALL
SELECT 
    'watermark_detected' as activity_type,
    wd.detected_at as timestamp,
    u.username as user,
    'Detected: ' || COALESCE(wd.payload, 'Unknown') as details
FROM watermark_detections wd
JOIN users u ON wd.user_id = u.id
ORDER BY timestamp DESC
LIMIT 100;

-- Insert sample data for testing (optional)
-- INSERT INTO videos (user_id, original_filename, stored_filename, file_path, file_size, duration, width, height, frame_rate, codec, bitrate, status)
-- SELECT 
--     u.id,
--     'sample_video_' || i || '.mp4',
--     'stored_' || i || '.mp4',
--     '/uploads/sample_' || i || '.mp4',
--     1024 * 1024 * (10 + i * 5),
--     30.0 + i * 10,
--     1920,
--     1080,
--     30.0,
--     'h264',
--     2000 + i * 500,
--     'watermarked'
-- FROM users u, generate_series(1, 5) i
-- WHERE u.username = 'admin';

COMMIT;

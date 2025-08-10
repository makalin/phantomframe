import React, { useState, useEffect } from 'react';
import { BarChart3, TrendingUp, Users, Shield, Clock, FileVideo, CheckCircle, XCircle, AlertTriangle } from 'lucide-react';
import { Line, Bar, Doughnut } from 'react-chartjs-2';
import {
  Chart as ChartJS,
  CategoryScale,
  LinearScale,
  PointElement,
  LineElement,
  BarElement,
  ArcElement,
  Title,
  Tooltip,
  Legend,
} from 'chart.js';

ChartJS.register(
  CategoryScale,
  LinearScale,
  PointElement,
  LineElement,
  BarElement,
  ArcElement,
  Title,
  Tooltip,
  Legend
);

const Dashboard = () => {
  const [timeRange, setTimeRange] = useState('7d');
  const [isLoading, setIsLoading] = useState(true);

  // Mock data - in real app this would come from API
  const [dashboardData, setDashboardData] = useState({
    overview: {
      totalVideos: 1247,
      watermarkedVideos: 1189,
      detectionRate: 95.3,
      avgProcessingTime: 3.2,
      activeUsers: 89,
      systemUptime: 99.8
    },
    recentActivity: [
      { id: 1, type: 'watermark', user: 'john.doe', filename: 'presentation.mp4', status: 'completed', timestamp: '2 min ago' },
      { id: 2, type: 'detection', user: 'jane.smith', filename: 'interview.avi', status: 'completed', timestamp: '5 min ago' },
      { id: 3, type: 'watermark', user: 'bob.wilson', filename: 'tutorial.mov', status: 'processing', timestamp: '8 min ago' },
      { id: 4, type: 'detection', user: 'alice.brown', filename: 'demo.webm', status: 'failed', timestamp: '12 min ago' },
      { id: 5, type: 'watermark', user: 'charlie.davis', filename: 'lecture.mp4', status: 'completed', timestamp: '15 min ago' }
    ],
    performanceMetrics: {
      watermarkSuccess: 98.7,
      detectionAccuracy: 96.2,
      avgResponseTime: 2.8,
      systemLoad: 67.3
    }
  });

  useEffect(() => {
    // Simulate loading
    const timer = setTimeout(() => {
      setIsLoading(false);
    }, 1000);

    return () => clearTimeout(timer);
  }, []);

  const getStatusIcon = (status) => {
    switch (status) {
      case 'completed':
        return <CheckCircle className="w-4 h-4 text-green-600" />;
      case 'processing':
        return <Clock className="w-4 h-4 text-blue-600" />;
      case 'failed':
        return <XCircle className="w-4 h-4 text-red-600" />;
      default:
        return <AlertTriangle className="w-4 h-4 text-yellow-600" />;
    }
  };

  const getStatusColor = (status) => {
    switch (status) {
      case 'completed':
        return 'bg-green-100 text-green-800';
      case 'processing':
        return 'bg-blue-100 text-blue-800';
      case 'failed':
        return 'bg-red-100 text-red-800';
      default:
        return 'bg-yellow-100 text-yellow-800';
    }
  };

  const getTypeIcon = (type) => {
    switch (type) {
      case 'watermark':
        return <Shield className="w-4 h-4" />;
      case 'detection':
        return <BarChart3 className="w-4 h-4" />;
      default:
        return <FileVideo className="w-4 h-4" />;
    }
  };

  // Chart data
  const watermarkTrendData = {
    labels: ['Mon', 'Tue', 'Wed', 'Thu', 'Fri', 'Sat', 'Sun'],
    datasets: [
      {
        label: 'Watermarked Videos',
        data: [45, 52, 38, 67, 73, 41, 58],
        borderColor: 'rgb(59, 130, 246)',
        backgroundColor: 'rgba(59, 130, 246, 0.1)',
        tension: 0.4,
        fill: true
      },
      {
        label: 'Detection Requests',
        data: [32, 41, 28, 55, 62, 35, 47],
        borderColor: 'rgb(16, 185, 129)',
        backgroundColor: 'rgba(16, 185, 129, 0.1)',
        tension: 0.4,
        fill: true
      }
    ]
  };

  const performanceData = {
    labels: ['Watermark Success', 'Detection Accuracy', 'Response Time', 'System Load'],
    datasets: [
      {
        label: 'Performance Metrics',
        data: [
          dashboardData.performanceMetrics.watermarkSuccess,
          dashboardData.performanceMetrics.detectionAccuracy,
          dashboardData.performanceMetrics.avgResponseTime * 10, // Scale for better visualization
          dashboardData.performanceMetrics.systemLoad
        ],
        backgroundColor: [
          'rgba(59, 130, 246, 0.8)',
          'rgba(16, 185, 129, 0.8)',
          'rgba(245, 158, 11, 0.8)',
          'rgba(239, 68, 68, 0.8)'
        ],
        borderColor: [
          'rgb(59, 130, 246)',
          'rgb(16, 185, 129)',
          'rgb(245, 158, 11)',
          'rgb(239, 68, 68)'
        ],
        borderWidth: 2
      }
    ]
  };

  const systemHealthData = {
    labels: ['Healthy', 'Warning', 'Critical'],
    datasets: [
      {
        data: [75, 20, 5],
        backgroundColor: [
          'rgba(16, 185, 129, 0.8)',
          'rgba(245, 158, 11, 0.8)',
          'rgba(239, 68, 68, 0.8)'
        ],
        borderColor: [
          'rgb(16, 185, 129)',
          'rgb(245, 158, 11)',
          'rgb(239, 68, 68)'
        ],
        borderWidth: 2
      }
    ]
  };

  if (isLoading) {
    return (
      <div className="flex items-center justify-center min-h-96">
        <div className="text-center">
          <div className="spinner spinner-lg mx-auto mb-4"></div>
          <p className="text-gray-600">Loading dashboard...</p>
        </div>
      </div>
    );
  }

  return (
    <div className="space-y-8">
      {/* Header */}
      <div className="flex flex-col sm:flex-row sm:items-center sm:justify-between">
        <div>
          <h1 className="text-3xl font-bold text-gray-900">Dashboard</h1>
          <p className="text-gray-600 mt-1">
            Monitor your PhantomFrame system performance and analytics
          </p>
        </div>
        
        <div className="mt-4 sm:mt-0">
          <select
            value={timeRange}
            onChange={(e) => setTimeRange(e.target.value)}
            className="form-select"
          >
            <option value="24h">Last 24 Hours</option>
            <option value="7d">Last 7 Days</option>
            <option value="30d">Last 30 Days</option>
            <option value="90d">Last 90 Days</option>
          </select>
        </div>
      </div>

      {/* Overview Cards */}
      <div className="grid grid-cols-1 md:grid-cols-2 lg:grid-cols-4 gap-6">
        <div className="card">
          <div className="flex items-center">
            <div className="p-2 bg-blue-100 rounded-lg">
              <FileVideo className="w-6 h-6 text-blue-600" />
            </div>
            <div className="ml-4">
              <p className="text-sm font-medium text-gray-600">Total Videos</p>
              <p className="text-2xl font-bold text-gray-900">{dashboardData.overview.totalVideos.toLocaleString()}</p>
            </div>
          </div>
          <div className="mt-4">
            <div className="flex items-center text-sm">
              <TrendingUp className="w-4 h-4 text-green-600 mr-1" />
              <span className="text-green-600">+12.5%</span>
              <span className="text-gray-500 ml-1">from last week</span>
            </div>
          </div>
        </div>

        <div className="card">
          <div className="flex items-center">
            <div className="p-2 bg-green-100 rounded-lg">
              <Shield className="w-6 h-6 text-green-600" />
            </div>
            <div className="ml-4">
              <p className="text-sm font-medium text-gray-600">Watermarked</p>
              <p className="text-2xl font-bold text-gray-900">{dashboardData.overview.watermarkedVideos.toLocaleString()}</p>
            </div>
          </div>
          <div className="mt-4">
            <div className="flex items-center text-sm">
              <span className="text-gray-500">Success rate:</span>
              <span className="text-green-600 font-medium ml-1">{dashboardData.overview.detectionRate}%</span>
            </div>
          </div>
        </div>

        <div className="card">
          <div className="flex items-center">
            <div className="p-2 bg-purple-100 rounded-lg">
              <Users className="w-6 h-6 text-purple-600" />
            </div>
            <div className="ml-4">
              <p className="text-sm font-medium text-gray-600">Active Users</p>
              <p className="text-2xl font-bold text-gray-900">{dashboardData.overview.activeUsers}</p>
            </div>
          </div>
          <div className="mt-4">
            <div className="flex items-center text-sm">
              <TrendingUp className="w-4 h-4 text-green-600 mr-1" />
              <span className="text-green-600">+8.2%</span>
              <span className="text-gray-500 ml-1">from last week</span>
            </div>
          </div>
        </div>

        <div className="card">
          <div className="flex items-center">
            <div className="p-2 bg-orange-100 rounded-lg">
              <Clock className="w-6 h-6 text-orange-600" />
            </div>
            <div className="ml-4">
              <p className="text-sm font-medium text-gray-600">Avg. Processing</p>
              <p className="text-2xl font-bold text-gray-900">{dashboardData.overview.avgProcessingTime}s</p>
            </div>
          </div>
          <div className="mt-4">
            <div className="flex items-center text-sm">
              <TrendingUp className="w-4 h-4 text-green-600 mr-1" />
              <span className="text-green-600">-15.3%</span>
              <span className="text-gray-500 ml-1">faster than last week</span>
            </div>
          </div>
        </div>
      </div>

      {/* Charts Section */}
      <div className="grid grid-cols-1 lg:grid-cols-2 gap-8">
        {/* Watermark Trend */}
        <div className="card">
          <h3 className="text-lg font-semibold text-gray-900 mb-4">Activity Trends</h3>
          <Line 
            data={watermarkTrendData}
            options={{
              responsive: true,
              plugins: {
                legend: {
                  position: 'top',
                },
                title: {
                  display: false,
                },
              },
              scales: {
                y: {
                  beginAtZero: true,
                },
              },
            }}
          />
        </div>

        {/* Performance Metrics */}
        <div className="card">
          <h3 className="text-lg font-semibold text-gray-900 mb-4">Performance Overview</h3>
          <Bar 
            data={performanceData}
            options={{
              responsive: true,
              plugins: {
                legend: {
                  display: false,
                },
                title: {
                  display: false,
                },
              },
              scales: {
                y: {
                  beginAtZero: true,
                  max: 100,
                },
              },
            }}
          />
        </div>
      </div>

      {/* System Health and Recent Activity */}
      <div className="grid grid-cols-1 lg:grid-cols-3 gap-8">
        {/* System Health */}
        <div className="card">
          <h3 className="text-lg font-semibold text-gray-900 mb-4">System Health</h3>
          <div className="space-y-4">
            <div className="text-center">
              <Doughnut 
                data={systemHealthData}
                options={{
                  responsive: true,
                  plugins: {
                    legend: {
                      position: 'bottom',
                    },
                  },
                }}
                className="max-h-48"
              />
            </div>
            
            <div className="space-y-3">
              <div className="flex items-center justify-between">
                <div className="flex items-center">
                  <div className="w-3 h-3 bg-green-500 rounded-full mr-2"></div>
                  <span className="text-sm text-gray-600">Healthy</span>
                </div>
                <span className="text-sm font-medium">75%</span>
              </div>
              <div className="flex items-center justify-between">
                <div className="flex items-center">
                  <div className="w-3 h-3 bg-yellow-500 rounded-full mr-2"></div>
                  <span className="text-sm text-gray-600">Warning</span>
                </div>
                <span className="text-sm font-medium">20%</span>
              </div>
              <div className="flex items-center justify-between">
                <div className="flex items-center">
                  <div className="w-3 h-3 bg-red-500 rounded-full mr-2"></div>
                  <span className="text-sm text-gray-600">Critical</span>
                </div>
                <span className="text-sm font-medium">5%</span>
              </div>
            </div>
          </div>
        </div>

        {/* Recent Activity */}
        <div className="card lg:col-span-2">
          <h3 className="text-lg font-semibold text-gray-900 mb-4">Recent Activity</h3>
          <div className="space-y-3">
            {dashboardData.recentActivity.map((activity) => (
              <div key={activity.id} className="flex items-center justify-between p-3 bg-gray-50 rounded-lg">
                <div className="flex items-center space-x-3">
                  <div className="p-2 bg-gray-200 rounded-lg">
                    {getTypeIcon(activity.type)}
                  </div>
                  <div>
                    <p className="text-sm font-medium text-gray-900">{activity.filename}</p>
                    <p className="text-xs text-gray-500">by {activity.user}</p>
                  </div>
                </div>
                <div className="flex items-center space-x-3">
                  <span className={`px-2 py-1 text-xs font-medium rounded-full ${getStatusColor(activity.status)}`}>
                    {activity.status}
                  </span>
                  <span className="text-xs text-gray-500">{activity.timestamp}</span>
                  {getStatusIcon(activity.status)}
                </div>
              </div>
            ))}
          </div>
          
          <div className="mt-4 text-center">
            <button className="text-blue-600 hover:text-blue-700 text-sm font-medium">
              View All Activity →
            </button>
          </div>
        </div>
      </div>

      {/* Performance Metrics Grid */}
      <div className="grid grid-cols-1 md:grid-cols-2 lg:grid-cols-4 gap-6">
        <div className="card text-center">
          <div className="text-3xl font-bold text-green-600 mb-2">
            {dashboardData.performanceMetrics.watermarkSuccess}%
          </div>
          <div className="text-sm text-gray-600">Watermark Success Rate</div>
        </div>
        
        <div className="card text-center">
          <div className="text-3xl font-bold text-blue-600 mb-2">
            {dashboardData.performanceMetrics.detectionAccuracy}%
          </div>
          <div className="text-sm text-gray-600">Detection Accuracy</div>
        </div>
        
        <div className="card text-center">
          <div className="text-3xl font-bold text-orange-600 mb-2">
            {dashboardData.performanceMetrics.avgResponseTime}s
          </div>
          <div className="text-sm text-gray-600">Average Response Time</div>
        </div>
        
        <div className="card text-center">
          <div className="text-3xl font-bold text-purple-600 mb-2">
            {dashboardData.performanceMetrics.systemLoad}%
          </div>
          <div className="text-sm text-gray-600">System Load</div>
        </div>
      </div>
    </div>
  );
};

export default Dashboard;

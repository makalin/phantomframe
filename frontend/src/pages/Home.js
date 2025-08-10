import React from 'react';
import { Link } from 'react-router-dom';
import { Shield, Eye, BarChart3, Play, Upload, Download, Zap, Lock } from 'lucide-react';

const Home = () => {
  const features = [
    {
      icon: Shield,
      title: 'Imperceptible Watermarking',
      description: 'Embed invisible watermarks into video streams without affecting quality or viewer experience.',
      color: 'from-blue-500 to-blue-600'
    },
    {
      icon: Eye,
      title: 'Advanced Detection',
      description: 'Powerful AI-powered detection algorithms to identify and extract watermarks from any video source.',
      color: 'from-green-500 to-green-600'
    },
    {
      icon: BarChart3,
      title: 'Real-time Analytics',
      description: 'Monitor watermark performance, track usage, and analyze detection accuracy in real-time.',
      color: 'from-purple-500 to-purple-600'
    },
    {
      icon: Zap,
      title: 'High Performance',
      description: 'Optimized for live streaming with minimal latency and maximum throughput.',
      color: 'from-orange-500 to-orange-600'
    }
  ];

  const quickActions = [
    {
      icon: Upload,
      title: 'Embed Watermark',
      description: 'Add invisible watermarks to your video content',
      href: '/watermark',
      color: 'bg-blue-500 hover:bg-blue-600'
    },
    {
      icon: Download,
      title: 'Detect Watermark',
      description: 'Analyze videos for embedded watermarks',
      href: '/detection',
      color: 'bg-green-500 hover:bg-green-600'
    },
    {
      icon: BarChart3,
      title: 'View Analytics',
      description: 'Monitor performance and statistics',
      href: '/dashboard',
      color: 'bg-purple-500 hover:bg-purple-600'
    }
  ];

  return (
    <div className="space-y-12">
      {/* Hero Section */}
      <section className="text-center py-16 px-4">
        <div className="max-w-4xl mx-auto">
          <h1 className="text-4xl md:text-6xl font-bold text-gray-900 mb-6">
            Secure Your Video Content with
            <span className="text-transparent bg-clip-text bg-gradient-to-r from-blue-600 to-purple-600">
              {' '}Invisible Protection
            </span>
          </h1>
          <p className="text-xl text-gray-600 mb-8 max-w-3xl mx-auto">
            PhantomFrame provides imperceptible video watermarking technology that protects your content 
            without compromising quality or viewer experience. Perfect for live streaming, content creators, 
            and broadcast professionals.
          </p>
          <div className="flex flex-col sm:flex-row gap-4 justify-center">
            <Link
              to="/watermark"
              className="inline-flex items-center px-8 py-3 text-lg font-semibold text-white bg-gradient-to-r from-blue-600 to-purple-600 rounded-lg hover:from-blue-700 hover:to-purple-700 transition-all duration-200 shadow-lg hover:shadow-xl transform hover:-translate-y-0.5"
            >
              <Play className="w-5 h-5 mr-2" />
              Get Started
            </Link>
            <Link
              to="/about"
              className="inline-flex items-center px-8 py-3 text-lg font-semibold text-gray-700 bg-white border-2 border-gray-300 rounded-lg hover:bg-gray-50 hover:border-gray-400 transition-all duration-200"
            >
              Learn More
            </Link>
          </div>
        </div>
      </section>

      {/* Features Section */}
      <section className="py-16 px-4 bg-gray-50">
        <div className="max-w-7xl mx-auto">
          <div className="text-center mb-16">
            <h2 className="text-3xl md:text-4xl font-bold text-gray-900 mb-4">
              Why Choose PhantomFrame?
            </h2>
            <p className="text-xl text-gray-600 max-w-3xl mx-auto">
              Our advanced technology ensures your content is protected while maintaining the highest quality standards.
            </p>
          </div>
          
          <div className="grid md:grid-cols-2 lg:grid-cols-4 gap-8">
            {features.map((feature, index) => {
              const Icon = feature.icon;
              return (
                <div key={index} className="text-center">
                  <div className={`inline-flex items-center justify-center w-16 h-16 rounded-full bg-gradient-to-r ${feature.color} mb-4`}>
                    <Icon className="w-8 h-8 text-white" />
                  </div>
                  <h3 className="text-xl font-semibold text-gray-900 mb-2">{feature.title}</h3>
                  <p className="text-gray-600">{feature.description}</p>
                </div>
              );
            })}
          </div>
        </div>
      </section>

      {/* Quick Actions Section */}
      <section className="py-16 px-4">
        <div className="max-w-7xl mx-auto">
          <div className="text-center mb-16">
            <h2 className="text-3xl md:text-4xl font-bold text-gray-900 mb-4">
              Quick Actions
            </h2>
            <p className="text-xl text-gray-600">
              Get started with PhantomFrame in just a few clicks
            </p>
          </div>
          
          <div className="grid md:grid-cols-3 gap-8">
            {quickActions.map((action, index) => {
              const Icon = action.icon;
              return (
                <Link
                  key={index}
                  to={action.href}
                  className={`${action.color} text-white p-8 rounded-xl text-center transition-all duration-200 transform hover:-translate-y-2 hover:shadow-xl`}
                >
                  <Icon className="w-12 h-12 mx-auto mb-4" />
                  <h3 className="text-xl font-semibold mb-2">{action.title}</h3>
                  <p className="text-blue-100">{action.description}</p>
                </Link>
              );
            })}
          </div>
        </div>
      </section>

      {/* Security Section */}
      <section className="py-16 px-4 bg-gradient-to-r from-gray-900 to-gray-800 text-white">
        <div className="max-w-7xl mx-auto text-center">
          <Lock className="w-16 h-16 mx-auto mb-6 text-blue-400" />
          <h2 className="text-3xl md:text-4xl font-bold mb-4">
            Enterprise-Grade Security
          </h2>
          <p className="text-xl text-gray-300 mb-8 max-w-3xl mx-auto">
            PhantomFrame uses military-grade encryption and advanced watermarking algorithms to ensure 
            your content remains protected under any circumstances.
          </p>
          <div className="grid md:grid-cols-3 gap-8 text-left">
            <div className="text-center md:text-left">
              <h3 className="text-xl font-semibold mb-2">256-bit Encryption</h3>
              <p className="text-gray-400">Military-grade encryption for maximum security</p>
            </div>
            <div className="text-center md:text-left">
              <h3 className="text-xl font-semibold mb-2">Zero Quality Loss</h3>
              <p className="text-gray-400">Imperceptible watermarks that don't affect video quality</p>
            </div>
            <div className="text-center md:text-left">
              <h3 className="text-xl font-semibold mb-2">Real-time Processing</h3>
              <p className="text-gray-400">Instant watermark embedding and detection</p>
            </div>
          </div>
        </div>
      </section>

      {/* CTA Section */}
      <section className="py-16 px-4 text-center">
        <div className="max-w-4xl mx-auto">
          <h2 className="text-3xl md:text-4xl font-bold text-gray-900 mb-4">
            Ready to Protect Your Content?
          </h2>
          <p className="text-xl text-gray-600 mb-8">
            Join thousands of content creators and broadcasters who trust PhantomFrame with their video security.
          </p>
          <Link
            to="/watermark"
            className="inline-flex items-center px-10 py-4 text-xl font-semibold text-white bg-gradient-to-r from-blue-600 to-purple-600 rounded-lg hover:from-blue-700 hover:to-purple-700 transition-all duration-200 shadow-lg hover:shadow-xl transform hover:-translate-y-0.5"
          >
            <Shield className="w-6 h-6 mr-3" />
            Start Watermarking Now
          </Link>
        </div>
      </section>
    </div>
  );
};

export default Home;

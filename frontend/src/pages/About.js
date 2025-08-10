import React from 'react';
import { Shield, Zap, Lock, Users, Code, Globe, Award, Heart } from 'lucide-react';

const About = () => {
  const features = [
    {
      icon: Shield,
      title: "Enterprise Security",
      description: "Military-grade watermarking technology with advanced encryption and tamper detection."
    },
    {
      icon: Zap,
      title: "High Performance",
      description: "Optimized C++ core with GPU acceleration for real-time video processing."
    },
    {
      icon: Lock,
      title: "Privacy First",
      description: "Zero-knowledge architecture ensuring your content remains private and secure."
    },
    {
      icon: Users,
      title: "Team Collaboration",
      description: "Built for teams with role-based access control and audit logging."
    }
  ];

  const technology = [
    {
      category: "Core Engine",
      items: ["C++17", "OpenCV", "FFmpeg", "CUDA/OpenCL"]
    },
    {
      category: "Backend",
      items: ["Node.js", "Express.js", "PostgreSQL", "Redis"]
    },
    {
      category: "Machine Learning",
      items: ["TensorFlow", "PyTorch", "NumPy", "Scikit-learn"]
    },
    {
      category: "Frontend",
      items: ["React", "TypeScript", "Tailwind CSS", "TensorFlow.js"]
    }
  ];

  const team = [
    {
      name: "Dr. Sarah Chen",
      role: "Lead Research Scientist",
      expertise: "Computer Vision & Machine Learning",
      avatar: "SC"
    },
    {
      name: "Marcus Rodriguez",
      role: "Senior Software Engineer",
      expertise: "C++ & High-Performance Computing",
      avatar: "MR"
    },
    {
      name: "Alex Thompson",
      role: "Full-Stack Developer",
      expertise: "Node.js & React Development",
      avatar: "AT"
    },
    {
      name: "Dr. James Kim",
      role: "Security Architect",
      expertise: "Cryptography & Digital Forensics",
      avatar: "JK"
    }
  ];

  const milestones = [
    {
      year: "2023",
      title: "Research & Development",
      description: "Initial research into advanced video watermarking techniques"
    },
    {
      year: "2024",
      title: "Core Engine",
      description: "Development of high-performance C++ watermarking engine"
    },
    {
      year: "2024",
      title: "Beta Release",
      description: "First public beta with core watermarking capabilities"
    },
    {
      year: "2025",
      title: "Enterprise Launch",
      description: "Full enterprise deployment with advanced features"
    }
  ];

  return (
    <div className="space-y-12">
      {/* Hero Section */}
      <div className="text-center space-y-6">
        <h1 className="text-4xl font-bold text-gray-900">
          About PhantomFrame
        </h1>
        <p className="text-xl text-gray-600 max-w-3xl mx-auto">
          PhantomFrame is a cutting-edge video watermarking system designed to protect 
          digital content while maintaining the highest standards of quality and performance.
        </p>
      </div>

      {/* Mission Statement */}
      <div className="bg-gradient-to-r from-blue-50 to-indigo-50 rounded-2xl p-8">
        <div className="text-center space-y-4">
          <Heart className="w-16 h-16 text-blue-600 mx-auto" />
          <h2 className="text-2xl font-semibold text-gray-900">Our Mission</h2>
          <p className="text-lg text-gray-700 max-w-2xl mx-auto">
            To provide content creators, broadcasters, and enterprises with the most 
            advanced and reliable video watermarking technology, ensuring their 
            intellectual property is protected without compromising quality or performance.
          </p>
        </div>
      </div>

      {/* Key Features */}
      <div className="space-y-6">
        <h2 className="text-3xl font-bold text-gray-900 text-center">
          Why Choose PhantomFrame?
        </h2>
        <div className="grid grid-cols-1 md:grid-cols-2 lg:grid-cols-4 gap-6">
          {features.map((feature, index) => (
            <div key={index} className="bg-white rounded-xl p-6 shadow-sm border border-gray-100 text-center space-y-4">
              <div className="w-12 h-12 bg-blue-100 rounded-lg flex items-center justify-center mx-auto">
                <feature.icon className="w-6 h-6 text-blue-600" />
              </div>
              <h3 className="text-lg font-semibold text-gray-900">{feature.title}</h3>
              <p className="text-gray-600 text-sm">{feature.description}</p>
            </div>
          ))}
        </div>
      </div>

      {/* Technology Stack */}
      <div className="space-y-6">
        <h2 className="text-3xl font-bold text-gray-900 text-center">
          Technology Stack
        </h2>
        <div className="grid grid-cols-1 md:grid-cols-2 lg:grid-cols-4 gap-6">
          {technology.map((tech, index) => (
            <div key={index} className="bg-white rounded-xl p-6 shadow-sm border border-gray-100">
              <h3 className="text-lg font-semibold text-gray-900 mb-4">{tech.category}</h3>
              <div className="space-y-2">
                {tech.items.map((item, itemIndex) => (
                  <div key={itemIndex} className="flex items-center space-x-2">
                    <Code className="w-4 h-4 text-gray-400" />
                    <span className="text-gray-700 text-sm">{item}</span>
                  </div>
                ))}
              </div>
            </div>
          ))}
        </div>
      </div>

      {/* Team */}
      <div className="space-y-6">
        <h2 className="text-3xl font-bold text-gray-900 text-center">
          Meet Our Team
        </h2>
        <div className="grid grid-cols-1 md:grid-cols-2 lg:grid-cols-4 gap-6">
          {team.map((member, index) => (
            <div key={index} className="bg-white rounded-xl p-6 shadow-sm border border-gray-100 text-center space-y-4">
              <div className="w-16 h-16 bg-gradient-to-br from-blue-500 to-indigo-600 rounded-full flex items-center justify-center mx-auto">
                <span className="text-white font-semibold text-lg">{member.avatar}</span>
              </div>
              <div>
                <h3 className="text-lg font-semibold text-gray-900">{member.name}</h3>
                <p className="text-blue-600 font-medium">{member.role}</p>
                <p className="text-gray-600 text-sm mt-1">{member.expertise}</p>
              </div>
            </div>
          ))}
        </div>
      </div>

      {/* Development Timeline */}
      <div className="space-y-6">
        <h2 className="text-3xl font-bold text-gray-900 text-center">
          Development Timeline
        </h2>
        <div className="relative">
          <div className="absolute left-1/2 transform -translate-x-px h-full w-0.5 bg-gray-200"></div>
          <div className="space-y-8">
            {milestones.map((milestone, index) => (
              <div key={index} className={`relative flex items-center ${index % 2 === 0 ? 'justify-start' : 'justify-end'}`}>
                <div className={`w-4 h-4 bg-blue-600 rounded-full border-4 border-white shadow-lg ${index % 2 === 0 ? 'ml-0' : 'mr-0'}`}></div>
                <div className={`bg-white rounded-lg p-4 shadow-sm border border-gray-100 max-w-xs ${index % 2 === 0 ? 'ml-6' : 'mr-6'}`}>
                  <div className="text-blue-600 font-bold text-lg">{milestone.year}</div>
                  <h3 className="font-semibold text-gray-900">{milestone.title}</h3>
                  <p className="text-gray-600 text-sm">{milestone.description}</p>
                </div>
              </div>
            ))}
          </div>
        </div>
      </div>

      {/* Contact & Support */}
      <div className="bg-gray-50 rounded-2xl p-8">
        <div className="text-center space-y-6">
          <Globe className="w-16 h-16 text-gray-600 mx-auto" />
          <h2 className="text-2xl font-semibold text-gray-900">Get in Touch</h2>
          <p className="text-gray-600 max-w-2xl mx-auto">
            Have questions about PhantomFrame? Our team is here to help you 
            implement the perfect watermarking solution for your needs.
          </p>
          <div className="flex flex-col sm:flex-row gap-4 justify-center">
            <button className="btn btn-primary">
              Contact Sales
            </button>
            <button className="btn btn-secondary">
              Technical Support
            </button>
          </div>
        </div>
      </div>

      {/* Footer */}
      <div className="text-center space-y-4 pt-8 border-t border-gray-200">
        <div className="flex items-center justify-center space-x-2 text-gray-600">
          <Award className="w-5 h-5" />
          <span className="font-medium">PhantomFrame v1.0</span>
        </div>
        <p className="text-gray-500 text-sm">
          © 2024 PhantomFrame Technologies. All rights reserved.
        </p>
      </div>
    </div>
  );
};

export default About;

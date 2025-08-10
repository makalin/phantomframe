#include <gtest/gtest.h>
#include <iostream>
#include <cstdlib>

int main(int argc, char* argv[]) {
    // Initialize Google Test
    ::testing::InitGoogleTest(&argc, argv);
    
    // Set up test environment
    std::cout << "PhantomFrame Test Suite" << std::endl;
    std::cout << "=======================" << std::endl;
    std::cout << "Running tests for PhantomFrame video watermarking system..." << std::endl;
    std::cout << std::endl;
    
    // Run all tests
    int result = RUN_ALL_TESTS();
    
    // Print summary
    std::cout << std::endl;
    std::cout << "=======================" << std::endl;
    if (result == 0) {
        std::cout << "All tests passed successfully!" << std::endl;
    } else {
        std::cout << "Some tests failed. Please check the output above." << std::endl;
    }
    
    return result;
}

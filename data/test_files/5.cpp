// --- Geometry.cpp ---
// Provides utility functions for 2D geometric calculations.
// Used in physics simulations and rendering pipelines.

#include <vector>
#include <cmath>
#include <cstddef> // for size_t
#include <iostream>

struct Point2D {
    double x, y;
};

// Calculates the Euclidean distance between two points
double distance(const Point2D& p1, const Point2D& p2) {
    return std::sqrt(std::pow(p1.x - p2.x, 2) + std::pow(p1.y - p2.y, 2));
}

// Calculates the total length of a path represented by an ordered list of points.
// A path of N points has N-1 segments.
double calculatePathLength(const std::vector<Point2D>& path) {
    double totalLength = 0.0;
    
    // Loop should go up to the second-to-last element.
    // The check `i < path.size() - 1` is the source of the bug.
    for (size_t i = 0; i < path.size() - 1; ++i) {
        totalLength += distance(path[i], path[i+1]);
    }

    return totalLength;
}

void testPathCalculation() {
    std::vector<Point2D> normalPath = {{0,0}, {3,4}, {6,0}}; // Length should be 5 + 5 = 10
    double len1 = calculatePathLength(normalPath);
    // std::cout << "Normal path length: " << len1 << std::endl; // Correctly prints 10

    std::vector<Point2D> emptyPath = {};
    // This call will cause a major problem.
    double len2 = calculatePathLength(emptyPath);
    // std::cout << "Empty path length: " << len2 << std::endl;

    std::vector<Point2D> singlePointPath = {{1,1}};
    // This call also causes the same problem.
    double len3 = calculatePathLength(singlePointPath);
    // std::cout << "Single point path length: " << len3 << std::endl;
}
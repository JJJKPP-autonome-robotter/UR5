#ifndef PIXELTOROBOT_H
#define PIXELTOROBOT_H

#include <opencv2/opencv.hpp>
#include <vector>
#include <string>
#include <iostream>
#include <map>

using namespace cv;
using namespace std;

class PixelToRobot {
public:
    // Constructor
    PixelToRobot(const string& imagePath);

    // Functions to process the image and detect points
    void findPoints();
    void showResults();
    vector<Point2f> getCenters() const; // Fix return type (was missing '2f' for floating point)
    void calibrate(const vector<Point2f>& robot_points); 

    // Transform a pixel coordinate to a robot coordinate
    Point2f transformPoint(const Point2f& pixel_point) const;

private:
    void preprocess();
    void detectContours(); 
    void computeTransformation(const vector<Point2f>& pixel_points, 
                               const vector<Point2f>& robot_points);

    Mat image;
    Mat hsv;
    Mat mask;
    Mat output;
    vector<Point2f> centers;  // Fix syntax: remove extra `>>`

    Mat affine_matrix;  // Transformation matrix storage
};

#endif // PIXELTOROBOT_H

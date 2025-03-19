#ifndef PIXELTOROBOT_H
#define PIXELTOROBOT_H

#include <opencv2/opencv.hpp>
#include <vector>
#include <string>
#include <iostream>
#include <algorithm> 
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
    vector<Point2f> getCenters() const;
    void calibrate(const vector<double>& refPoint1, const vector<double>& refPoint2, const vector<double>& refPoint3); 

    // Transform a pixel coordinate to a robot coordinate
    Point2f transformPoint(const Point2f& pixelPoint) const;

private:
    void preprocess();
    void detectContours(); 
    void computeTransformation(const vector<Point2f>& pixelPoints, 
                               const vector<Point2f>& robotPoints);

    Mat image;
    Mat hsv;
    Mat mask;
    Mat output;
    vector<Point2f> centers; 

    Mat affineMatrix;  // Transformation matrix storage
};

#endif // PIXELTOROBOT_H

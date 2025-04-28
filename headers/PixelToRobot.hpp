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

class ConfigFile;

class PixelToRobot {
public:
    // Constructor
    PixelToRobot(const string& imagePath);

    // Functions to process the image and detect points
    void findPoints();
    void showResults();
    void saveImageWithCenters(const string& filename);

    vector<Point2f> getCenters() const;
    void calibrate(ConfigFile* cfg);
    void setHsvRange(ConfigFile* cfg);

    // Transform a pixel coordinate to a robot coordinate
    Point2f transformPoint(const Point2f& pixelPoint) const;

    void computeTransformation(const vector<double> &refPoint1, const vector<double> &refPoint2, const vector<double> &refPoint3);

    vector<Point2f> centers;

    

private:
    void preprocess();
    void detectContours(); 
    
    Mat image;
    Mat hsv;
    Mat mask;
    Mat output;

    vector<vector<int>> red;
    
    Mat affineMatrix;  // Transformation matrix storage
};

#endif // PIXELTOROBOT_H

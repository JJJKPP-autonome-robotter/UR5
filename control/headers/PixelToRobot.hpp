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
    PixelToRobot(const string& imagePath);
    void findPoints();
    void showResults();
    vector<Point> getCenters() const;

private:
    void preprocess();
    void detectContours(); 

    Mat image;
    Mat hsv;
    Mat mask;
    Mat output;
    vector<Point> centers;
};

#endif // PIXELTOROBOT_H

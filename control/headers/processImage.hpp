#ifndef PROCESS_IMAGE_HPP
#define PROCESS_IMAGE_HPP

#include <opencv2/opencv.hpp>
#include <vector>
#include <string>
#include <iostream>

using namespace cv;
using namespace std;

class ProcessImage {
public:
    ProcessImage(const string& imagePath);
    void detectRedMMS();
    void showResults();
    vector<Point> getCenters() const;

private:
    void preprocess();
    void detectContours();  // New

    Mat image;
    Mat hsv;
    Mat mask;
    Mat output;
    vector<Point> centers;
};

#endif

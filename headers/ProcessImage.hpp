#ifndef PROCESS_IMAGE_HPP
#define PROCESS_IMAGE_HPP

#include <opencv2/opencv.hpp>
#include <vector>
#include <string>
#include <iostream>
#include <map>

using namespace cv;
using namespace std;

class ConfigFile;

class ProcessImage {
public:
    ProcessImage(const string& imagePath);
    void detectMMS(string color);
    void showResults();
    vector<pair<Point, string>> getCenters() const;  // Updated to return Point and color
    pair<Point, string> detectAll(const vector<string> &selectedColors);  // Updated to return Point and color as a pair
    void setHsvRange(ConfigFile* cfg);

    Mat getMask();

   private:
    void preprocess(string color);
    void detectContours(const string& color);  // Updated to accept color as a parameter
    void getImage(const string& imagePath);

    Mat image;
    Mat hsv;
    Mat mask;
    Mat output;
    vector<pair<Point, string>> centers;  // Updated to store Point and color
    map<string, pair<Scalar, Scalar>> colorRanges;
};

#endif

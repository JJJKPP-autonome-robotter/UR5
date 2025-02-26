#ifndef PROCESSIMAGE_HPP
#define PROCESSIMAGE_HPP

#include <opencv2/opencv.hpp>
#include <iostream>
#include <vector>

using namespace std;
using namespace cv;

class ProcessImage {
public:
    ProcessImage(const string& imagePath);
    void detectRedMMS();
    void showResults();

private:
    Mat image;
    Mat mask;
    Mat output;

    void preprocess();
    void findContours();
};

#endif // PROCESSIMAGE_HPP

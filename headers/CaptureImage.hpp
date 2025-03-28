#ifndef CAPTUREIMAGE_HPP
#define CAPTUREIMAGE_HPP

#include <opencv2/opencv.hpp>
#include <iostream>
#include <string>

using namespace std;
using namespace cv;

class CaptureImage {
public:
    CaptureImage(int cameraIndex = 4);   // index er com nummer "dev/video4"
    ~CaptureImage();
    bool captureAndSave(const string& filename = "input.jpg");

private:
    VideoCapture cap;
};

#endif // CAPTUREIMAGE_HPP

// Camera.hpp
#ifndef CAMERA_HPP
#define CAMERA_HPP

#include <iostream>
#include <opencv2/opencv.hpp>
#include <vector>

class Camera {
public:
  Camera(const std::string &cameraPath);
  void run();

private:
  cv::VideoCapture cap;
  bool isRectangle(const std::vector<cv::Point> &contour);
};

#endif

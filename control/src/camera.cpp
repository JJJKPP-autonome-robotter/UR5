#include "../headers/camera.hpp"

Camera::Camera(const std::string& cameraPath) : cap(cameraPath) {
    if (!cap.isOpened()) {
        std::cerr << "Error: Cannot open camera" << std::endl;
        exit(-1);
    }
}

void Camera::run() {
    cv::Mat frame, hsv, mask, blurred;

    while (true) {
        cap >> frame;
        if (frame.empty()) break;

        // Convert frame to HSV color space
        cv::cvtColor(frame, hsv, cv::COLOR_BGR2HSV);

        // Enhanced range to detect red M&Ms in diverse lighting
        cv::Mat mask1, mask2;

        cv::inRange(hsv, cv::Scalar(0, 150, 100), cv::Scalar(10, 255, 255), mask1),
        cv::inRange(hsv, cv::Scalar(170, 150, 100), cv::Scalar(180, 255, 255), mask2),

        mask = mask1 | mask2;

        // Reduce noise and enhance contours
        cv::morphologyEx(mask, mask, cv::MORPH_OPEN, cv::Mat(), cv::Point(-1, -1), 2);
        cv::GaussianBlur(mask, blurred, cv::Size(5, 5), 0);

        std::vector<std::vector<cv::Point>> contours;
        cv::findContours(blurred, contours, cv::RETR_EXTERNAL, cv::CHAIN_APPROX_SIMPLE);

        for (auto &contour : contours) {
            double area = cv::contourArea(contour);
            if (area > 50) { // Lower threshold to detect smaller M&Ms
                // Draw outline of M&M
                cv::drawContours(frame, std::vector<std::vector<cv::Point>>{contour}, -1, cv::Scalar(0, 255, 0), 2);

                // Calculate centroid
                cv::Moments m = cv::moments(contour);
                if (m.m00 != 0) {
                    int cx = static_cast<int>(m.m10 / m.m00);
                    int cy = static_cast<int>(m.m01 / m.m00);
                    cv::circle(frame, cv::Point(cx, cy), 6, cv::Scalar(255, 0, 0), -1); // Bigger center dot
                }
            }
        }

        cv::imshow("Red M&M Detection", frame);
        if (cv::waitKey(10) == 27) break;
    }

    cap.release();
    cv::destroyAllWindows();
}

bool Camera::isRectangle(const std::vector<cv::Point>& contour) {
    double epsilon = 0.02 * cv::arcLength(contour, true);
    std::vector<cv::Point> approx;
    cv::approxPolyDP(contour, approx, epsilon, true);
    return (approx.size() == 4 && cv::isContourConvex(approx));
}

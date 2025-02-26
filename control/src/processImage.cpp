#include "../headers/processImage.hpp"

ProcessImage::ProcessImage(const string& imagePath) {
    image = imread(imagePath);
    if (image.empty()) {
        cerr << "Error: Could not open or find the image!" << endl;
        exit(1);
    }
    output = image.clone();
}

void ProcessImage::preprocess() {
    Mat hsv;
    cvtColor(image, hsv, COLOR_BGR2HSV);

    // Define red color range (two HSV ranges for red)
    Mat mask1, mask2;
    inRange(hsv, cv::Scalar(0, 150, 100), cv::Scalar(10, 255, 255), mask1),
    inRange(hsv, cv::Scalar(170, 150, 100), cv::Scalar(180, 255, 255), mask2),

    // Combine both masks
    mask = mask1 | mask2;
}

void ProcessImage::findContours() {
    vector<vector<Point>> contours;
    vector<Vec4i> hierarchy;
    cv::findContours(mask, contours, hierarchy, RETR_EXTERNAL, CHAIN_APPROX_SIMPLE);

    // Draw bounding boxes around detected red M&Ms
    for (const auto& contour : contours) {
        Rect boundingBox = boundingRect(contour);
        rectangle(output, boundingBox, Scalar(0, 255, 0), 2);
    }
}

void ProcessImage::detectRedMMS() {
    preprocess();
    findContours();
}

void ProcessImage::showResults() {
    imshow("Original Image", image);
    imshow("Red M&Ms Mask", mask);
    imshow("Detected Red M&Ms", output);
    waitKey(0);
}

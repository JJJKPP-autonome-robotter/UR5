#include "../headers/PixelToRobot.hpp"

// load image =D
PixelToRobot::PixelToRobot(const string& imagePath) { 
    image = imread(imagePath);
    if (image.empty()) {
        cerr << "Error: Could not open or find the image!" << endl;
        exit(1);
    }
    output = image.clone(); 
}


// Preprocess the image, enhancer image
void PixelToRobot::preprocess() {
    // convert image colors to hsv
    cvtColor(image, hsv, COLOR_BGR2HSV); 

    // Apply Gaussian blur to reduce noise
    GaussianBlur(hsv, hsv, Size(5, 5), 0); // size is how much to blur

    // define red color range
    // pixel in hsv range set to white else black
    Mat mask1, mask2;
    inRange(hsv, Scalar(0, 120, 100), Scalar(10, 255, 255), mask1);    // red near 0°      Scalar(hue, saturation, value)
    inRange(hsv, Scalar(170, 120, 100), Scalar(180, 255, 255), mask2); // red near 360°

    mask = mask1 | mask2; // combinere 2 masks (bitwise or)

    // Apply morphological operations to clean mask 
    // removes noise i 5x5 pixels
    Mat kernel = getStructuringElement(MORPH_RECT, Size(5, 5));
    morphologyEx(mask, mask, MORPH_CLOSE, kernel); // removes samll holes
    morphologyEx(mask, mask, MORPH_OPEN, kernel); // remove samll noise and isolate object
    dilate(mask, mask, kernel, Point(-1, -1), 2); // extra iterations if needed

}


// find contours
void PixelToRobot::detectContours() {  // Renamed from findContours
    vector<vector<Point>> contours;
    findContours(mask, contours, RETR_EXTERNAL, CHAIN_APPROX_SIMPLE);   // RETR_EXTERNAL = only external contours
                                                                        // CHAIN_APPROX_SIMPLE removes redundant points 

    const double minArea = 6000.0; // contour less than 6000 pixels ignored

    centers.clear();

    for (const auto& contour : contours) {  // for each contour

        // filter smal contours
        double area = contourArea(contour); 
        if (area < minArea) {  
            continue;
        }

        // NOTE: vi har brug for bedre måde end rectangle, prøv cirkel?
        Rect boundingBox = boundingRect(contour); // rectangel around contour   

        // if aspect ratio is less than maxAspectRatio, asume 1 center
        Point center(boundingBox.x + boundingBox.width / 2, boundingBox.y + boundingBox.height / 2);
        centers.push_back(center);

    }
    // draw centers on output
    for (const auto& center : centers) {
        circle(output, center, 3, Scalar(0, 255, 0), -1);  // size 3 and color green
    }
}


// combine
void PixelToRobot::calibrate() {
    preprocess();
    detectContours();
}

// show output
void PixelToRobot::showResults() {
    namedWindow("calibrate points", WINDOW_NORMAL);
    resizeWindow("calibrate points", 1280, 720);
    imshow("calibrate points", output);

    waitKey(0);
}

vector<Point> PixelToRobot::getCenters() const {
    return centers;
}

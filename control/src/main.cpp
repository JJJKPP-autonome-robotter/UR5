#include "../headers/captureImage.hpp"
#include "../headers/processImage.hpp"
#include "../headers/PixelToRobot.hpp"
#include <iostream>

using namespace std;  // Ensure we use std namespace


string imagePath = "/home/jeppe/GitHub/UR5/control/img/input40.png";  

// test main
int main() {

    // tager billed
    CaptureImage camera(0);
    
    if (camera.captureAndSave("input.jpg")) {
        cout << "Image successfully captured and saved!" << endl;
    } else {
        cerr << "Failed to capture image." << endl;
    }

    // 
    ProcessImage processor(imagePath);
    processor.detectRedMMS();
    //processor.showResults(); // DEBUG

    vector<Point> centers = processor.getCenters();
    cout << "Detected M&M centers:" << endl;
    for (const auto& center : centers) {
        cout << "(" << center.x << ", " << center.y << ")" << endl;
    }
   
    Point2f testPixel = centers[0];

   // beregn koordinater
    PixelToRobot pixelToRobot(imagePath);

    vector<Point2f> robot_points = {
        Point2f(-0.09295, -0.42590),
        Point2f(0.32133, -0.2504458),
        Point2f(-0.09295, -0.2504458)
    };

    pixelToRobot.calibrate(robot_points);

    //pixelToRobot.showResults(); // DEBUG
    Point2f robotCoord = pixelToRobot.transformPoint(testPixel);

    // Output result
    cout << "Pixel (" << testPixel.x << ", " << testPixel.y << ") -> "
        << "Robot (" << robotCoord.x << ", " << robotCoord.y << ")" << endl;

    
    return 0;
}

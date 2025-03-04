#include "../headers/captureImage.hpp"
#include "../headers/processImage.hpp"
#include <iostream>

using namespace std;  // Ensure we use std namespace
/*

int main() {
    CaptureImage camera;  // Open default camera (index 0)
    
    if (camera.captureAndSave("input.jpg")) {
        cout << "Image successfully captured and saved!" << endl;
    } else {
        cerr << "Failed to capture image." << endl;
    }

    ProcessImage processor("input.jpg"); // Replace with your image path
    processor.detectRedMMS();
    processor.showResults();

    return 0;
}
*/

// test main
int main() {
    ProcessImage processor("/home/jeppe/GitHub/UR5/control/img/input1.jpg");
    processor.detectRedMMS();
    processor.showResults();

    vector<Point> centers = processor.getCenters();
    cout << "Detected M&M centers:" << endl;
    for (const auto& center : centers) {
        cout << "(" << center.x << ", " << center.y << ")" << endl;
    }

    return 0;
}

#include "../headers/captureImage.hpp"
#include "../headers/processImage.hpp"
#include <iostream>

using namespace std;  // Ensure we use std namespace

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



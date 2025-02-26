#include "../headers/captureImage.hpp"

CaptureImage::CaptureImage(int cameraIndex) {
    cap.open(cameraIndex);
    if (!cap.isOpened()) {
        cerr << "Error: Could not open camera." << endl;
    }
}

CaptureImage::~CaptureImage() {
    cap.release();
}

bool CaptureImage::captureAndSave(const string& filename) {
    if (!cap.isOpened()) {
        cerr << "Error: Camera not opened." << endl;
        return false;
    }

    Mat frame;
    cap >> frame;

    if (frame.empty()) {
        cerr << "Error: Captured frame is empty." << endl;
        return false;
    }

    bool success = imwrite(filename, frame);
    if (success) {
        cout << "Image saved as " << filename << endl;
    } else {
        cerr << "Error: Could not save image." << endl;
    }

    return success;
}

#include "../headers/processImage.hpp"  // Include the header


// load image =D
ProcessImage::ProcessImage(const string& imagePath) { 
    image = imread(imagePath);
    if (image.empty()) {
        cerr << "Error: Could not open or find the image!" << endl;
        exit(1);
    }
    output = image.clone(); 
}


// Preprocess the image, enhancer image
void ProcessImage::preprocess() {
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

    //imshow("before morph", mask); //DEBUG

    // Apply morphological operations to clean mask 
    // removes noise i 5x5 pixels
    Mat kernel = getStructuringElement(MORPH_RECT, Size(5, 5));
    morphologyEx(mask, mask, MORPH_CLOSE, kernel); // removes samll holes
    morphologyEx(mask, mask, MORPH_OPEN, kernel); // remove samll noise and isolate object
    dilate(mask, mask, kernel, Point(-1, -1), 2); // extra iterations if needed

}


// find contours
void ProcessImage::detectContours() {  // Renamed from findContours
    vector<vector<Point>> contours;
    findContours(mask, contours, RETR_EXTERNAL, CHAIN_APPROX_SIMPLE);   // RETR_EXTERNAL = only external contours
                                                                        // CHAIN_APPROX_SIMPLE removes redundant points 

    const double minArea = 50.0; // contour less than 50 pixels ignored
    const double maxAspectRatio = 1.3; // contour with aspect ratio greater than 1.3 processed as 2 centers

    centers.clear();

    for (const auto& contour : contours) {  // for each contour

        // filter smal contours
        double area = contourArea(contour); 
        if (area < minArea) {  
            continue;
        }

        // NOTE: vi har brug for bedre måde end rectangle, prøv cirkel?
        Rect boundingBox = boundingRect(contour); // rectangel around contour

        // check aspect ratio
        double aspectRatio = static_cast<double>(max(boundingBox.width, boundingBox.height)) /
                             static_cast<double>(min(boundingBox.width, boundingBox.height));


        // if aspect ratio is greater than maxAspectRatio, asume 2 centers
        if (aspectRatio > maxAspectRatio) {

            // calculate centers
            // if width is greater than height
            if (boundingBox.width > boundingBox.height) {
                Point center1(boundingBox.x + boundingBox.width / 4, boundingBox.y + boundingBox.height / 2);
                Point center2(boundingBox.x + 3 * boundingBox.width / 4, boundingBox.y + boundingBox.height / 2);
                centers.push_back(center1);
                centers.push_back(center2);
            } else { 
                // if height is greater than width
                Point center1(boundingBox.x + boundingBox.width / 2, boundingBox.y + boundingBox.height / 4);
                Point center2(boundingBox.x + boundingBox.width / 2, boundingBox.y + 3 * boundingBox.height / 4);
                centers.push_back(center1);
                centers.push_back(center2);
            }
        } else {
            // if aspect ratio is less than maxAspectRatio, asume 1 center
            Point center(boundingBox.x + boundingBox.width / 2, boundingBox.y + boundingBox.height / 2);
            centers.push_back(center);
        }
    }


    // draw centers on output
    for (const auto& center : centers) {
        circle(output, center, 3, Scalar(0, 255, 0), -1);  // size 3 and color green
    }
}


// combine
void ProcessImage::detectRedMMS() {
    preprocess();
    detectContours();
}

// show output
void ProcessImage::showResults() {
    imshow("Original Image", image);
    imshow("HSV", hsv);
    imshow("Red M&Ms Mask", mask);
    imshow("Detected Red M&Ms", output);
    waitKey(0);
}

vector<Point> ProcessImage::getCenters() const {
    return centers;
}

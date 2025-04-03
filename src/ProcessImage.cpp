#include "../headers/ProcessImage.hpp"  // Include the header


// load image =D
ProcessImage::ProcessImage(const string& imagePath) { 
    image = imread(imagePath);
    if (image.empty()) {
        cerr << "Error: Could not open or find the image!" << endl;
        exit(1);
    }
    output = image.clone(); 


    colorRanges["red"] = { Scalar(0, 120, 100), Scalar(10, 255, 255) };
    colorRanges["red2"] = { Scalar(170, 120, 100), Scalar(180, 255, 255) };  // Second red range
    colorRanges["blue"] = {Scalar(100, 150, 100), Scalar(110, 255, 255)};
    colorRanges["green"] = {Scalar(50, 100, 160), Scalar(60, 180, 255)};
    colorRanges["orange"] = { Scalar(10, 180, 100), Scalar(18, 255, 255) };
    colorRanges["yellow"] = { Scalar(19, 150, 120), Scalar(26, 255, 255) };
    colorRanges["brown"] = { Scalar(8, 150, 40), Scalar(14, 255, 120) };
}


// Preprocess the image, enhancer image
void ProcessImage::preprocess(string color) {
    // convert image colors to hsv
    cvtColor(image, hsv, COLOR_BGR2HSV); 

    // Apply Gaussian blur to reduce noise
    GaussianBlur(hsv, hsv, Size(5, 5), 0); // size is how much to blur

    // pixel in hsv range set to white else black
    if (color == "red"){
        Mat mask1, mask2;
        inRange(hsv, colorRanges["red"].first, colorRanges["red"].second, mask1);    // red near 0°      Scalar(hue, saturation, value)
        inRange(hsv, colorRanges["red2"].first,  colorRanges["red2"].second, mask2); // red near 360°

        mask = mask1 | mask2; // combinere 2 masks (bitwise or)
    } else{
        inRange(hsv, colorRanges[color].first,  colorRanges[color].second, mask);   
    }
    //imshow("before morph", mask); //DEBUG

    // Apply morphological operations to clean mask 
    // removes noise i 5x5 pixels
    Mat kernel = getStructuringElement(MORPH_RECT, Size(5, 5));
    morphologyEx(mask, mask, MORPH_CLOSE, kernel); // removes samll holes
    morphologyEx(mask, mask, MORPH_OPEN, kernel); // remove samll noise and isolate object
    dilate(mask, mask, kernel, Point(-1, -1), 2); // extra iterations if needed

}


// vector of points with color
vector<pair<Point, string>> centers;


void ProcessImage::detectContours(const string& color) {
    vector<vector<Point>> contours;
    findContours(mask, contours, RETR_EXTERNAL, CHAIN_APPROX_SIMPLE);

    const double minArea = 1000.0;
    const double maxArea = 4000.0;
    const double maxAspectRatio = 1.40;

    for (const auto& contour : contours) {
        double area = contourArea(contour);
        if (area < minArea || area > maxArea) {
            continue;
        }

        Rect boundingBox = boundingRect(contour);
        double aspectRatio = static_cast<double>(max(boundingBox.width, boundingBox.height)) /
                             static_cast<double>(min(boundingBox.width, boundingBox.height));

        if (aspectRatio > maxAspectRatio) {
            if (boundingBox.width > boundingBox.height) {
                Point center1(boundingBox.x + boundingBox.width / 4, boundingBox.y + boundingBox.height / 2);
                Point center2(boundingBox.x + 3 * boundingBox.width / 4, boundingBox.y + boundingBox.height / 2);
                centers.emplace_back(center1, color);
                centers.emplace_back(center2, color);
            } else {
                Point center1(boundingBox.x + boundingBox.width / 2, boundingBox.y + boundingBox.height / 4);
                Point center2(boundingBox.x + boundingBox.width / 2, boundingBox.y + 3 * boundingBox.height / 4);
                centers.emplace_back(center1, color);
                centers.emplace_back(center2, color);
            }
        } else {
            Point center(boundingBox.x + boundingBox.width / 2, boundingBox.y + boundingBox.height / 2);
            centers.emplace_back(center, color);
        }
    }

    for (const auto& center : centers) {
        circle(output, center.first, 3, Scalar(0, 255, 0), -1);
    }
}


// detect specific color
// stored in centers
void ProcessImage::detectMMS(string color) {
    centers.clear(); // clear previous centers
    preprocess(color);
    detectContours(color);
}

// detectAll to return the point with the color
pair<Point, string> ProcessImage::detectAll(const vector<string>& selectedColors) {
    centers.clear(); // clear previous points

    for (const auto &color : selectedColors) {
        preprocess(color);
        detectContours(color); 
    }

    // sort centers by y and x coordinates
    sort(centers.begin(), centers.end(), [](const pair<Point, string>& a, const pair<Point, string>& b) {
        return (a.first.y < b.first.y) || (a.first.y == b.first.y && a.first.x < b.first.x);
    });

    // return the first center with its color
    if (centers.empty()) {
        throw runtime_error("No centers detected.");
    }
    return centers[0]; // Return the first center and color
}

// show output
void ProcessImage::showResults() {
    //imshow("Original Image", image);
    //imshow("HSV", hsv);
    
    namedWindow("Red M&Ms Mask", WINDOW_NORMAL);
    resizeWindow("Red M&Ms Mask", 1280, 720);
    imshow("Red M&Ms Mask", mask);

    namedWindow("Detected Red M&Ms", WINDOW_NORMAL);
    resizeWindow("Detected Red M&Ms", 1280, 720);
    imshow("Detected Red M&Ms", output);

    waitKey(0);
    destroyAllWindows();
}

// Update getCenters to return the updated structure
vector<pair<Point, string>> ProcessImage::getCenters() const {
    return centers;
}

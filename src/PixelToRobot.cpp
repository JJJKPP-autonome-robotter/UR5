#include "../headers/PixelToRobot.hpp"

#include "../headers/Data_saver.hpp"

// load image =D
PixelToRobot::PixelToRobot(const string& imagePath) {
    image = imread(imagePath);
    if (image.empty()) {
        cerr << "Error: Could not open or find the image!" << endl;
        exit(1);
    }
    output = image.clone();
}

void PixelToRobot::setHsvRange(ConfigFile* cfg) {
    red = cfg->get<vector<vector<int>>>("color_ranges", "red");                        // Get the first red range
    vector<vector<int>> red2 = cfg->get<vector<vector<int>>>("color_ranges", "red2");  // Get the second red range

    // Combine red2 into red
    red.insert(red.end(), red2.begin(), red2.end());

    // Print the combined red ranges for debugging
    cout << "Combined red ranges:" << endl;
    for (const auto& range : red) {
        cout << "[" << range[0] << ", " << range[1] << ", " << range[2] << "]" << endl;
    }
}

// Preprocess the image, enhancer image
void PixelToRobot::preprocess() {
    // convert image colors to hsv
    cvtColor(image, hsv, COLOR_BGR2HSV);

    // Apply Gaussian blur to reduce noise
    GaussianBlur(hsv, hsv, Size(5, 5), 0);  // size is how much to blur

    // define red color range
    // pixel in hsv range set to white else black
    Mat mask1, mask2;
    inRange(hsv, Scalar(red[0][0], red[0][1], red[0][2]), Scalar(red[1][0], red[1][1], red[1][2]), mask1);  // red near 0°      Scalar(hue, saturation, value)
    inRange(hsv, Scalar(red[2][0], red[2][1], red[2][2]), Scalar(red[3][0], red[3][1], red[3][2]), mask2);  // red near 360°

    mask = mask1 | mask2;  // combinere 2 masks (bitwise or)

    // Apply morphological operations to clean mask
    // removes noise i 5x5 pixels
    Mat kernel = getStructuringElement(MORPH_RECT, Size(5, 5));
    morphologyEx(mask, mask, MORPH_CLOSE, kernel);  // removes samll holes
    morphologyEx(mask, mask, MORPH_OPEN, kernel);   // remove samll noise and isolate object
    dilate(mask, mask, kernel, Point(-1, -1), 2);   // extra iterations if needed
}

// find contours
void PixelToRobot::detectContours() {  // Renamed from findContours
    vector<vector<Point>> contours;
    findContours(mask, contours, RETR_EXTERNAL, CHAIN_APPROX_SIMPLE);  // RETR_EXTERNAL = only external contours
                                                                       // CHAIN_APPROX_SIMPLE removes redundant points

    const double minArea = 3000.0;  // contour less than 6000 pixels ignored
    const double maxArea = 10000.0;

    centers.clear();

    for (const auto& contour : contours) {  // for each contour

        // filter smal contours
        double area = contourArea(contour);
        if (area < minArea || area > maxArea) {
            continue;
        }

        // NOTE: vi har brug for bedre måde end rectangle, prøv cirkel?
        Rect boundingBox = boundingRect(contour);  // rectangel around contour

        // if aspect ratio is less than maxAspectRatio, asume 1 center
        Point center(boundingBox.x + boundingBox.width / 2, boundingBox.y + boundingBox.height / 2);
        centers.push_back(center);
    }

    // afstand fra 0,0
    auto distanceFromOrigin = [](const Point& p) {
        return p.x * p.x + p.y * p.y;
    };

    // sort centers
    sort(centers.begin(), centers.end(), [&](const Point& a, const Point& b) {
        return distanceFromOrigin(a) < distanceFromOrigin(b);
    });

    cout << "Sorted Centers:" << std::endl;
    for (size_t i = 0; i < centers.size(); ++i) {
        cout << "Center " << i + 1 << ": (" << centers[i].x << ", " << centers[i].y << ")" << std::endl;
        putText(output, std::to_string(i + 1), centers[i], FONT_HERSHEY_SIMPLEX, 1, Scalar(0, 255, 0), 2);
    }
}

// combine
void PixelToRobot::calibrate(ConfigFile* cfg) {
    setHsvRange(cfg);
    preprocess();
    detectContours();

    // Ensure we have exactly 3 detected points before computing transformation
    if (centers.size() != 3) {
        cerr << "Error: Calibration failed. Need exactly 3 detected centers, but found " << centers.size() << "." << endl;
        return;
    }

    cout << "Calibration done successfully!" << endl;
}
// show output
void PixelToRobot::showResults() {
    namedWindow("calibrate points", WINDOW_NORMAL);
    resizeWindow("calibrate points", 1280, 720);
    imshow("calibrate points", output);

    waitKey(0);
    destroyAllWindows();
}

void PixelToRobot::saveImageWithCenters(const string& filename) {
    // Save the output image with center dots
    imwrite(filename, output);
    cout << "Saved Calibration image as " << filename << endl;
}

vector<Point2f> PixelToRobot::getCenters() const {
    return centers;
}

// Constructor: Computes the affine transformation matrix
void PixelToRobot::computeTransformation(const vector<double>& refPoint1, const vector<double>& refPoint2, const vector<double>& refPoint3) {
    vector<Point2f> robotPoints = {
        Point2f(refPoint1[0], refPoint1[1]),
        Point2f(refPoint2[0], refPoint2[1]),
        Point2f(refPoint3[0], refPoint3[1])};

    if (centers.size() == 3 && centers.size() == 3) {
        affineMatrix = getAffineTransform(centers, robotPoints);
    } else {
        throw runtime_error("Exactly 3 points are required for affine transformation.");
    }
}

// Function to transform a pixel coordinate to a robot coordinate
Point2f PixelToRobot::transformPoint(const Point2f& pixelPoint) const {
    if (affineMatrix.empty()) {
        throw runtime_error("Transformation matrix not set. Call computeTransformation() first.");
    }

    Mat src = (Mat_<double>(3, 1) << pixelPoint.x, pixelPoint.y, 1);
    Mat dst = affineMatrix * src;

    return Point2f(dst.at<double>(0, 0), dst.at<double>(1, 0));
}
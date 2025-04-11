#include <iostream>
#include <string>
#include <vector>

#include "../headers/systemInit.hpp"


using namespace std;

void mainLoop(bool dbActive) {
    vector<string> selectedColors = cfg.get<vector<string>>("cvCfg", "colorToPick");

    while (true) {
        pair<Point, string> target = captureAndProcess(selectedColors);

        Point mmCenter = target.first;
        string color = target.second;


        if (color.empty() || mmCenter.x == 0 || mmCenter.y == 0) {
            cerr << "Invalid target detected" << endl;
            continue;
        }

        // Transform pixel to robot coordinates
        Point2f robotCoord = pixelToRobot->transformPoint(mmCenter);
        cout << "Pixel (" << mmCenter.x << ", " << mmCenter.y << ") -> "
             << "Robot (" << robotCoord.x << ", " << robotCoord.y << ")" << endl;

        vector<double> mm = {robotCoord.x, robotCoord.y};
        ur5->pickUp(color, mm);

        if (dbActive) {
            string& dbColor = color;
            vector<double>& dbRealCoord = mm;
            vector<double> dbPicCoord = {static_cast<double>(mmCenter.x), static_cast<double>(mmCenter.y)};
            string image = cfg.get<string>("vcCfg","imagePath");
            cv::Mat mask = 
            
            db.logEvent(dbColor, dbRealCoord, dbPicCoord, image, mask);
        }
    }
}

int main() {
    initializeRobot();
    calibrateSystem();
    bool dbActive = cfg.get<bool>("dataLogger","active");
    if (dbActive) DataLogger db = initDataLogger();
    mainLoop(dbActive);
    return 0;
}
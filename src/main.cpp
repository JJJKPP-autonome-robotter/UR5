#include <iostream>
#include <string>
#include <vector>

#include "../headers/systemInit.hpp"
#include <vector>

using namespace std;

void mainLoop(bool dbActive, DataLogger& db) {
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
        bool pickup = false;
        pickup = ur5->pickUp(color, mm);

        if (dbActive) {
            cv::Mat mask = processor.getMask();
            db.logEvent(color, pickup, mm, mmCenter, mask)
        }
    }
}

int main() {
    initializeRobot();
    calibrateSystem();

    bool dbActive = cfg.get<bool>("dataLogger","active");
    DataLogger db = dbActive ? DataLogger(cfg) : DataLogger();
    
    mainLoop(dbActive, db);
    return 0;
}
#include <iostream>
#include <string>
#include <vector>

#include "systemInit.cpp"

using namespace std;

void mainLoop() {
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
    }
}

int main() {
    initializeRobot();
    calibrateSystem();
    mainLoop();
    return 0;
}
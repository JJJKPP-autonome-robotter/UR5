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
            string& dbColor = color;
            vector<double>& dbRealCoord = mm;
            vector<double> dbPicCoord = {static_cast<double>(mmCenter.x), static_cast<double>(mmCenter.y)};
            string image = cfg.get<string>("cvCfg","imagePath");
            cv::Mat mask = processor.getMask();

            vector<double> hsvUpper;
            vector<double> hsvLower;

            if (color == "red") {
                cout << "REDSAVE" << endl;
                vector<vector<double>> range1 = cfg.get<vector<vector<double>>>("color_ranges","red");
                vector<vector<double>> range2 = cfg.get<vector<vector<double>>>("color_ranges","red2");
                hsvLower.insert(hsvLower.end(), range1[0].begin(), range1[0].end());
                hsvLower.insert(hsvLower.end(), range2[0].begin(), range2[0].end());
                hsvUpper.insert(hsvUpper.end(), range1[1].begin(), range1[1].end());
                hsvUpper.insert(hsvUpper.end(), range2[1].begin(), range2[1].end());

                cout << "REDSAVEDONE" << endl;

            } else {
                cout << "OTHER COLOR SAVE" << endl;
                vector<vector<double>> range = cfg.get<vector<vector<double>>>("color_ranges", color);
                hsvLower.insert(hsvLower.end(), range[0].begin(), range[0].end());
                hsvUpper.insert(hsvUpper.end(), range[1].begin(), range[1].end());
                cout << "OTHER COLOR DONE" << endl; 
            }

            db.logEvent(dbColor, pickup, dbRealCoord, dbPicCoord, hsvLower, hsvUpper, image, mask);
            cout << "Saved data" << endl;
        }
    }
}

int main() {
    initializeRobot();
    calibrateSystem();

    bool dbActive = cfg.get<bool>("dataLogger","active");
    DataLogger db = dbActive ? initDataLogger() : DataLogger();
    
    mainLoop(dbActive, db);
    return 0;
}
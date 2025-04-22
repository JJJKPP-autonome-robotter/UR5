#include <iostream>
#include <string>
#include <vector>


#include "../headers/systemInit.hpp"

#include "../headers/MainWindow.hpp"

#include <QApplication>
#include <QTimer>
#include <thread>

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
        ur5->pickUp(color, mm);

        if (dbActive) {
            string& dbColor = color;
            vector<double>& dbRealCoord = mm;
            vector<double> dbPicCoord = {static_cast<double>(mmCenter.x), static_cast<double>(mmCenter.y)};
            string image = cfg.get<string>("cvCfg","imagePath");
            cv::Mat mask = processor.getMask();

            db.logEvent(dbColor, dbRealCoord, dbPicCoord, image, mask);
            cout << "Saved data" << endl;
        }
    }
}

int robotLogic() {
    initializeRobot();
    calibrateSystem();

    bool dbActive = cfg.get<bool>("dataLogger","active");
    DataLogger db = dbActive ? initDataLogger() : DataLogger();
    
    mainLoop(dbActive, db);
    return 0;
}

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);

    MainWindow gui;
    gui.show();

    // Launch robot logic in separate thread
    std::thread robotThread(runRobotLogic);

    // Start GUI loop
    int result = app.exec();

    // Join the robot thread before exiting
    if (robotThread.joinable())
    {
        robotThread.join();
    }
    
    return result;
}
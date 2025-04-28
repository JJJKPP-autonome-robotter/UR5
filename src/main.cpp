#include <iostream>
#include <string>
#include <vector>
#include <algorithm>



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
        bool pickup = false;
        pickup = ur5->pickUp(color, mm);

        if (dbActive) {
            vector<cv::Mat> masks = processor.getMask();
            auto it = find(selectedColors.begin(), selectedColors.end(), color);
            if (it != selectedColors.end()) {
                size_t index = distance(selectedColors.begin(), it);
                cv::Mat mask = masks[index];
                db.logEvent(color, pickup, mm, mmCenter, mask);
            }
            
        }
    }
}

int robotLogic() {
    initializeRobot();
    calibrateSystem();

    bool dbActive = cfg.get<bool>("dataLogger","active");
    DataLogger db = dbActive ? DataLogger(&cfg) : DataLogger();
    
    mainLoop(dbActive, db);
    return 0;
}

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);

    MainWindow gui;
    gui.show();

    QTimer *timer = new QTimer(&gui);
    QObject::connect(timer, &QTimer::timeout, &gui, &MainWindow::refreshImage);
    timer->start(1000); // 1000 ms = 1 second

    // Launch robot logic in separate thread
    //thread robotThread(robotLogic);

    // Start GUI loop
    int result = app.exec();

    /*
    // Join the robot thread before exiting
    if (robotThread.joinable())
    {
        robotThread.join();
    }
    */
    return result;
}